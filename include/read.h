#ifndef _READ_H_
#define _READ_H_

#include "message.h"

#include <stdint.h>

uint8_t read_uint8_t(uint8_t* buf);
uint16_t read_uint16_t(uint8_t* buf);
uint32_t read_uint32_t(uint8_t* buf);
void read_ipv4(uint8_t* buf, char* ip);
void read_ipv6(uint8_t* buf, char* ip);
void read_header(uint8_t* buffer, struct header* hdr);
int read_question(uint8_t* buffer, const int pos, struct question* q);
int read_qname(uint8_t* buffer, const int pos, char* qname);
int read_resource_record(uint8_t* buffer, const int pos, struct resource_record* rr);
void read_soa(uint8_t* buf, const int pos, char* rdata);

#endif
