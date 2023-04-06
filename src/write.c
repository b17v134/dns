#include "write.h"
#include "message.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

uint16_t get_flags(uint8_t qr, uint8_t rd)
{
    return ((qr & 1) << 16) + ((rd & 1) << 8);
}

void write_uint16_t(uint8_t* buf, uint16_t value)
{
    *(uint8_t*)(buf) = (value & 0xFF00) >> 8;
    *(uint8_t*)(buf + 1) = value & 0xFF;
}

int write_qname(uint8_t* buf, const char* qname)
{
    int length = strlen(qname);
    if (qname[length - 1] == '.') {
        length--;
    }
    bool write_count = true;
    int pos = 0;
    for (int i = 0; i < length; i++) {
        if (write_count) {
            uint8_t count = 0;
            for (int j = i; (j < length) && (qname[j] != '.'); j++) {
                count++;
            }
            *(uint8_t*)(buf + pos) = count;
            pos++;
            write_count = false;
        }
        if (qname[i] == '.') {
            write_count = true;
            continue;
        }
        *(uint8_t*)(buf + pos) = qname[i];
        pos++;
    }
    return pos;
}

int write_header(uint8_t* buf, struct header h)
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

int write_question(uint8_t* buf, struct question* q)
{
    int length = write_qname(buf, q->qname);
    write_uint16_t(buf + length + 1, q->qtype);
    write_uint16_t(buf + length + 3, q->qclass);

    return length + 5;
}
