#include "classes.h"
#include "utils.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DNS_CHECK_CLASS(strValue, value)          \
    {                                             \
        if (strcmp(dns_class, (strValue)) == 0) { \
            result = (value);                     \
            goto finish;                          \
        }                                         \
    }

uint16_t dns_class_to_int(const char* class)
{
    int size = strlen(class) + 1;
    char* dns_class = malloc(sizeof(char) * size);
    if (dns_class == NULL) {
        perror("Cannot allocate memory");
        exit(1);
    }

    uint16_t result = DNS_CLASS_ERROR;

    strupr(class, dns_class);
    DNS_CHECK_CLASS("IN", DNS_CLASS_IN)
    DNS_CHECK_CLASS("CH", DNS_CLASS_CH)
    DNS_CHECK_CLASS("HS", DNS_CLASS_HS)

finish:
    free(dns_class);
    return result;
}

#define DNS_CHECK_INT_CLASS(value, stdValue) \
    case (value):                            \
        return (stdValue);                   \
        break;

char* int_to_dns_class(uint16_t class)
{
    switch (class) {
        DNS_CHECK_INT_CLASS(DNS_CLASS_IN, "IN")
        DNS_CHECK_INT_CLASS(DNS_CLASS_CH, "CH")
        DNS_CHECK_INT_CLASS(DNS_CLASS_HS, "HS")
    default:
        return "ERROR";
        break;
    }
}
