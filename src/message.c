#include "dns.h"

#include <arpa/inet.h>
#include <errno.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BUF 1024

struct header create_header()
{
    srand(time(NULL));

    struct header hdr = {};
    memset(&hdr, 0, sizeof(struct header));
    hdr.id = rand() % 0b1111111111111111;
    hdr.qr = 1;
    hdr.rd = 1;
    hdr.qdcount = 1;
    hdr.ancount = 0;
    hdr.nscount = 0;
    hdr.arcount = 0;

    return hdr;
}

uint8_t create_request(struct question *question, void *buf, uint16_t buf_size)
{
    struct header r = create_header();
    int header_length = write_header(buf, r);
    int length = write_question(buf + header_length, question);
    return header_length + length;
}

int resolv(const struct request r, struct response *rsp)
{
    if (r.pr == udp)
    {
        return resolv_udp(r, rsp);
    }

    return resolv_tls(r, rsp);
}

bool ssl_config(SSL_CTX *ctx, char *ca, char *certificate)
{
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    if (!SSL_CTX_load_verify_locations(ctx, ca, NULL))
    {
        ERR_print_errors_fp(stderr);
        return false;
    }

    if (SSL_CTX_use_certificate_file(ctx, certificate, SSL_FILETYPE_PEM) <= 0)
    {
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}

int create_socket(const char *hostname, int port)
{
    int s;
    struct hostent *host;
    struct sockaddr_in addr;
    if ((host = gethostbyname(hostname)) == NULL)
    {
        perror("error gethostbyname");
        return -1;
    }
    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long *)(host->h_addr);
    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        close(s);
        perror("error open a connection");
        return -1;
    }
    return s;
}

int resolv_tls(const struct request r, struct response *rsp)
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == NULL)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    int server = create_socket(r.addr, r.port);
    if (server == -1)
    {
        return -1;
    }

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server);
    if (!ssl_config(ctx, r.ca, r.certificate))
    {
        return -1;
    }

    if (SSL_connect(ssl) != 1)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    struct question q = {r.qname, r.type, 1};
    void *buf = malloc(1024);
    if (buf == NULL)
    {
        perror("Cannot allocate memory");
        return -1;
    }

    int s = create_request(&q, buf, 1024);
    void *send_buf;
    send_buf = malloc(s + 2);
    if (send_buf == NULL)
    {
        perror("Cannot allocate memory");
        return -1;
    }
    write_uint16_t(send_buf, s);
    memcpy(send_buf + 2, buf, s);

    int w = SSL_write(ssl, send_buf, s + 2);
    if (w <= 0)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    int bytes = SSL_read(ssl, buf, 2);
    if (bytes <= 0)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    int buf_size = read_uint16_t(buf);

    bytes = SSL_read(ssl, buf, buf_size);
    if (bytes <= 0)
    {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    *rsp = get_response(buf, bytes);
    SSL_free(ssl);
    return 0;
}

int resolv_udp(const struct request r, struct response *rsp)
{
    char *buffer = malloc(sizeof(char) * 4096);
    if (buffer == NULL)
    {
        perror("Cannot allocate memory");
        return -1;
    }
    int sockfd;
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int result = getaddrinfo(r.addr, "53", &hints, &res);
    if (result != 0)
    {
        return -1;
    }
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    int n;
    unsigned int len;
    struct question q;
    q.qname = r.qname;
    q.qtype = r.type;
    q.qclass = 1;
    void *buf;
    buf = malloc(1024);
    if (buf == NULL)
    {
        perror("Cannot allocate memory");
        exit(1);
    }
    int s = create_request(&q, buf, 1024);
    result = sendto(sockfd, buf, s, MSG_CONFIRM, res->ai_addr, res->ai_addrlen);
    free(buf);

    n = recvfrom(sockfd, (char *)buffer, BUF, MSG_WAITALL, res->ai_addr, &len);
    buffer[n] = '\0';
    close(sockfd);

    *rsp = get_response(buffer, n);
    free(buffer);
    return 0;
}

struct response get_response(void *buffer, int len)
{
    struct response result;

    memset(&result, 0, sizeof(struct response));

    read_header(buffer, &(result.hdr));

    int pos = 12;
    if (result.hdr.qdcount > 0)
    {
        result.questions = malloc(sizeof(struct question) * result.hdr.qdcount);
        for (int i = 0; i < result.hdr.qdcount; i++)
        {
            pos = read_question(buffer, pos, &result.questions[i]);
        }
    }

    if (result.hdr.ancount > 0)
    {
        result.answers = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.ancount);
        for (int i = 0; i < result.hdr.ancount; i++)
        {
            pos = read_resource_record(buffer, pos, &(result.answers[i]));
        }
    }

    if (result.hdr.nscount > 0)
    {
        result.authority_records = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.nscount);
        for (int i = 0; i < result.hdr.nscount; i++)
        {
            pos = read_resource_record(buffer, pos, &(result.authority_records[i]));
        }
    }

    if (result.hdr.arcount > 0)
    {
        result.additional_records = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.arcount);
        for (int i = 0; i < result.hdr.arcount; i++)
        {
            pos = read_resource_record(buffer, pos, &(result.additional_records[i]));
        }
    }

    return result;
}

void free_response(struct response resp)
{
    for (int i = 0; i < resp.hdr.qdcount; i++)
    {
        free(resp.questions[i].qname);
    }
    if (resp.hdr.qdcount > 0)
    {
        free(resp.questions);
    }

    for (int i = 0; i < resp.hdr.ancount; i++)
    {
        free(resp.answers[i].name);
        free(resp.answers[i].rdata);
    }
    if (resp.hdr.ancount > 0)
    {
        free(resp.answers);
    }

    for (int i = 0; i < resp.hdr.nscount; i++)
    {
        free(resp.authority_records[i].name);
        free(resp.authority_records[i].rdata);
    }
    if (resp.hdr.nscount > 0)
    {
        free(resp.authority_records);
    }

    for (int i = 0; i < resp.hdr.arcount; i++)
    {
        free(resp.additional_records[i].name);
        free(resp.additional_records[i].rdata);
    }

    if (resp.hdr.arcount > 0)
    {
        free(resp.additional_records);
    }
}