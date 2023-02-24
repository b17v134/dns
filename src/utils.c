#include "utils.h"

#include <ctype.h>
#include <string.h>

void strupr(const char* str, char* result)
{
    for (int i = 0; str[i] != '\0'; i++) {
        result[i] = toupper(str[i]);
    }
    result[strlen(str)] = '\0';
}
