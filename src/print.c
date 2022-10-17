#include "classes.h"
#include "print.h"
#include "types.h"

#include <stdio.h>

void print_header(const struct header hdr)
{
    printf(
        "question: %d\nanswer: %d\nauthority: %d\nadditional: %d\n",
        hdr.qdcount,
        hdr.ancount,
        hdr.nscount,
        hdr.arcount);
}

void print_resource_record(const struct resource_record record)
{
    printf(
        "%s\t%d\t%s\t%s\t%s\n",
        record.name,
        record.ttl,
        int_to_dns_class(record.class),
        int_to_dns_type(record.type),
        record.rdata);
}

void print_response(const struct response resp)
{
    puts("header:");
    print_header(resp.hdr);

    puts("\nquestions:");
    for (int i = 0; i < resp.hdr.qdcount; i++)
    {
        printf(
            "%s\t\t%s\t%s\n",
            resp.questions[i].qname,
            int_to_dns_class(resp.questions[i].qclass),
            int_to_dns_type(resp.questions[i].qtype));
    }
    puts("\nanswers:");
    for (int i = 0; i < resp.hdr.ancount; i++)
    {
        print_resource_record(resp.answers[i]);
    }
    puts("\nauthority records:");
    for (int i = 0; i < resp.hdr.nscount; i++)
    {
        print_resource_record(resp.authority_records[i]);
    }
}
