#ifndef __C_TESTAPP_TOOLS_H__
#define __C_TESTAPP_TOOLS_H__

#include <stddef.h>

size_t c_testapp_strncpy(char *const dst, const char *const src, const size_t maxlen)
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

#endif