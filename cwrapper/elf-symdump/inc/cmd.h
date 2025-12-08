#ifndef CMD_H
#define CMD_H

typedef struct
{
    char *ofile;
    char *ifile;
    char **symbols;
    int symbol_count;
} CmdOptions;

void parse_cmd(int argc, char *argv[], CmdOptions *options);

#endif