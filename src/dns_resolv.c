#include "dns_resolv.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define BUF 1024

uint16_t get_flags(uint8_t qr, uint8_t rd)
{
    return ((qr & 0b1) << 16) + ((rd & 0b1) << 8);
}

void write_uint16_t(void *buf, uint16_t value)
{
    *(uint8_t *)(buf) = (value & 0xFF00) >> 8;
    *(uint8_t *)(buf + 1) = value & 0xFF;
}

int write_qname(void *buf, char *qname)
{
    int length = strlen(qname);

    // Write qname.
    int prev = 0;
    int size = 0;
    int cur_pos = 0;
    for (int pos = 0; pos < length; pos++)
    {
        if (qname[pos] == '.')
        {
            size = pos - prev;
            *(uint8_t *)(buf + cur_pos) = size;
            memcpy(buf + cur_pos + 1, &(qname[prev]), size);
            prev = pos + 1;
            cur_pos += size + 1;
        }
    }
    *(uint8_t *)(buf + cur_pos) = 0;

    return cur_pos;
}

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

int write_header(void *buf, struct header h)
{
    uint16_t flags = get_flags(h.qr, h.rd);
    write_uint16_t(buf, h.id);
    write_uint16_t(buf + 2, flags);
    write_uint16_t(buf + 4, h.qdcount);
    write_uint16_t(buf + 6, h.ancount);
    write_uint16_t(buf + 8, h.nscount);
    write_uint16_t(buf + 10, h.arcount);

    return 12;
}

int write_question(void *buf, struct question *q)
{
    int length = write_qname(buf, q->qname);
    write_uint16_t(buf + length + 1, q->qtype);
    write_uint16_t(buf + length + 3, q->qclass);

    return length + 5;
}

uint8_t create_request(struct question *question, void *buf, uint16_t buf_size)
{
    struct header r = create_header();
    int header_length = write_header(buf, r);
    int length = write_question(buf + header_length, question);
    puts("created request");
    return header_length + length;
}

int resolv(const struct request r, char *buffer)
{
    int sockfd;
    struct sockaddr_in servaddr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return -1;
    }
    printf("addr = %s\n", r.addr);
    printf("port = %d\n", r.port);
    printf("qname = %s\n", r.qname);
    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(r.port);
    servaddr.sin_addr.s_addr = inet_addr(r.addr);

    int n, len;
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
    puts("create request");
    int s = create_request(&q, buf, 1024);
    puts("send request");
    puts("buffer = ");
    for (int i = 0; i < s; i++)
    {
        void *addr = buf + i;
        printf("%c ", addr);
    }
    puts("\n");
    size_t result = sendto(sockfd, buf, s, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    free(buf);
    printf("Request sent size = %d. result = %d\n", s, result);

    n = recvfrom(sockfd, (char *)buffer, BUF, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
    printf("Server : %d\n", n);
    close(sockfd);
    return n;
}

uint16_t read_uint8_t(void *buf)
{
    return (*(uint8_t *)buf);
}

uint16_t read_uint16_t(void *buf)
{
    return (*(uint8_t *)buf) * 0x100 + (*(uint8_t *)(buf + 1));
}

uint32_t read_uint32_t(void *buf)
{
    return (*(uint8_t *)buf) * 0x1000000 +
           (*(uint8_t *)(buf + 1)) * 0x10000 +
           (*(uint8_t *)(buf + 2)) * 0x100 +
           (*(uint8_t *)(buf + 3));
}

void read_ipv4(void *buf, char *ip)
{
    sprintf(
        ip,
        "%d.%d.%d.%d",
        read_uint8_t(buf),
        read_uint8_t(buf + 1),
        read_uint8_t(buf + 2),
        read_uint8_t(buf + 3));
}

void read_ipv6(void *buf, char *ip)
{
    char num[3];
    ip[0] = '\0';

    for (int i = 0; i < 16; i++)
    {
        sprintf(num, "%02x", read_uint8_t(buf + i));
        strcat(ip, num);
        if ((i % 2 == 1) && (i < 15))
        {
            strcat(ip, ":");
        }
    }
}

struct response get_response(void *buffer, int len)
{
    puts("");
    for (int i = 0; i < len; i++)
    {
        printf("%02x ", *(uint8_t *)(buffer + i));
    }
    puts("");
    for (int i = 0; i < len; i++)
    {
        printf("%c", *(char *)(buffer + i));
    }
    puts("");
    struct response result = {0};

    result.hdr.id = read_uint16_t(buffer);
    uint16_t flags = read_uint16_t(buffer + 2);
    result.hdr.qdcount = read_uint16_t(buffer + 4);
    result.hdr.ancount = read_uint16_t(buffer + 6);
    result.hdr.nscount = read_uint16_t(buffer + 8);
    result.hdr.arcount = read_uint16_t(buffer + 10);
    printf("result.hdr.id = %x\n", result.hdr.id);
    printf("result.hdr.qdcount = %d\n", result.hdr.qdcount);
    printf("result.hdr.ancount = %d\n", result.hdr.ancount);
    printf("result.hdr.nscount = %d\n", result.hdr.nscount);
    printf("result.hdr.arcount = %d\n", result.hdr.arcount);

    int pos = 12;
    result.questions = malloc(sizeof(struct question) * result.hdr.qdcount);
    for (int i = 0; i < result.hdr.qdcount; i++)
    {
        char *qname = NULL;
        uint8_t length;
        while (length = *(uint8_t *)(buffer + pos))
        {
            printf("length = %d\n", length);
            if (qname == NULL)
            {
                qname = (char *)malloc(sizeof(char) * (length + 1));
                memcpy(qname, buffer + pos + 1, length);
            }
            else
            {
                qname = (char *)realloc(qname, sizeof(char) * (length + 1));
                strncat(qname, buffer + pos + 1, length);
            }

            printf("qname = %s\n", qname);
            strcat(qname, ".");
            printf("qname = %s\n", qname);
            pos += length + 1;
            printf("pos = %d\n", pos);
            puts("===========");
        }
        result.questions[i].qname = qname;
        result.questions[i].qtype = read_uint16_t(buffer + pos + 1);
        printf("qtype = %d\n", result.questions[i].qtype);
        result.questions[i].qclass = read_uint16_t(buffer + pos + 3);
        printf("qclass = %d\n", result.questions[i].qclass);
        pos += 5;
    }
    printf("current pos = %d\n", pos);
    result.answers = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.ancount);
    for (int i = 0; i < result.hdr.ancount; i++)
    {
        char *qname = NULL;
        uint8_t length;
        uint8_t tmp_pos = pos;
        while (length = *(uint8_t *)(buffer + tmp_pos))
        {
            printf("answer length = %d\n", length);
            if (((length & 0b11000000) >> 6) == 0b11)
            {
                puts("pointer");
                printf("%02x %02x\n", *(uint8_t *)(buffer + tmp_pos), *(uint8_t *)(buffer + tmp_pos + 1));
                tmp_pos = read_uint16_t(buffer + tmp_pos) - 0b1100000000000000;
                printf("tmp_pos = %d\n", tmp_pos);

                length = *(uint8_t *)(buffer + tmp_pos);
                printf("length = %d\n", length);
            }
            if (qname == NULL)
            {
                qname = (char *)malloc(sizeof(char) * (length + 1));
                memcpy(qname, buffer + tmp_pos + 1, length);
            }
            else
            {
                qname = (char *)realloc(qname, sizeof(char) * (length + 1));
                strncat(qname, buffer + tmp_pos + 1, length);
            }

            printf("name = %s\n", qname);
            strcat(qname, ".");
            printf("name = %s\n", qname);
            tmp_pos += length + 1;
            printf("pos = %d\n", pos);
            puts("===========");
        }
        result.answers[i].name = qname;
        pos += 1;
        printf("current pos after name = %d\n", pos);
        result.answers[i].type = read_uint16_t(buffer + pos + 1);
        printf("type = %d\n", result.answers[i].type);
        result.answers[i].class = read_uint16_t(buffer + pos + 3);
        printf("class = %d\n", result.answers[i].class);
        result.answers[i].ttl = read_uint32_t(buffer + pos + 5);
        printf("ttl = %d\n", result.answers[i].ttl);
        result.answers[i].rdlength = read_uint16_t(buffer + pos + 9);
        int tmp = result.answers[i].rdlength;
        printf("rdlength = %d\n", tmp);
        if (result.answers[i].type == DNS_TYPE_A)
        {
            result.answers[i].rdata = malloc(sizeof(char) * 16);
            read_ipv4(buffer + pos + 11, result.answers[i].rdata);
        }
        if (result.answers[i].type == DNS_TYPE_AAAA)
        {
            result.answers[i].rdata = malloc(sizeof(char) * 50); // @todo: fix size
            read_ipv6(buffer + pos + 11, result.answers[i].rdata);
        }
        printf("rdata = %s\n", result.answers[i].rdata);
        pos += 11 + tmp;
    }
    return result;
}

void strupr(const char *str, char *result)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        result[i] = toupper(str[i]);
    }
    result[strlen(str) + 1] = '\0';
}

#define DNS_CHECK_TYPE(strValue, value)        \
    {                                          \
        if (strcmp(dns_type, (strValue)) == 0) \
        {                                      \
            result = (value);                  \
            goto finish;                       \
        }                                      \
    }

uint16_t dns_type_to_int(const char *type)
{
    int size = strlen(type) + 1;
    char *dns_type = malloc(sizeof(char) * size);
    if (dns_type == NULL)
    {
        perror("Cannot allocate memory");
        exit(1);
    }

    strupr(type, dns_type);

    uint16_t result = DNS_TYPE_ERROR;

    DNS_CHECK_TYPE("A", DNS_TYPE_A)
    DNS_CHECK_TYPE("NS", DNS_TYPE_NS)
    DNS_CHECK_TYPE("MD", DNS_TYPE_MD)
    DNS_CHECK_TYPE("AAAA", DNS_TYPE_AAAA)

finish:
    free(dns_type);
    return result;
}