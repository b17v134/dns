#include "dns.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

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

uint8_t create_request(struct question *question, void *buf, uint16_t buf_size)
{
    struct header r = create_header();
    int header_length = write_header(buf, r);
    int length = write_question(buf + header_length, question);
    return header_length + length;
}

int resolv(const struct request r, struct response *rsp)
{
    if (r.pr == https)
    {
        return resolv_https(r, rsp);
    }

    if (r.pr == udp)
    {
        return resolv_udp(r, rsp);
    }

    return resolv_tls(r, rsp);
}

struct response get_response(void *buffer, int len)
{
    struct response result;

    memset(&result, 0, sizeof(struct response));

    read_header(buffer, &(result.hdr));

    int pos = 12;
    if (result.hdr.qdcount > 0)
    {
        result.questions = malloc(sizeof(struct question) * result.hdr.qdcount);
        for (int i = 0; i < result.hdr.qdcount; i++)
        {
            pos = read_question(buffer, pos, &result.questions[i]);
        }
    }

    if (result.hdr.ancount > 0)
    {
        result.answers = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.ancount);
        for (int i = 0; i < result.hdr.ancount; i++)
        {
            pos = read_resource_record(buffer, pos, &(result.answers[i]));
        }
    }

    if (result.hdr.nscount > 0)
    {
        result.authority_records = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.nscount);
        for (int i = 0; i < result.hdr.nscount; i++)
        {
            pos = read_resource_record(buffer, pos, &(result.authority_records[i]));
        }
    }

    if (result.hdr.arcount > 0)
    {
        result.additional_records = (struct resource_record *)malloc(sizeof(struct resource_record) * result.hdr.arcount);
        for (int i = 0; i < result.hdr.arcount; i++)
        {
            pos = read_resource_record(buffer, pos, &(result.additional_records[i]));
        }
    }

    return result;
}

void free_response(struct response resp)
{
    for (int i = 0; i < resp.hdr.qdcount; i++)
    {
        free(resp.questions[i].qname);
    }
    if (resp.hdr.qdcount > 0)
    {
        free(resp.questions);
    }

    for (int i = 0; i < resp.hdr.ancount; i++)
    {
        free(resp.answers[i].name);
        free(resp.answers[i].rdata);
    }
    if (resp.hdr.ancount > 0)
    {
        free(resp.answers);
    }

    for (int i = 0; i < resp.hdr.nscount; i++)
    {
        free(resp.authority_records[i].name);
        free(resp.authority_records[i].rdata);
    }
    if (resp.hdr.nscount > 0)
    {
        free(resp.authority_records);
    }

    for (int i = 0; i < resp.hdr.arcount; i++)
    {
        free(resp.additional_records[i].name);
        free(resp.additional_records[i].rdata);
    }

    if (resp.hdr.arcount > 0)
    {
        free(resp.additional_records);
    }
}