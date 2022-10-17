#include "read.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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