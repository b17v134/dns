#include "dns.h"

#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

bool ssl_config(SSL_CTX* ctx, char* ca, char* certificate)
{
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    if (!SSL_CTX_load_verify_locations(ctx, ca, NULL)) {
        ERR_print_errors_fp(stderr);
        return false;
    }

    if (SSL_CTX_use_certificate_file(ctx, certificate, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return false;
    }
    return true;
}

int create_socket(const char* hostname, int port)
{
    int s;
    struct hostent* host;
    struct sockaddr_in addr;
    if ((host = gethostbyname(hostname)) == NULL) {
        perror("error gethostbyname");
        return -1;
    }
    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        close(s);
        perror("error open a connection");
        return -1;
    }
    return s;
}

int resolv_tls(const struct request r, struct response* rsp)
{
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD* method = TLS_client_method();
    SSL_CTX* ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    int server = create_socket(r.addr, r.port);
    if (server == -1) {
        return -1;
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server); // @todo: check SSL_set_fd result.
    if (!ssl_config(ctx, r.ca, r.certificate)) {
        return -1;
    }

    if (SSL_connect(ssl) != 1) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    struct question q = { r.qname, r.type, 1 };
    void* buf = malloc(1024);
    if (buf == NULL) {
        perror("Cannot allocate memory");
        return -1;
    }

    int s = create_request(&q, buf, 1024);
    uint8_t* send_buf;
    send_buf = malloc(s + 2);
    if (send_buf == NULL) {
        perror("Cannot allocate memory");
        return -1;
    }
    write_uint16_t(send_buf, s);
    memcpy(send_buf + 2, buf, s);

    int w = SSL_write(ssl, send_buf, s + 2);
    if (w <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    int bytes = SSL_read(ssl, buf, 2);
    if (bytes <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    int buf_size = read_uint16_t(buf);

    bytes = SSL_read(ssl, buf, buf_size);
    if (bytes <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    *rsp = get_response(buf, bytes);
    SSL_free(ssl);
    return 0;
}
