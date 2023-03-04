//    main.cpp
//        Placeholder file for variable extraction test
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#include "file1.hpp"
#include "file2.hpp"
#include "argument_parser.hpp"
#include "scrutiny.hpp"
#include "abstract_comm_channel.hpp"
#include "udp_bridge.hpp"

#if SCRUTINY_BUILD_WINDOWS
#include "win_serial_port_bridge.hpp"
#include <windows.h>
using SerialPortBridge = WinSerialPortBridge;
#else
#include "nix_serial_port_bridge.hpp"
using SerialPortBridge = NixSerialPortBridge;
#endif

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <chrono>
#include <thread>
#include <algorithm>

using namespace std;

void mainfunc1()
{
    static int mainfunc1Var = 7777777;
    (void)mainfunc1Var;
}

void mainfunc1(int x)
{
    (void)x;
    static double mainfunc1Var = 8888888.88;
    (void)mainfunc1Var;
}

void memdump(uintptr_t startAddr, uint32_t length)
{
    uintptr_t addr = startAddr;
    while (addr < startAddr + length)
    {
        uint8_t *ptr = reinterpret_cast<uint8_t *>(addr);
        cout << "0x" << hex << setw(16) << setfill('0') << addr << ":\t";
        uintptr_t nToPrint = startAddr + length - addr;
        if (nToPrint > 16)
        {
            nToPrint = 16;
        }
        for (unsigned int i = 0; i < nToPrint; i++)
        {
            cout << hex << setw(2) << setfill('0') << static_cast<uint32_t>(ptr[i]);
        }
        cout << endl;
        addr += nToPrint;
    }
}

static uint8_t scrutiny_rx_buffer[128];
static uint8_t scrutiny_tx_buffer[256];

scrutiny::RuntimePublishedValue rpvs[] = {
    {0x1000, scrutiny::VariableType::sint8},
    {0x1001, scrutiny::VariableType::sint16},
    {0x1002, scrutiny::VariableType::sint32},

    {0x2000, scrutiny::VariableType::uint8},
    {0x2001, scrutiny::VariableType::uint16},
    {0x2002, scrutiny::VariableType::uint32},

    {0x3000, scrutiny::VariableType::float32},
    {0x4000, scrutiny::VariableType::boolean},

    {0x5000, scrutiny::VariableType::boolean},
    {0x5001, scrutiny::VariableType::uint16},

#if SCRUTINY_SUPPORT_64BITS
    {0x1003, scrutiny::VariableType::sint64},
    {0x2003, scrutiny::VariableType::uint64},
    {0x3001, scrutiny::VariableType::float64},
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
    bool rpv_id_4000;
    bool rpv_id_5000;
    uint16_t rpv_id_5001;

#if SCRUTINY_SUPPORT_64BITS
    int64_t rpv_id_1003;
    uint64_t rpv_id_2003;
    double rpv_id_3001;
#endif
} rpvStorage;

bool TestAppRPVReadCallback(const scrutiny::RuntimePublishedValue rpv, scrutiny::AnyType *outval)
{
    bool ok = true;
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
    else if (rpv.id == 0x4000)
    {
        outval->boolean = rpvStorage.rpv_id_4000;
    }
    else if (rpv.id == 0x5000)
    {
        outval->boolean = rpvStorage.rpv_id_5000;
    }
    else if (rpv.id == 0x5001)
    {
        outval->uint16 = rpvStorage.rpv_id_5001;
    }
    else
    {
        ok = false;
    }

    return ok;
}

bool TestAppRPVWriteCallback(const scrutiny::RuntimePublishedValue rpv, const scrutiny::AnyType *inval)
{
    bool ok = true;
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
    else if (rpv.id == 0x4000)
    {
        rpvStorage.rpv_id_4000 = inval->boolean;
    }
    else if (rpv.id == 0x5000)
    {
        rpvStorage.rpv_id_5000 = inval->boolean;
    }
    else if (rpv.id == 0x5001)
    {
        rpvStorage.rpv_id_5001 = inval->uint16;
    }
    else
    {
        ok = false;
    }

    return ok;
}

void init_all_values()
{
    file1SetValues();
    file2SetValues();
    funcInFile1(1, 2);
    file2func1();
    file2func1(123);
    mainfunc1();
    mainfunc1(123);

    rpvStorage.rpv_id_1000 = -10;
    rpvStorage.rpv_id_1001 = -30000;
    rpvStorage.rpv_id_1002 = 0xABCDEF;
    rpvStorage.rpv_id_2000 = 0xABu;
    rpvStorage.rpv_id_2001 = 0x1234u;
    rpvStorage.rpv_id_2002 = 0x12345678u;
    rpvStorage.rpv_id_3000 = 3.1415926f;
    rpvStorage.rpv_id_4000 = true;

    rpvStorage.rpv_id_5000 = false;
    rpvStorage.rpv_id_5001 = 0;

#if SCRUTINY_SUPPORT_64BITS
    rpvStorage.rpv_id_1003 = 0xAABBCCDDEEFF;
    rpvStorage.rpv_id_2003 = 0x111111111111u;
    rpvStorage.rpv_id_3001 = 2.71828;
#endif
}

void process_interactive_data()
{
    static bool enable = false;
    static uint16_t counter = 0;

    if (rpvStorage.rpv_id_5000)
    {
        rpvStorage.rpv_id_5001++;
    }

    if (enable)
    {
        counter++;
    }
}

void process_scrutiny_lib(AbstractCommChannel *channel)
{
    uint8_t buffer[1024];
    static_assert(sizeof(buffer) <= 0xFFFF, "Scrutiny expect a buffer smaller than 16 bits");
    scrutiny::MainHandler scrutiny_handler;
    scrutiny::Config config;
    scrutiny::VariableFrequencyLoopHandler vf_loop("Variable freq");
    scrutiny::FixedFrequencyLoopHandler ff_loop(200000, "50Hz");
    scrutiny::LoopHandler *loops[] = {&ff_loop, &vf_loop};
    config.set_buffers(scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer), scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer));
    config.set_published_values(rpvs, sizeof(rpvs) / sizeof(scrutiny::RuntimePublishedValue), TestAppRPVReadCallback, TestAppRPVWriteCallback);
    config.set_loops(loops, sizeof(loops) / sizeof(loops[0]));

#if SCRUTINY_ENABLE_DATALOGGING
    static uint8_t datalogging_buffer[4096];
    config.set_datalogging_buffers(datalogging_buffer, sizeof(datalogging_buffer));
#endif

    config.max_bitrate = 100000;
    config.display_name = "TestApp Executable";
    config.session_counter_seed = 0xdeadbeef;
    scrutiny_handler.init(&config);

    chrono::time_point<chrono::steady_clock> last_timestamp, now_timestamp;
    last_timestamp = chrono::steady_clock::now();
    now_timestamp = chrono::steady_clock::now();

    try
    {
        channel->start();
        int len_received = 0;
        while (true)
        {
            process_interactive_data();
            len_received = channel->receive(buffer, sizeof(buffer)); // Non-blocking. Can return 0
            now_timestamp = chrono::steady_clock::now();
            uint32_t timestep = static_cast<uint32_t>(chrono::duration_cast<chrono::microseconds>(now_timestamp - last_timestamp).count());

            if (len_received > 0)
            {
                cout << "in:  (" << dec << len_received << ")\t";
                for (int i = 0; i < len_received; i++)
                {
                    cout << hex << setw(2) << setfill('0') << static_cast<uint32_t>(buffer[i]);
                }
                cout << endl;
            }

            scrutiny_handler.comm()->receive_data(buffer, static_cast<uint16_t>(len_received));

            uint16_t data_to_send = scrutiny_handler.comm()->data_to_send();
            data_to_send = min(data_to_send, static_cast<uint16_t>(sizeof(buffer)));

            if (data_to_send > 0)
            {
                scrutiny_handler.comm()->pop_data(buffer, data_to_send);
                channel->send(buffer, data_to_send);

                cout << "out: (" << dec << data_to_send << ")\t";
                for (unsigned int i = 0; i < data_to_send; i++)
                {
                    cout << hex << setw(2) << setfill('0') << static_cast<uint32_t>(buffer[i]);
                }
                cout << endl;
            }

            scrutiny_handler.process(timestep);
            vf_loop.process(timestep);
            ff_loop.process();
#if SCRUTINY_BUILD_WINDOWS
            Sleep(10);
#else
            this_thread ::sleep_for(chrono::milliseconds(10));
#endif
            last_timestamp = now_timestamp;
        }
    }
    catch (std::exception const &e)
    {
        cerr << e.what() << endl;
    }

    channel->stop();
}

int main(int argc, char *argv[])
{
    static_assert(sizeof(char) == 1, "testapp doesn't support char bigger than 8 bits (yet)");

    int errorcode = 0;
    static int staticIntInMainFunc = 22222;
    (void)staticIntInMainFunc;
    init_all_values();

    ArgumentParser parser;
    parser.parse(argc, argv);

    if (!parser.is_valid())
    {
        cerr << "Invalid usage" << endl
             << parser.error_message() << endl;
        errorcode = -1;
    }
    else
    {
        if (parser.command() == TestAppCommand::Memdump)
        {
            MemoryRegion region;
            while (parser.has_another_memory_region())
            {
                try
                {
                    parser.next_memory_region(&region);
                    memdump(region.start_address, region.length);
                }
                catch (std::exception const &e)
                {
                    cerr << e.what() << endl;
                    errorcode = -1;
                    break;
                }
            }
        }
        // Listen on a UDP port, reply to the address of the last sender.

        else if (parser.command() == TestAppCommand::UdpListen)
        {
            cout << "UDP comm on port " << parser.udp_port() << endl;

            UdpBridge::global_init();
            UdpBridge udp_bridge(parser.udp_port());

            process_scrutiny_lib(&udp_bridge);

            UdpBridge::global_close();
        }
        else if (parser.command() == TestAppCommand::SerialListen)
        {
            auto serial_config = parser.serial_config();
            SerialPortBridge serial(serial_config.port_name, serial_config.baudrate);
            cout << "Serial comm on " << serial_config.port_name << " @" << serial_config.baudrate << " baud" << endl;

            process_scrutiny_lib(&serial);
        }
    }

    return errorcode;
}
