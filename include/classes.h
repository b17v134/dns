#ifndef _CLASSES_H_
#define _CLASSES_H_

#include <stdint.h>

#define DNS_CLASS_ERROR 0
#define DNS_CLASS_IN 1
#define DNS_CLASS_CH 3
#define DNS_CLASS_HS 4

uint16_t dns_class_to_int(const char *class);
char *int_to_dns_class(uint16_t class);

#endif
