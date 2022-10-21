#include "message.h"
#include "write.h"

#include <string.h>
#include <stdio.h>

uint16_t get_flags(uint8_t qr, uint8_t rd)
{
    return ((qr & 0b1) << 16) + ((rd & 0b1) << 8);
}

void write_uint16_t(void *buf, uint16_t value)
{
    *(uint8_t *)(buf) = (value & 0xFF00) >> 8;
    *(uint8_t *)(buf + 1) = value & 0xFF;
}

int write_qname(void *buf, const char *qname)
{
    int length = strlen(qname);
    if (qname[length - 1] == '.')
    {
        length--;
    }
    uint8_t chunk_length = 0;
    for (int pos = length - 1; pos >= 0; pos--)
    {
        if (qname[pos] == '.')
        {
            *(uint8_t *)(buf + pos + 1) = chunk_length;
            chunk_length = 0;
        }
        else
        {
            chunk_length++;
            *(char *)(buf + pos + 1) = qname[pos];
        }
    }
    *(uint8_t *)(buf) = chunk_length;
    return length + 1;
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