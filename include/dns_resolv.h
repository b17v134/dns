#ifndef _DNS_RESOLV_H_
#define _DNS_RESOLV_H_

#include "message.h"

#include <stdint.h>
#include <stdbool.h>

uint16_t get_flags(uint8_t qr, uint8_t rd);
uint8_t create_request(struct question *question, void *buf, uint16_t buf_size);
int resolv(const struct request r, char *buffer);
struct response get_response(void *buffer, int len);

#endif