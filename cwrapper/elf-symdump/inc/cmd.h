//    cmd.h
//        Command line parsing
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef CMD_H
#define CMD_H

typedef struct
{
    char *ofile;
    char *ifile;
    char **symbols;
    int symbol_count;
    char* symbol_name_prefix;
} CmdOptions;

void parse_cmd(int argc, char *argv[], CmdOptions *options);

#endif