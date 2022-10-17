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

void read_header(void *buffer, struct header *hdr)
{
    hdr->id = read_uint16_t(buffer);
    uint16_t flags = read_uint16_t(buffer + 2);
    hdr->qdcount = read_uint16_t(buffer + 4);
    hdr->ancount = read_uint16_t(buffer + 6);
    hdr->nscount = read_uint16_t(buffer + 8);
    hdr->arcount = read_uint16_t(buffer + 10);
}

int read_question(void *buffer, const int pos, struct question *q)
{
    char *qname = NULL;
    int current_pos = pos;
    uint8_t length;

    while (length = *(uint8_t *)(buffer + current_pos))
    {
        if (qname == NULL)
        {
            qname = (char *)malloc(sizeof(char) * (length + 1));
            memcpy(qname, buffer + current_pos + 1, length);
        }
        else
        {
            qname = (char *)realloc(qname, sizeof(char) * (length + 1));
            strncat(qname, buffer + current_pos + 1, length);
        }
        strcat(qname, ".");
        current_pos += length + 1;
    }
    q->qname = qname;
    q->qtype = read_uint16_t(buffer + current_pos + 1);
    q->qclass = read_uint16_t(buffer + current_pos + 3);
    current_pos += 5;

    return current_pos;
}

struct response get_response(void *buffer, int len)
{
    struct response result = {0};

    read_header(buffer, &(result.hdr));

    int pos = 12;
    result.questions = malloc(sizeof(struct question) * result.hdr.qdcount);
    for (int i = 0; i < result.hdr.qdcount; i++)
    {
        pos = read_question(buffer, pos, &result.questions[i]);
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
        switch (result.answers[i].type)
        {
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
            memcpy(result.answers[i].rdata, buffer + pos + 11, result.answers[i].rdlength);
            result.answers[i].rdata[result.answers[i].rdlength] = '\0';
            break;
        }
        pos += 11 + tmp;
    }

    result.authority_records = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.nscount);
    for (int i = 0; i < result.hdr.nscount; i++)
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
        result.authority_records[i].name = qname;
        pos += 1;
        result.authority_records[i].type = read_uint16_t(buffer + pos + 1);
        result.authority_records[i].class = read_uint16_t(buffer + pos + 3);
        result.authority_records[i].ttl = read_uint32_t(buffer + pos + 5);
        result.authority_records[i].rdlength = read_uint16_t(buffer + pos + 9);
        int tmp = result.authority_records[i].rdlength;

        result.authority_records[i].rdata = malloc(sizeof(char) * (result.authority_records[i].rdlength + 1));
        memcpy(result.authority_records[i].rdata, buffer + pos + 11, result.authority_records[i].rdlength);
        result.authority_records[i].rdata[result.authority_records[i].rdlength] = '\0';
        pos += 11 + tmp;
    }
    return result;
}
