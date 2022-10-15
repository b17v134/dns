#ifndef _DNS_RESOLV_H_
#define _DNS_RESOLV_H_

#include <stdint.h>
#include <stdbool.h>

#define DNS_TYPE_ERROR 0
#define DNS_TYPE_A 1
#define DNS_TYPE_NS 2
#define DNS_TYPE_MD 3

// https://www.rfc-editor.org/rfc/rfc1035 4.1.1
struct header
{
    uint16_t id;
    // A one bit field that specifies whether this message is a query (0), or a response (1).
    uint8_t qr;
    // Recursion Desired
    uint8_t rd;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
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
};

struct request
{
    char *addr;
    uint16_t port;
    enum protocol pr;
    char *qname;
    uint16_t type;
};

struct response
{
    struct header hdr;
    struct question *questions;
    struct resource_record *answers;
};

void strupr(const char *str, char *result);
uint16_t dns_type_to_int(const char *type);

uint8_t create_request(struct question *question, void *buf, uint16_t buf_size);
int resolv(const struct request r, char *buffer);
struct response get_response(void *buffer, int len);

#endif