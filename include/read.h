#ifndef _READ_H_
#define _READ_H_

#include "message.h"

#include <stdint.h>

uint8_t read_uint8_t(void *buf);
uint16_t read_uint16_t(void *buf);
uint32_t read_uint32_t(void *buf);
void read_ipv4(void *buf, char *ip);
void read_ipv6(void *buf, char *ip);
void read_header(void *buffer, struct header *hdr);
int read_question(void *buffer, const int pos, struct question *q);
int read_resource_record(void *buffer, const int pos, struct resource_record *rr);

#endif