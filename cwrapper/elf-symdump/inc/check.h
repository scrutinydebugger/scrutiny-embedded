//    main.h
//        Main header
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-elf-symdump)
//
//    Copyright (c) 2025 Scrutiny Debugger

#ifndef MAIN_H
#define MAIN_H

#include "stdio.h"
#include "stdlib.h"

#define CHECK(cond, msg)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(cond))                                                                                                   \
        {                                                                                                              \
            fprintf(stderr, "%s\n", msg);                                                                              \
            fflush(stderr);                                                                                            \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
                                                                                                                       \
    } while (0)

#define UNREACHABLE()                                                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        fprintf(stderr, "unreachable code\n");                                                                         \
        fflush(stderr);                                                                                                \
        fflush(stdout);                                                                                                \
        abort();                                                                                                       \
    } while (0)

#endif