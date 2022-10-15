#include "types.h"

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void strupr(const char *str, char *result)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        result[i] = toupper(str[i]);
    }
    result[strlen(str) + 1] = '\0';
}

#define DNS_CHECK_TYPE(strValue, value)        \
    {                                          \
        if (strcmp(dns_type, (strValue)) == 0) \
        {                                      \
            result = (value);                  \
            goto finish;                       \
        }                                      \
    }

uint16_t dns_type_to_int(const char *type)
{
    int size = strlen(type) + 1;
    char *dns_type = malloc(sizeof(char) * size);
    if (dns_type == NULL)
    {
        perror("Cannot allocate memory");
        exit(1);
    }

    strupr(type, dns_type);

    uint16_t result = DNS_TYPE_ERROR;

    DNS_CHECK_TYPE("A", DNS_TYPE_A)
    DNS_CHECK_TYPE("NS", DNS_TYPE_NS)
    DNS_CHECK_TYPE("MD", DNS_TYPE_MD)
    DNS_CHECK_TYPE("AAAA", DNS_TYPE_AAAA)

finish:
    free(dns_type);
    return result;
}