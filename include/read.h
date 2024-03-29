#ifndef _READ_H_
#define _READ_H_

#include "errors.h"
#include "message.h"

#include <stdint.h>

uint8_t read_uint8_t(const uint8_t* buf);
uint16_t read_uint16_t(const uint8_t* buf);
uint32_t read_uint32_t(const uint8_t* buf);
void read_ipv4(const uint8_t* buf, char* ip);
void read_ipv6(const uint8_t* buf, char* ip);
void read_header(const uint8_t* buffer, struct header* hdr);
int read_question(const uint8_t* buffer, const int pos, struct question* q);
int read_qname(const uint8_t* buffer, const int pos, char* qname);
dns_error read_resource_record(const uint8_t* buffer, const int pos, struct resource_record* rr, int* current_pos);
dns_error read_hinfo(const uint8_t* buf, const int pos, char* rdata);
dns_error read_mx(const uint8_t* buf, const int pos, char* rdata);
dns_error read_soa(const uint8_t* buf, const int pos, char* rdata);

#endif
