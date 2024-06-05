#include "tools.h"
#include <stddef.h>

size_t c_testapp_strncpy(char *const dst, const char *const src, size_t const maxlen)
{
    size_t n = 0;
    for (; n < maxlen; n++)
    {
        dst[n] = src[n];
        if (src[n] == '\0')
        {
            break;
        }
    }

    if (n >= maxlen)
    {
        dst[maxlen - 1] = '\0';
    }
    return n;
}
