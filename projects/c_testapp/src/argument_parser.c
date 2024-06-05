//    argument_parser.c
//        For CLI parsing
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "argument_parser.h"
#include "tools.h"


void c_testapp_argument_parser_init(c_testapp_argument_parser_t *parser)
{
    parser->m_valid = 0;
    parser->m_command = C_TESTAPP_COMMAND_None;
    parser->m_region_index = 0;
    parser->m_argc = 0;
    parser->m_argv = NULL;
    parser->m_last_error = "";
}

void c_testapp_argument_parser_parse(c_testapp_argument_parser_t *parser, int argc, char* argv[])
{
    parser->m_argc = argc;
    parser->m_argv = argv;
    if (argc < 2)
    {
        parser->m_valid = 0;
        parser->m_last_error = "Missing command";
        return;
    }

    char cmd[255] = {0};
    c_testapp_strncpy(cmd, argv[1], sizeof(cmd));
    size_t len = strlen(cmd);
    for (size_t i=0; i<len; i++){
        cmd[i] = (char)tolower(cmd[i]);
    }

    if (strcmp(cmd, "memdump") == 0)
    {
        parser->m_command = C_TESTAPP_COMMAND_Memdump;

        if (argc >= 4 && argc % 2 == 0)
        {
            parser->m_valid = 1;
        }
        else
        {
            parser->m_last_error = "Bad number of arguments";
        }
    }
    else if ( strcmp(cmd, "udp-listen") == 0)
    {
        parser->m_command = C_TESTAPP_COMMAND_UdpListen;
        if (argc >= 3)
        {
            int32_t port = atoi(argv[2]);
            if (port > 0 && port <0x10000)
            {
                parser->m_udp_port = (uint16_t)port;
                parser->m_valid = 1;
            }
            else
            {
                parser->m_last_error = "Port not in range 0-65535";
            }
        }
        else
        {
             parser->m_last_error = "Missing port";
        }
    }
    else if (strcmp(cmd, "serial-listen") == 0)
    {
        parser->m_command = C_TESTAPP_COMMAND_SerialListen;
        if (argc >= 3)
        {
            parser->m_serial_config.baudrate = 115200;

            c_testapp_strncpy(parser->m_serial_config.port_name, argv[2], sizeof(parser->m_serial_config.port_name)) ;
            int arg_error = 0;
            for (int32_t i=3; i<argc; i++)
            {
                char const* arg = argv[i];

                if (strcmp(arg, "--baudrate") == 0)
                {
                    if (i+1 >= argc)
                    {
                        parser->m_last_error = "Missing baudrate";
                        arg_error = 1;
                        break;
                    }
                    
                    int32_t baudrate = atoi(parser->m_argv[i+1]);
                    if(baudrate <= 0 || baudrate > 0x7FFFFFFF)
                    {
                        parser->m_last_error = "Invalid baudrate";
                        arg_error = 1;
                        break;
                    }
                    parser->m_serial_config.baudrate = (uint32_t)baudrate;
                }
            }

            if (!arg_error)
            {
                parser->m_valid = 1;
            }
        }
        else
        {
            parser->m_last_error = "Missing port name";
        }
    }
    else
    {
        parser->m_last_error = "Unknown command";
    }
}

int c_testapp_argument_parser_has_another_memory_region(c_testapp_argument_parser_t *parser)
{
    if (parser->m_argc < 2)
    {
        return 0;
    }

    if (parser->m_argc - 2 < parser->m_region_index + 1)
    {
        return 0;
    }

    return 1;
}

c_testapp_argument_parser_error_e c_testapp_argument_parser_next_memory_region(c_testapp_argument_parser_t *parser, c_testapp_memory_region_t *region)
{
    uint32_t const region_offset = 2;
    if (parser->m_command != C_TESTAPP_COMMAND_Memdump || !parser->m_valid)
    {
        return C_TESTAPP_ARGPARSE_ERROR_WrongCommand;
    }

    if (!c_testapp_argument_parser_has_another_memory_region(parser))
    {
        return C_TESTAPP_ARGPARSE_ERROR_Depleted;
    }

    int base1 = 10;
    int base2 = 10;
    char start_address[32] = {0};
    c_testapp_strncpy(start_address, parser->m_argv[parser->m_region_index + region_offset], sizeof(start_address));

    if (strnlen(start_address, sizeof(start_address)) > 2 && start_address[0] == '0' && start_address[1] == 'x')
    {
        c_testapp_strncpy(start_address, &start_address[2], sizeof(start_address)-2);
        base1 = 16;
    }
    
    char length[32] = {0};
    c_testapp_strncpy(length, parser->m_argv[parser->m_region_index + region_offset +1], sizeof(length));

    if (strnlen(length, sizeof(length)) > 2 && length[0] == '0' && length[1] == 'x')
    {
        c_testapp_strncpy(length, &length[2], sizeof(length)-2);
        base2 = 16;
    }

    region->start_address = (uintptr_t) strtoll(start_address, NULL, base1);
    region->length = (uint32_t)(strtol(length, NULL, base2));

    parser->m_region_index += 2;
    return C_TESTAPP_ARGPARSE_ERROR_NoError;
}
