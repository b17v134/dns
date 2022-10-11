#ifndef _DNS_RESOLV_H_
#define _DNS_RESOLV_H_

#include <stdint.h>
#include <stdbool.h>

// https://www.rfc-editor.org/rfc/rfc1035 4.1.1
struct request {
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
struct question {
    char *qname;
    uint16_t qtype;
    uint16_t qclass;
};

// https://www.rfc-editor.org/rfc/rfc1035 4.1.3
struct resource_record {
    char *name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    void *rdata;
};

uint8_t create_request(struct question *question, void *buf, uint16_t buf_size);
void resolv(char * addr, uint16_t port, char *qname);

#endif