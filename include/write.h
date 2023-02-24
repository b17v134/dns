#ifndef _WRITE_H_
#define _WRITE_H_

#include "message.h"

void write_uint16_t(void* buf, uint16_t value);
int write_qname(void* buf, const char* qname);
int write_header(void* buf, struct header h);
int write_question(void* buf, struct question* q);

#endif