#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>

// https://www.iana.org/assignments/dns-parameters/dns-parameters.xhtml#dns-parameters-4
#define DNS_TYPE_ERROR 0
#define DNS_TYPE_A 1
#define DNS_TYPE_NS 2
#define DNS_TYPE_MD 3
#define DNS_TYPE_MF 4
#define DNS_TYPE_CNAME 5

#define DNS_TYPE_AAAA 28

uint16_t dns_type_to_int(const char *type);
char* int_to_dns_type(uint16_t type);

#endif