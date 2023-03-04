#include "read.h"
#include "types.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t read_uint8_t(uint8_t* buf)
{
    return (*(uint8_t*)buf);
}

uint16_t read_uint16_t(uint8_t* buf)
{
    return ntohs(*(uint16_t*)buf);
}

uint32_t read_uint32_t(uint8_t* buf)
{
    return ntohl(*(uint32_t*)buf);
}

void read_ipv4(uint8_t* buf, char* ip)
{
    sprintf(
        ip,
        "%d.%d.%d.%d",
        read_uint8_t(buf),
        read_uint8_t(buf + 1),
        read_uint8_t(buf + 2),
        read_uint8_t(buf + 3));
}

void read_ipv6(uint8_t* buf, char* ip)
{
    char num[3];
    ip[0] = '\0';

    for (int i = 0; i < 16; i++) {
        sprintf(num, "%02x", read_uint8_t(buf + i));
        strcat(ip, num);
        if ((i % 2 == 1) && (i < 15)) {
            strcat(ip, ":");
        }
    }
}

void read_header(uint8_t* buffer, struct header* hdr)
{
    hdr->id = read_uint16_t(buffer);

    uint16_t flags = read_uint16_t(buffer + 2);
    hdr->qr = (flags & (1 << 15)) >> 15;
    hdr->opcode = (flags & (0xF << 11)) >> 11;
    hdr->aa = (flags & (1 << 10)) >> 10;
    hdr->tc = (flags & (1 << 9)) >> 9;
    hdr->rd = (flags & (1 << 8)) >> 8;
    hdr->ra = (flags & (1 << 7)) >> 7;
    hdr->z = (flags & (0x7 << 4)) >> 4;
    hdr->rcode = flags & 0xF;

    hdr->qdcount = read_uint16_t(buffer + 4);
    hdr->ancount = read_uint16_t(buffer + 6);
    hdr->nscount = read_uint16_t(buffer + 8);
    hdr->arcount = read_uint16_t(buffer + 10);
}

int read_question(uint8_t* buffer, const int pos, struct question* q)
{
    char* qname = NULL;
    qname = malloc(sizeof(char) * BUFSIZ);
    if (qname == NULL) {
        perror("Cannot allocate memory");
        return -1;
    }
    int current_pos = pos;
    uint8_t length;

    while ((length = *(uint8_t*)(buffer + current_pos))) {
        strncat(qname, (char*)(buffer + current_pos + 1), length);
        strcat(qname, ".");
        current_pos += length + 1;
    }

    char* tmp = (char*)realloc(qname, sizeof(char) * (strlen(qname) + 1));
    if (tmp == NULL) {
        perror("Cannot allocate memory");
        free(qname);
        return -1;
    }
    q->qname = tmp;
    q->qtype = read_uint16_t(buffer + current_pos + 1);
    q->qclass = read_uint16_t(buffer + current_pos + 3);
    current_pos += 5;

    return current_pos;
}

int read_qname(uint8_t* buffer, const int pos, char* qname)
{
    int current_pos = pos;
    uint8_t length;
    uint16_t tmp_pos = current_pos;
    int is_pointer = 0; // no pointer
    while ((length = *(uint8_t*)(buffer + tmp_pos))) {
        if (((length & (0x3 << 6)) >> 6) == 0x3) {
            tmp_pos = read_uint16_t(buffer + tmp_pos) - (0x3 << 14);
            length = *(uint8_t*)(buffer + tmp_pos);
            is_pointer = 1;
        } else {
            if (is_pointer == 0) {
                current_pos += length + 1;
            }
        }
        strncat(qname, (char*)(buffer + tmp_pos + 1), length);
        strcat(qname, ".");
        tmp_pos += length + 1;
    }
    current_pos += 1;
    return current_pos;
}

int read_resource_record(uint8_t* buffer, const int pos, struct resource_record* rr)
{

    char* qname = malloc(sizeof(char) * BUFSIZ);
    if (qname == NULL) {
        perror("Cannot allocate memory");
        return -1;
    }
    memset(qname, 0, BUFSIZ);
    int current_pos = read_qname(buffer, pos, qname);
    char* new_qname = (char*)realloc(qname, sizeof(char) * (strlen(qname) + 1));
    if (new_qname == NULL) {
        perror("Cannot allocate memory");
        free(qname);
        return -1;
    }
    rr->name = new_qname;
    rr->type = read_uint16_t(buffer + current_pos + 1);
    rr->class = read_uint16_t(buffer + current_pos + 3);
    rr->ttl = read_uint32_t(buffer + current_pos + 5);
    rr->rdlength = read_uint16_t(buffer + current_pos + 9);
    int tmp = rr->rdlength;
    switch (rr->type) {
    case DNS_TYPE_A:
        rr->rdata = malloc(sizeof(char) * 16);
        read_ipv4(buffer + current_pos + 11, rr->rdata);
        break;
    case DNS_TYPE_AAAA:
        rr->rdata = malloc(sizeof(char) * 50); // @todo: fix size
        read_ipv6(buffer + current_pos + 11, rr->rdata);
        break;
    case DNS_TYPE_SOA:
        rr->rdata = malloc(sizeof(char) * BUFSIZ);
        memset(rr->rdata, 0, BUFSIZ);
        read_soa(buffer, current_pos + 11, rr->rdata);
        rr->rdata = (char*)realloc(rr->rdata, sizeof(char) * (strlen(rr->rdata) + 1));
        break;
    default:
        rr->rdata = malloc(sizeof(char) * BUFSIZ);
        memset(rr->rdata, 0, BUFSIZ);
        read_qname(buffer, current_pos + 11, rr->rdata);
        rr->rdata = (char*)realloc(rr->rdata, sizeof(char) * (strlen(rr->rdata) + 1));
        break;
    }
    current_pos += 11 + tmp;

    return current_pos;
}

void read_soa(uint8_t* buf, const int pos, char* rdata)
{
    int cur_pos = read_qname(buf, pos, rdata);
    strcat(rdata, " ");
    char* rname;
    rname = malloc(sizeof(char) * BUFSIZ);
    cur_pos = read_qname(buf, cur_pos + 1, rname);
    strcat(rdata, rname);
    strcat(rdata, " ");
    free(rname);
    for (int i = 0; i < 5; i++) {
        uint32_t item = read_uint32_t(buf + cur_pos + 1);
        char* str_item;
        str_item = malloc(sizeof(char) * BUFSIZ);
        sprintf(str_item, "%u", item);
        strcat(rdata, str_item);
        if (i != 4) {
            strcat(rdata, " ");
        }
        free(str_item);
        cur_pos += 4;
    }
}
