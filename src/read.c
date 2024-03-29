#include "read.h"
#include "types.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t read_uint8_t(const uint8_t* buf)
{
    return *buf;
}

uint16_t read_uint16_t(const uint8_t* buf)
{
    return ntohs(*(uint16_t*)buf);
}

uint32_t read_uint32_t(const uint8_t* buf)
{
    return ntohl(*(uint32_t*)buf);
}

void read_ipv4(const uint8_t* buf, char* ip)
{
    sprintf(
        ip,
        "%d.%d.%d.%d",
        read_uint8_t(buf),
        read_uint8_t(buf + 1),
        read_uint8_t(buf + 2),
        read_uint8_t(buf + 3));
}

void read_ipv6(const uint8_t* buf, char* ip)
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

void read_header(const uint8_t* buffer, struct header* hdr)
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

int read_question(const uint8_t* buffer, const int pos, struct question* q)
{
    char* qname = NULL;
    qname = malloc(sizeof(char) * BUFSIZ);
    if (qname == NULL) {
        perror("Cannot allocate memory");
        return -1;
    }
    bzero(qname, BUFSIZ);
    int current_pos = read_qname(buffer, pos, qname);

    char* tmp = (char*)realloc(qname, sizeof(char) * (strlen(qname) + 1));
    if (tmp == NULL) {
        perror("Cannot allocate memory");
        free(qname);
        return -1;
    }
    q->qname = tmp;
    q->qtype = read_uint16_t(buffer + current_pos);
    q->qclass = read_uint16_t(buffer + current_pos + 2);
    current_pos += 4;

    return current_pos;
}

int read_qname(const uint8_t* buffer, const int pos, char* qname)
{
    int max_pos = pos;
    uint8_t length;
    uint16_t tmp_pos = pos;
    if (buffer[pos] == 0) {
        strcpy(qname, ".");
        return pos;
    }
    while ((length = *(uint8_t*)(buffer + tmp_pos))) {
        if (((length & (0x3 << 6)) >> 6) == 0x3) {
            if (max_pos < tmp_pos) {
                max_pos = tmp_pos + 1;
            }
            tmp_pos = read_uint16_t(buffer + tmp_pos) - (0x3 << 14);
            length = *(uint8_t*)(buffer + tmp_pos);
            int old_len = strlen(qname);
            memcpy(qname + old_len, (char*)(buffer + tmp_pos + 1), length);
            qname[old_len + length] = '\0';
            strcat(qname, ".");
            tmp_pos += length + 1;
        } else {
            if (max_pos < tmp_pos) {
                max_pos = tmp_pos;
            }

            int old_len = strlen(qname);
            memcpy(qname + old_len, (char*)(buffer + tmp_pos + 1), length);
            qname[old_len + length] = '\0';
            strcat(qname, ".");
            tmp_pos += length + 1;
        }
    }

    if (max_pos < tmp_pos) {
        max_pos = tmp_pos;
    }
    return max_pos + 1;
}

dns_error read_resource_record(const uint8_t* buffer, const int pos, struct resource_record* rr, int* current_pos)
{

    char* qname = malloc(sizeof(char) * BUFSIZ);
    if (qname == NULL) {
        return DNS_ERROR_OUT_OF_MEMORY;
    }
    bzero(qname, BUFSIZ);
    int tmp_current_pos = read_qname(buffer, pos, qname);
    char* new_qname = (char*)realloc(qname, sizeof(char) * (strlen(qname) + 1));
    if (new_qname == NULL) {
        free(qname);
        return DNS_ERROR_OUT_OF_MEMORY;
    }
    rr->name = new_qname;
    rr->type = read_uint16_t(buffer + tmp_current_pos + 1);
    rr->class = read_uint16_t(buffer + tmp_current_pos + 3);
    rr->ttl = read_uint32_t(buffer + tmp_current_pos + 5);
    rr->rdlength = read_uint16_t(buffer + tmp_current_pos + 9);
    int tmp = rr->rdlength;

    rr->rdata = malloc(sizeof(char) * BUFSIZ);
    if (rr->rdata == NULL) {
        return DNS_ERROR_OUT_OF_MEMORY;
    }

    bzero(rr->rdata, BUFSIZ);
    dns_error error = DNS_ERROR_OK;
    switch (rr->type) {
    case DNS_TYPE_A:
        read_ipv4(buffer + tmp_current_pos + 11, rr->rdata);
        break;
    case DNS_TYPE_AAAA:
        read_ipv6(buffer + tmp_current_pos + 11, rr->rdata);
        break;
    case DNS_TYPE_HINFO:
        error = read_hinfo(buffer, tmp_current_pos + 11, rr->rdata);
        break;
    case DNS_TYPE_MX:
        error = read_mx(buffer, tmp_current_pos + 11, rr->rdata);
        break;
    case DNS_TYPE_SOA:
        error = read_soa(buffer, tmp_current_pos + 11, rr->rdata);
        break;
    default:
        error = read_qname(buffer, tmp_current_pos + 11, rr->rdata);
        break;
    }
    if (error != DNS_ERROR_OK) {
        return error;
    }
    char* new_rdata = (char*)realloc(rr->rdata, sizeof(char) * (strlen(rr->rdata) + 1));
    if (new_rdata == NULL) {
        return DNS_ERROR_OUT_OF_MEMORY;
    }
    rr->rdata = new_rdata;
    tmp_current_pos += 11 + tmp;

    *current_pos = tmp_current_pos;

    return DNS_ERROR_OK;
}

int read_string(const uint8_t* buf, const int pos, char* result)
{
    uint8_t size = read_uint8_t(buf + pos);
    strncpy(result, (char*)(buf + pos + 1), size);
    return pos + size + 1;
}

dns_error read_hinfo(const uint8_t* buf, const int pos, char* rdata)
{
    char* cpu;
    cpu = malloc(sizeof(char) * BUFSIZ);
    if (cpu == NULL) {
        return DNS_ERROR_OUT_OF_MEMORY;
    }
    bzero(cpu, BUFSIZ);
    int cur_pos = read_string(buf, pos, cpu);
    char* os;
    os = malloc(sizeof(char) * BUFSIZ);
    if (os == NULL) {
        free(cpu);
        return DNS_ERROR_OUT_OF_MEMORY;
    }
    bzero(os, BUFSIZ);
    read_string(buf, cur_pos, os);
    sprintf(rdata, "\"%s\" \"%s\"", cpu, os);
    free(os);
    free(cpu);

    return DNS_ERROR_OK;
}

dns_error read_mx(const uint8_t* buf, const int pos, char* rdata)
{
    uint16_t preference = read_uint16_t(buf + pos);
    sprintf(rdata, "%u", preference);
    strcat(rdata, " ");

    char* rname = (char*)malloc(sizeof(char) * BUFSIZ);
    if (rname == NULL) {
        return DNS_ERROR_OUT_OF_MEMORY;
    }
    bzero(rname, BUFSIZ);

    read_qname(buf, pos + 2, rname);
    strcat(rdata, rname);
    free(rname);

    return DNS_ERROR_OK;
}

dns_error read_soa(const uint8_t* buf, const int pos, char* rdata)
{
    int cur_pos = read_qname(buf, pos, rdata);
    strcat(rdata, " ");

    char* rname;
    rname = malloc(sizeof(char) * BUFSIZ);
    if (rname == NULL) {
        return DNS_ERROR_OUT_OF_MEMORY;
    }

    cur_pos = read_qname(buf, cur_pos, rname);
    strcat(rdata, rname);
    strcat(rdata, " ");
    free(rname);
    for (int i = 0; i < 5; i++) {
        uint32_t item = read_uint32_t(buf + cur_pos);
        char* str_item;
        str_item = malloc(sizeof(char) * BUFSIZ);
        if (str_item == NULL) {
            return DNS_ERROR_OUT_OF_MEMORY;
        }
        sprintf(str_item, "%u", item);
        strcat(rdata, str_item);
        if (i != 4) {
            strcat(rdata, " ");
        }
        free(str_item);
        cur_pos += 4;
    }

    return DNS_ERROR_OK;
}
