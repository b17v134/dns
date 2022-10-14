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

uint8_t create_request(struct question *question, void *buf, uint16_t buf_size)
{
    uint16_t index = 0;

    srand(time(NULL));

    struct header r;
    memset(&r, 0, sizeof(struct header));
    r.id = rand() % 0b1111111111111111;
    r.qr = 1;
    r.rd = 1;
    uint16_t flags = get_flags(1, 1);
    printf("flag = %x\n", flags);
    r.qdcount = 1;
    r.ancount = 0;
    r.nscount = 0;
    r.arcount = 0;
    puts("init request");
    // Write id.
    memcpy(buf, &r.id, 2);

    // Write flags
    // memcpy(buf + 2, &r.flags, 2);
    *(uint8_t *)(buf + 2) = (flags & 0xFF00) >> 8;
    *(uint8_t *)(buf + 3) = flags & 0xFF;
    // memcpy(buf + 4, &(r.qdcount & 0xFF00), 1);
    *(uint8_t *)(buf + 4) = (r.qdcount & 0xFF00) >> 8;
    *(uint8_t *)(buf + 5) = r.qdcount & 0xFF;
    memcpy(buf + 6, &r.ancount, 2);
    memcpy(buf + 8, &r.nscount, 2);
    memcpy(buf + 10, &r.arcount, 2);
    puts("buffered header");
    // Write question.
    int length = strlen(question->qname);

    // Write qname.
    int prev = 0;
    int size = 0;
    int cur_pos = 0;
    for (int pos = 0; pos < length; pos++)
    {
        if (question->qname[pos] == '.')
        {
            size = pos - prev;
            *(uint8_t *)(buf + 12 + cur_pos) = size;
            memcpy(buf + 12 + cur_pos + 1, &(question->qname[prev]), size);
            prev = pos + 1;
            cur_pos += size + 1;
        }
    }
    *(uint8_t *)(buf + 12 + cur_pos) = 0;

    *(uint8_t *)(buf + 13 + cur_pos) = (question->qtype & 0xFF00) >> 8;
    *(uint8_t *)(buf + 14 + cur_pos) = question->qtype & 0xFF;

    *(uint8_t *)(buf + 15 + cur_pos) = (question->qclass & 0xFF00) >> 8;
    *(uint8_t *)(buf + 16 + cur_pos) = question->qclass & 0xFF;
    puts("created request");
    return 17 + cur_pos;
}

void resolv(const struct request r)
{
    int sockfd;
    char buffer[BUF];
    struct sockaddr_in servaddr;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return;
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

finish:
    free(dns_type);
    return result;
}