//    scrutiny_tools.h
//        Some simple tools used across the project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef __SCRUTINY_TOOLS_H__
#define __SCRUTINY_TOOLS_H__

#include <cstdlib>

namespace scrutiny
{
    namespace tools
    {
        inline size_t strnlen(const char* s, size_t maxlen)
        {
            size_t n=0;
            for(; n<maxlen; n++)
            {
                if (s[n] == '\0')
                {
                    break;
                }
            }

            return n;
        }
    }
}


#endif //__SCRUTINY_TOOLS_H__