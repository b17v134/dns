#include "dns.h"

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
    int s = create_request(&q, buf, 1024);
    size_t result = sendto(sockfd, buf, s, MSG_CONFIRM, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    free(buf);

    n = recvfrom(sockfd, (char *)buffer, BUF, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
    buffer[n] = '\0';
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
    struct response result = {0};

    result.hdr.id = read_uint16_t(buffer);
    uint16_t flags = read_uint16_t(buffer + 2);
    result.hdr.qdcount = read_uint16_t(buffer + 4);
    result.hdr.ancount = read_uint16_t(buffer + 6);
    result.hdr.nscount = read_uint16_t(buffer + 8);
    result.hdr.arcount = read_uint16_t(buffer + 10);

    int pos = 12;
    result.questions = malloc(sizeof(struct question) * result.hdr.qdcount);
    for (int i = 0; i < result.hdr.qdcount; i++)
    {
        char *qname = NULL;
        uint8_t length;
        while (length = *(uint8_t *)(buffer + pos))
        {
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
            strcat(qname, ".");
            pos += length + 1;
        }
        result.questions[i].qname = qname;
        result.questions[i].qtype = read_uint16_t(buffer + pos + 1);
        result.questions[i].qclass = read_uint16_t(buffer + pos + 3);
        pos += 5;
    }
    result.answers = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.ancount);
    for (int i = 0; i < result.hdr.ancount; i++)
    {
        char *qname = NULL;
        uint8_t length;
        uint8_t tmp_pos = pos;
        while (length = *(uint8_t *)(buffer + tmp_pos))
        {
            if (((length & 0b11000000) >> 6) == 0b11)
            {
                tmp_pos = read_uint16_t(buffer + tmp_pos) - 0b1100000000000000;

                length = *(uint8_t *)(buffer + tmp_pos);
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

            strcat(qname, ".");
            tmp_pos += length + 1;
        }
        result.answers[i].name = qname;
        pos += 1;
        result.answers[i].type = read_uint16_t(buffer + pos + 1);
        result.answers[i].class = read_uint16_t(buffer + pos + 3);
        result.answers[i].ttl = read_uint32_t(buffer + pos + 5);
        result.answers[i].rdlength = read_uint16_t(buffer + pos + 9);
        int tmp = result.answers[i].rdlength;
        switch (result.answers[i].type) { 
            case DNS_TYPE_A:
                result.answers[i].rdata = malloc(sizeof(char) * 16);
                read_ipv4(buffer + pos + 11, result.answers[i].rdata);
                break;
            case DNS_TYPE_AAAA:
                result.answers[i].rdata = malloc(sizeof(char) * 50); // @todo: fix size
                read_ipv6(buffer + pos + 11, result.answers[i].rdata);
                break;
            default:
                result.answers[i].rdata = malloc(sizeof(char) * (result.answers[i].rdlength + 1));
                memcpy(result.answers[i].rdata, buffer+ pos + 11, result.answers[i].rdlength);
                result.answers[i].rdata[result.answers[i].rdlength] = '\0';
                break;
        }
        pos += 11 + tmp;
    }
    return result;
}

void print_response(struct response resp)
{
    puts("questions:");
    for (int i = 0; i < resp.hdr.qdcount; i++)
    {
        printf(
            "%s\t%d\t%d\n", 
            resp.questions[i].qname, 
            resp.questions[i].qclass, 
            resp.questions[i].qtype
            );
    }
    puts("\nanswers:");
    for (int i = 0; i < resp.hdr.ancount; i++)
    {
        printf(
            "%s\t%d\t%d\t%s\t%s\n", 
            resp.answers[i].name, 
            resp.answers[i].ttl, 
            resp.answers[i].class, 
            int_to_dns_type(resp.answers[i].type), 
            resp.answers[i].rdata
            );
    }
}

