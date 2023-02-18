#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdio.h>
#include <stdint.h>

// https://www.rfc-editor.org/rfc/rfc1035 4.1.1
struct header
{
    // Identifier.
    uint16_t id;

    // A one bit field that specifies whether this message is a query (0), or a response (1).
    uint8_t qr;

    // A four bit field that specifies kind of query.
    uint8_t opcode;

    // Authoritative Answer.
    uint8_t aa;

    // Specifies that this message was truncated.
    uint8_t tc;

    // Recursion Desired.
    uint8_t rd;

    // Recursion Available.
    uint8_t ra;

    // Reserved for future use. Must be zero in all queries and responses.
    uint8_t z;

    // Response code.
    uint8_t rcode;

    // Number of entries in the question section.
    uint16_t qdcount;

    // Number of resource records in the answer section.
    uint16_t ancount;

    // Number of name server resource records in the authority records section.
    uint16_t nscount;

    // Number of resource records in the additional records section.
    uint16_t arcount;
};

// https://www.rfc-editor.org/rfc/rfc1035 4.1.2
struct question
{
    char *qname;
    uint16_t qtype;
    uint16_t qclass;
};

// https://www.rfc-editor.org/rfc/rfc1035 4.1.3
struct resource_record
{
    char *name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    char *rdata;
};

enum protocol
{
    tcp,
    udp,
    tls,
    https,
};

struct request
{
    char *addr;
    uint16_t port;
    enum protocol pr;
    char *qname;
    uint16_t type;
    char *ca;
    char *certificate;
};

struct response
{
    struct header hdr;
    struct question *questions;
    struct resource_record *answers;
    struct resource_record *authority_records;
    struct resource_record *additional_records;
};

uint16_t get_flags(uint8_t qr, uint8_t rd);
uint8_t create_request(struct question *question, void *buf, uint16_t buf_size);
int resolv(const struct request r, struct response *rsp);
int resolv_https(const struct request r, void *buffer, size_t *len);
int resolv_tls(const struct request r, void *buffer, size_t *len);
int resolv_udp(const struct request r, void *buffer, size_t *len);
struct response get_response(void *buffer, int len);
void free_response(struct response rsp);

#endif