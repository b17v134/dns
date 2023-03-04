#ifndef _WRITE_H_
#define _WRITE_H_

#include "message.h"

void write_uint16_t(uint8_t* buf, uint16_t value);
int write_qname(uint8_t* buf, const char* qname);
int write_header(uint8_t* buf, struct header h);
int write_question(uint8_t* buf, struct question* q);

#endif
