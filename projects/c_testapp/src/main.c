//    main.c
//        Entry point of C TestApp
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "argument_parser.h"
#include "scrutiny_cwrapper.h"
#include "udp_bridge.h"

#if SCRUTINY_BUILD_WINDOWS
#include "win_serial_port_bridge.h"
#include <windows.h>
#else
#include <unistd.h>
#include "nix_serial_port_bridge.h"
#endif


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#include "file1.h"
#include "file2.h"

#ifndef max
#define max(a,b) ((a) > (b)) ? (a) : (b)
#endif

#ifndef min
#define min(a,b) ((a) < (b)) ? (a) : (b)
#endif

static uint64_t micros()
{
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    return ((uint64_t) now.tv_sec) * 1000000 + ((uint64_t) now.tv_nsec) / 1000;
}

void mainfunc1()
{
    static volatile int mainfunc1Var = 7777777;
    (void)mainfunc1Var;
}

void memdump(uintptr_t startAddr, uint32_t length)
{
    uintptr_t addr = startAddr;
    while (addr < startAddr + length)
    {
        uint8_t *ptr = (uint8_t*)(addr);
        printf("0x%016" PRIx64 ":\t", (uint64_t)addr);
        uintptr_t nToPrint = startAddr + length - addr;
        if (nToPrint > 16)
        {
            nToPrint = 16;
        }
        for (unsigned int i = 0; i < nToPrint; i++)
        {
            printf("%02"PRIx32, (uint32_t)ptr[i]);
        }
        printf("\n");
        addr += nToPrint;
    }
}

static uint8_t scrutiny_rx_buffer[128];
static uint8_t scrutiny_tx_buffer[256];

scrutiny_c_runtime_published_value_t rpvs[] = {
    {0x1000, SCRUTINY_C_VARIABLE_TYPE_sint8},
    {0x1001, SCRUTINY_C_VARIABLE_TYPE_sint16},
    {0x1002, SCRUTINY_C_VARIABLE_TYPE_sint32},

    {0x2000, SCRUTINY_C_VARIABLE_TYPE_uint8},
    {0x2001, SCRUTINY_C_VARIABLE_TYPE_uint16},
    {0x2002, SCRUTINY_C_VARIABLE_TYPE_uint32},

    {0x3000, SCRUTINY_C_VARIABLE_TYPE_float32},

#if SCRUTINY_SUPPORT_64BITS
    {0x1003, SCRUTINY_C_VARIABLE_TYPE_sint64},
    {0x2003, SCRUTINY_C_VARIABLE_TYPE_uint64},
    {0x3001, SCRUTINY_C_VARIABLE_TYPE_float64},
#endif

};

struct
{
    int8_t rpv_id_1000;
    int16_t rpv_id_1001;
    int32_t rpv_id_1002;

    uint8_t rpv_id_2000;
    uint16_t rpv_id_2001;
    uint32_t rpv_id_2002;

    float rpv_id_3000;

#if SCRUTINY_SUPPORT_64BITS
    int64_t rpv_id_1003;
    uint64_t rpv_id_2003;
    double rpv_id_3001;
#endif
} rpvStorage;

int TestAppRPVReadCallback(const scrutiny_c_runtime_published_value_t rpv, scrutiny_c_any_type_t *outval)
{
    int ok = 1;
    if (rpv.id == 0x1000)
    {
        outval->sint8 = rpvStorage.rpv_id_1000;
    }
    else if (rpv.id == 0x1001)
    {
        outval->sint16 = rpvStorage.rpv_id_1001;
    }
    else if (rpv.id == 0x1002)
    {
        outval->sint32 = rpvStorage.rpv_id_1002;
    }
#if SCRUTINY_SUPPORT_64BITS
    else if (rpv.id == 0x1003)
    {
        outval->sint64 = rpvStorage.rpv_id_1003;
    }
#endif
    else if (rpv.id == 0x2000)
    {
        outval->uint8 = rpvStorage.rpv_id_2000;
    }
    else if (rpv.id == 0x2001)
    {
        outval->uint16 = rpvStorage.rpv_id_2001;
    }
    else if (rpv.id == 0x2002)
    {
        outval->uint32 = rpvStorage.rpv_id_2002;
    }
#if SCRUTINY_SUPPORT_64BITS
    else if (rpv.id == 0x2003)
    {
        outval->uint64 = rpvStorage.rpv_id_2003;
    }
#endif
    else if (rpv.id == 0x3000)
    {
        outval->float32 = rpvStorage.rpv_id_3000;
    }
#if SCRUTINY_SUPPORT_64BITS
    else if (rpv.id == 0x3001)
    {
        outval->float64 = rpvStorage.rpv_id_3001;
    }
#endif
    else
    {
        ok = 0;
    }

    return ok;
}

int TestAppRPVWriteCallback(const scrutiny_c_runtime_published_value_t rpv, const scrutiny_c_any_type_t *inval)
{
    int ok = 1;
    if (rpv.id == 0x1000)
    {
        rpvStorage.rpv_id_1000 = inval->sint8;
    }
    else if (rpv.id == 0x1001)
    {
        rpvStorage.rpv_id_1001 = inval->sint16;
    }
    else if (rpv.id == 0x1002)
    {
        rpvStorage.rpv_id_1002 = inval->sint32;
    }
#if SCRUTINY_SUPPORT_64BITS
    else if (rpv.id == 0x1003)
    {
        rpvStorage.rpv_id_1003 = inval->sint64;
    }
#endif
    else if (rpv.id == 0x2000)
    {
        rpvStorage.rpv_id_2000 = inval->uint8;
    }
    else if (rpv.id == 0x2001)
    {
        rpvStorage.rpv_id_2001 = inval->uint16;
    }
    else if (rpv.id == 0x2002)
    {
        rpvStorage.rpv_id_2002 = inval->uint32;
    }
#if SCRUTINY_SUPPORT_64BITS
    else if (rpv.id == 0x2003)
    {
        rpvStorage.rpv_id_2003 = inval->uint64;
    }
#endif
    else if (rpv.id == 0x3000)
    {
        rpvStorage.rpv_id_3000 = inval->float32;
    }
#if SCRUTINY_SUPPORT_64BITS
    else if (rpv.id == 0x3001)
    {
        rpvStorage.rpv_id_3001 = inval->float64;
    }
#endif
    else
    {
        ok = 0;
    }

    return ok;
}

void init_all_values()
{
    file1SetValues();
    file2SetValues();
    funcInFile1(1, 2);
    file2func1(123);
    mainfunc1();

    rpvStorage.rpv_id_1000 = -10;
    rpvStorage.rpv_id_1001 = -30000;
    rpvStorage.rpv_id_1002 = 0xABCDEF;
    rpvStorage.rpv_id_2000 = 0xABu;
    rpvStorage.rpv_id_2001 = 0x1234u;
    rpvStorage.rpv_id_2002 = 0x12345678u;
    rpvStorage.rpv_id_3000 = 3.1415926f;

#if SCRUTINY_SUPPORT_64BITS
    rpvStorage.rpv_id_1003 = 0xAABBCCDDEEFF;
    rpvStorage.rpv_id_2003 = 0x111111111111u;
    rpvStorage.rpv_id_3001 = 2.71828;
#endif


}

void process_interactive_data()
{
    static volatile int enable = 0;
    static volatile int16_t counter = 0;
    static volatile int16_t step = 1;

    (void)(enable);  // suppress unused variable error
    (void)(counter); // suppress unused variable error
    (void)(step);    // suppress unused variable error

    if (enable)
    {
        counter += step;
    }
}

void datalogging_callback()
{
    printf("Graph Triggered!\n");
}

void my_user_command(
    const uint8_t subfunction,
    const uint8_t *request_data,
    const uint16_t request_data_length,
    uint8_t *response_data,
    uint16_t *response_data_length,
    const uint16_t response_max_data_length)
{
    printf("User command: Subfunction #%u with %u data bytes: ", 
        (unsigned int)subfunction, 
        (unsigned int) request_data_length);
    for (uint32_t i = 0; i < request_data_length; i++)
    {
        printf("%02x", (unsigned int) request_data[i]);
    }
    printf("\n");

    if (response_max_data_length < 1)
    {
        *response_data_length = 0;
        return;
    }

    if (subfunction == 0 && response_max_data_length >= 8)
    {
        *response_data_length = 8;
        response_data[0] = subfunction;
        for (uint8_t i = 0; i < 7; i++)
        {
            response_data[i + 1] = i;
        }
    }
    else if (subfunction == 1)
    {
        *response_data_length = response_max_data_length;
        response_data[0] = subfunction;
        for (uint16_t i = 1; i < response_max_data_length; i++)
        {
            response_data[i] = (uint8_t)i;
        }
    }
    else if (subfunction == 2)
    {
        *response_data_length = response_max_data_length + 1;
    }
    else if (subfunction == 3)
    {
        *response_data_length = 0;
    }
    else if (subfunction == 4)
    {
        const uint16_t max_echo_size = min(request_data_length, (uint16_t)(response_max_data_length - 1));
        response_data[0] = subfunction;
        for (uint16_t i = 0; i < max_echo_size; i++)
        {
            response_data[i + 1] = request_data[i];
        }
        *response_data_length = max_echo_size + 1;
    }
    else
    {
        *response_data_length = 1;
        response_data[0] = 0xFF;
    }
}

enum channel_type_e{
    CHANNEL_TYPE_UDP,
    CHANNEL_TYPE_Serial
};

#define ERR_RETURN(msg) {fprintf(stderr, "%s\n", msg); return;}
#define ERR_BREAK(msg) {fprintf(stderr, "%s\n", msg); break;}

void process_scrutiny_lib(comm_channel_interface_t *channel)
{

    uint8_t buffer[1024];
    scrutiny_c_main_handler_t* scrutiny_handler = scrutiny_c_main_handler_construct(malloc(SCRUTINY_C_MAIN_HANDLER_SIZE), SCRUTINY_C_MAIN_HANDLER_SIZE);
    scrutiny_c_config_t* config = scrutiny_c_config_construct(malloc(SCRUTINY_C_CONFIG_SIZE), SCRUTINY_C_CONFIG_SIZE);
    
    scrutiny_c_loop_handler_ff_t* ff_loop = scrutiny_c_loop_handler_fixed_freq_construct(
        malloc(SCRUTINY_C_LOOP_HANDLER_FF_SIZE), 
        SCRUTINY_C_LOOP_HANDLER_FF_SIZE,
        100000,
        "100Hz Loop"
        );
    scrutiny_c_loop_handler_vf_t* vf_loop = scrutiny_c_loop_handler_variable_freq_construct(
        malloc(SCRUTINY_C_LOOP_HANDLER_VF_SIZE), 
        SCRUTINY_C_LOOP_HANDLER_VF_SIZE,
        "Variable freq loop"
        );

    if (scrutiny_handler == NULL) ERR_RETURN("Failed to allocate scrutiny_handler");
    if (config == NULL) ERR_RETURN("Failed to allocate config");
    if (ff_loop == NULL) ERR_RETURN("Failed to allocate ff_loop");
    if (vf_loop == NULL) ERR_RETURN("Failed to allocate vf_loop");
    
    scrutiny_c_loop_handler_t *loops[] = {ff_loop, vf_loop};

    scrutiny_c_config_set_buffers(config, scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer), scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer));
    scrutiny_c_config_set_published_values(config, rpvs, sizeof(rpvs) / sizeof(scrutiny_c_runtime_published_value_t), TestAppRPVReadCallback, TestAppRPVWriteCallback);
    scrutiny_c_config_set_loops(config, loops, sizeof(loops) / sizeof(loops[0]));
    scrutiny_c_config_set_user_command_callback(config, my_user_command);

#if SCRUTINY_ENABLE_DATALOGGING
    static uint8_t datalogging_buffer[4096];
    scrutiny_c_config_set_datalogging_trigger_callback(config, datalogging_callback);
    scrutiny_c_config_set_datalogging_buffers(config, datalogging_buffer, sizeof(datalogging_buffer));
#endif

    scrutiny_c_config_set_max_bitrate(config, 100000);
    scrutiny_c_config_set_display_name(config, "TestApp Executable");
    scrutiny_c_config_set_session_counter_seed(config, 0xdeadbeef);
    
    scrutiny_c_main_handler_init(scrutiny_handler, config);
    
    uint64_t start_timestamp, last_timestamp, now_timestamp;
    start_timestamp = micros();
    last_timestamp = micros();
    now_timestamp = micros();

    if (channel->start(channel->handle) != COMM_CHANNEL_STATUS_success){
        ERR_RETURN("Failed to start comm channel");
    }

    int len_received = 0;
    while (1)
    {
        process_interactive_data();

        if (channel->receive(channel->handle, buffer, sizeof(buffer), &len_received) != COMM_CHANNEL_STATUS_success){
            ERR_BREAK("Error while receiving");
        }

        now_timestamp = micros();
        uint64_t time_since_start = (now_timestamp - start_timestamp);
        uint32_t timestep_us = (uint32_t)(now_timestamp - last_timestamp);

        if (len_received > 0)
        {
            printf("%" PRIu64 "\tin: (%d)\t", time_since_start, len_received);
            for (int i = 0; i < len_received; i++)
            {
                printf("%02x", (unsigned int)buffer[i]);
            }
            printf("\n");
        }

        scrutiny_c_main_handler_receive_data(scrutiny_handler, buffer, (uint16_t)len_received);

        scrutiny_c_main_handler_process(scrutiny_handler, timestep_us * 10);

        uint16_t data_to_send =  scrutiny_c_main_handler_data_to_send(scrutiny_handler);
        data_to_send = min(data_to_send, (uint16_t)sizeof(buffer));

        if (data_to_send > 0)
        {
            scrutiny_c_main_handler_pop_data(scrutiny_handler, buffer, data_to_send);
            
            if (channel->send(channel->handle, buffer, data_to_send) != COMM_CHANNEL_STATUS_success){
                ERR_BREAK("Failed to start comm channel");
            }

            printf("%" PRIu64 "\tout:  (%d)\t", time_since_start, data_to_send);
            for (int i = 0; i < data_to_send; i++)
            {
                printf("%02x", (unsigned int)buffer[i]);
            }
            printf("\n");
        }

        scrutiny_c_loop_handler_variable_freq_process(vf_loop, timestep_us*10);
        scrutiny_c_loop_handler_fixed_freq_process(ff_loop);
#if SCRUTINY_BUILD_WINDOWS
        Sleep(10);
#else
        usleep(10000);
#endif
        last_timestamp = now_timestamp;
    }

    if (channel->stop(channel->handle) != COMM_CHANNEL_STATUS_success){
        ERR_RETURN("Error while stopping comm channel");
    }
}

int main(int argc, char *argv[])
{
    int errorcode = 0;
    static volatile int staticIntInMainFunc = 22222;
    (void)staticIntInMainFunc;
    init_all_values();

    c_testapp_argument_parser_t parser;
    c_testapp_argument_parser_init(&parser);
    c_testapp_argument_parser_parse(&parser, argc, argv);

    if (!parser.m_valid)
    {
        fprintf(stderr, "Invalid usage. %s\n", parser.m_last_error);
        errorcode = -1;
    }
    else
    {
        if (parser.m_command == C_TESTAPP_COMMAND_Memdump)
        {
            c_testapp_memory_region_t region;
            while (c_testapp_argument_parser_has_another_memory_region(&parser))
            {
                c_testapp_argument_parser_error_e err = c_testapp_argument_parser_next_memory_region(&parser, &region);
                memdump(region.start_address, region.length);

                if(err != C_TESTAPP_ARGPARSE_ERROR_NoError)
                {
                    fprintf(stderr, "Bad memory region arguments. %d\n", (int)err);
                    errorcode = -1;
                    break;
                }
            }
        }
        // Listen on a UDP port, reply to the address of the last sender.

        else if (parser.m_command == C_TESTAPP_COMMAND_UdpListen)
        {
            printf("UDP comm on port %u\n", parser.m_udp_port);

            comm_channel_interface_t channel;
            if (udp_bridge_global_init() == COMM_CHANNEL_STATUS_success)
            {
                udp_bridge_t udp_bridge;
                udp_bridge_init(&udp_bridge, parser.m_udp_port);
                channel.handle = &udp_bridge;
                channel.start = (comm_channel_start_t)udp_bridge_start;
                channel.stop = (comm_channel_stop_t)udp_bridge_stop;
                channel.send = (comm_channel_send_t)udp_bridge_send;
                channel.receive = (comm_channel_receive_t)udp_bridge_receive;

                process_scrutiny_lib(&channel);
            }

            udp_bridge_global_close();
        }
        else if (parser.m_command == C_TESTAPP_COMMAND_SerialListen)
        {
            printf("Serial comm on %s@%u baud\n", parser.m_serial_config.port_name, parser.m_serial_config.baudrate);
            comm_channel_interface_t channel;
            #if SCRUTINY_BUILD_WINDOWS
            win_serial_port_t serial;
            channel.handle = &serial;
            channel.start = win_serial_port_start;
            channel.stop = win_serial_port_stop;
            channel.send = win_serial_port_send;
            channel.receive = win_serial_port_receive;

            win_serial_port_init(&serial, parser.m_serial_config.port_name, parser.m_serial_config.baudrate);
            #else
            nix_serial_port_t serial;
            channel.handle = &serial;
            channel.start = (comm_channel_start_t)nix_serial_port_start;
            channel.stop = (comm_channel_stop_t)nix_serial_port_stop;
            channel.send = (comm_channel_send_t)nix_serial_port_send;
            channel.receive = (comm_channel_receive_t)nix_serial_port_receive;

            nix_serial_port_init(&serial, parser.m_serial_config.port_name, parser.m_serial_config.baudrate);
            #endif

            process_scrutiny_lib(&channel);
        }
    }

    return errorcode;
}
