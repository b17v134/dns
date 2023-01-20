#include "classes.h"
#include "print.h"
#include "types.h"

#include <stdio.h>

#include <json-c/json.h>

void print_header(const struct header hdr)
{
    printf(
        "id: %d\nresponse: %d\nopcode: %d\nauthoritative: %d\ntruncated: %d\nrecursion desired: %d\nrecursion available: %d\nreserved: %d\nrcode: %d\nquestion: %d\nanswer: %d\nauthority: %d\nadditional: %d\n",
        hdr.id,
        hdr.qr,
        hdr.opcode,
        hdr.aa,
        hdr.tc,
        hdr.rd,
        hdr.ra,
        hdr.z,
        hdr.rcode,
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
    puts("\nadditional records:");
    for (int i = 0; i < resp.hdr.arcount; i++)
    {
        print_resource_record(resp.additional_records[i]);
    }
}

json_object *json_resource_record(const struct resource_record record)
{
    json_object *item = json_object_new_object();
    json_object *name = json_object_new_string(record.name);
    json_object_object_add(item, "name", name);
    json_object *ttl = json_object_new_int(record.ttl);
    json_object_object_add(item, "ttl", ttl);
    json_object *qclass = json_object_new_string(int_to_dns_class(record.class));
    json_object_object_add(item, "class", qclass);
    json_object *qtype = json_object_new_string(int_to_dns_type(record.type));
    json_object_object_add(item, "type", qtype);
    json_object *rdata = json_object_new_string(record.rdata);
    json_object_object_add(item, "rdata", rdata);

    return item;
}

void print_json_response(const struct response resp)
{
    json_object *items;

    json_object *root = json_object_new_object();
    if (!root)
    {
        exit(1);
    }

    items = json_object_new_object();
    json_object_object_add(root, "header", items);

    json_object_object_add(items, "id", json_object_new_int(resp.hdr.id));
    json_object_object_add(items, "response", json_object_new_int(resp.hdr.qr));
    json_object_object_add(items, "opcode", json_object_new_int(resp.hdr.opcode));
    json_object_object_add(items, "authoritative", json_object_new_int(resp.hdr.aa));
    json_object_object_add(items, "truncated", json_object_new_int(resp.hdr.tc));
    json_object_object_add(items, "recursion desired", json_object_new_int(resp.hdr.rd));
    json_object_object_add(items, "recursion available", json_object_new_int(resp.hdr.ra));
    json_object_object_add(items, "reserved", json_object_new_int(resp.hdr.z));
    json_object_object_add(items, "rcode", json_object_new_int(resp.hdr.rcode));
    json_object_object_add(items, "question", json_object_new_int(resp.hdr.qdcount));
    json_object_object_add(items, "answer", json_object_new_int(resp.hdr.ancount));
    json_object_object_add(items, "authority", json_object_new_int(resp.hdr.nscount));
    json_object_object_add(items, "additional", json_object_new_int(resp.hdr.arcount));

    json_object *questions = json_object_new_array();
    json_object_object_add(root, "questions", questions);
    for (int i = 0; i < resp.hdr.qdcount; i++)
    {
        json_object *item = json_object_new_object();
        json_object *qname = json_object_new_string(resp.questions[i].qname);
        json_object_object_add(item, "qname", qname);
        json_object *qclass = json_object_new_string(int_to_dns_class(resp.questions[i].qclass));
        json_object_object_add(item, "qclass", qclass);
        json_object *qtype = json_object_new_string(int_to_dns_type(resp.questions[i].qtype));
        json_object_object_add(item, "qtype", qtype);
        json_object_array_add(questions, item);
    }

    json_object *answers = json_object_new_array();
    json_object_object_add(root, "answers", answers);
    for (int i = 0; i < resp.hdr.ancount; i++)
    {
        json_object_array_add(answers, json_resource_record(resp.answers[i]));
    }

    json_object *authority_records = json_object_new_array();
    json_object_object_add(root, "authority records", authority_records);
    for (int i = 0; i < resp.hdr.nscount; i++)
    {
        json_object_array_add(authority_records, json_resource_record(resp.authority_records[i]));
    }

    json_object *additional_records = json_object_new_array();
    json_object_object_add(root, "additional records", additional_records);
    for (int i = 0; i < resp.hdr.arcount; i++)
    {
        json_object_array_add(additional_records, json_resource_record(resp.additional_records[i]));
    }

    printf("%s\n", json_object_to_json_string_ext(root, JSON_C_TO_STRING_PRETTY));
    json_object_put(root);
}