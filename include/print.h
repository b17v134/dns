#ifndef _PRINT_H_
#define _PRINT_H_

#include "dns_resolv.h"

void print_header(const struct header hdr);
void print_resource_record(const struct resource_record record);
void print_response(const struct response resp);

#endif