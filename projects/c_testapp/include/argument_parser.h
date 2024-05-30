//    argument_parser.h
//        For CLI parsing
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___ARGUMENT_PARSER_H___
#define ___ARGUMENT_PARSER_H___

#include <stdint.h>

typedef enum
{
    C_TESTAPP_COMMAND_None,
    C_TESTAPP_COMMAND_Memdump,
    C_TESTAPP_COMMAND_UdpListen,
    C_TESTAPP_COMMAND_SerialListen
} c_testapp_command_e;

typedef struct
{
    uintptr_t start_address;
    uint32_t length;
} c_testapp_memory_region_t;

typedef struct
{
    char port_name[255];
    uint32_t baudrate;
} c_testapp_serial_config_t;

typedef struct
{
    int m_valid;
    c_testapp_command_e m_command;
    unsigned int m_region_index;
    unsigned int m_argc;
    char **m_argv;
    uint16_t m_udp_port;
    c_testapp_serial_config_t m_serial_config;
    char *m_last_error;
} c_testapp_argument_parser_t;

typedef enum
{
    C_TESTAPP_ARGPARSE_ERROR_NoError,
    C_TESTAPP_ARGPARSE_ERROR_WrongCommand,
    C_TESTAPP_ARGPARSE_ERROR_Depleted
} c_testapp_argument_parser_error_e;

void c_testapp_argument_parser_init(c_testapp_argument_parser_t *parser);
void c_testapp_argument_parser_parse(c_testapp_argument_parser_t *parser, int argc, char *argv[]);
c_testapp_argument_parser_error_e c_testapp_argument_parser_next_memory_region(c_testapp_argument_parser_t *parser, c_testapp_memory_region_t *region);
int c_testapp_argument_parser_has_another_memory_region(c_testapp_argument_parser_t *parser);

#endif // ___ARGUMENT_PARSER_H___
