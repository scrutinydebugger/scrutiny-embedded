//    cmd.c
//        Command line parsing
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "cmd.h"
#include "check.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void parse_cmd(int argc, char *argv[], CmdOptions *options)
{
    options->ifile = NULL;
    options->ofile = NULL;
    options->symbols = NULL;
    options->symbol_count = 0;
    options->symbol_name_prefix = NULL;

    int cursor = 1;

    while (cursor < argc)
    {

        if (strcmp(argv[cursor], "-i") == 0)
        {
            CHECK(cursor + 1 < argc, "Missing input file");
            CHECK(options->ifile == NULL, "Only one input file can be specified");
            options->ifile = argv[cursor + 1];
            cursor += 2;
            continue;
        }

        if (strcmp(argv[cursor], "-o") == 0)
        {
            CHECK(cursor + 1 < argc, "Missing output file");
            CHECK(options->ofile == NULL, "Only one output file can be specified");
            options->ofile = argv[cursor + 1];
            cursor += 2;
            continue;
        }

        if (strcmp(argv[cursor], "-p") == 0)
        {
            CHECK(cursor + 1 < argc, "Missing symbol name prefix");
            CHECK(options->symbol_name_prefix == NULL, "Only one symbol prefix can be specified");
            options->symbol_name_prefix = argv[cursor + 1];
            cursor += 2;
            continue;
        }

        options->symbols = &argv[cursor];
        options->symbol_count = argc - cursor;
        break;
    }

    CHECK(options->ifile != NULL, "Input file must be specified");
    CHECK(options->symbols != NULL, "No symbols were provided");
    CHECK(options->symbol_count > 0, "No symbols were provided");
}