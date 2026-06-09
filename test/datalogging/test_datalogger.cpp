//    test_datalogger.cpp
//        Test suite for the datalogger object. Test its capacity to log, trigger access bitfields
//        and report error on bad config.
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutinytest/scrutinytest.hpp"
#include <vector>

#include "raw_format_parser.hpp"
#include "scrutiny.hpp"
#include "scrutiny_test.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>

using namespace scrutiny;
using namespace std;

static uint32_t g_u32_rpv1000 = 0;
static uint32_t g_trigger_callback_count = 0;
static LoopHandler *g_last_rpv_callback_caller = SCRUTINY_NULL;

static unsigned char _rx_buffer[128];
static unsigned char _tx_buffer[128];

static unsigned char forbidden_buffer[32];
static unsigned char forbidden_buffer2[32];
static unsigned char readonly_buffer[32];
static unsigned char readonly_buffer2[32];

static struct
{
    unsigned char canary1[128];
    unsigned char data[128];
    unsigned char canary2[128];
} dlbuffer;

static struct
{
    unsigned char canary1[128];
    unsigned char data[SIZEOF_8BITS(dlbuffer.data) + 4];
    unsigned char canary2[128];
} output_buffer;

static bool rpv_read_callback(RuntimePublishedValue rpv, AnyType *outval, LoopHandler *const caller)
{
    g_last_rpv_callback_caller = caller;
    if (rpv.id == 0x1234 && rpv.type == VariableType::uint32)
    {
        outval->uint32 = 0xaabbccdd;
    }
    else if (rpv.id == 0x5678 && rpv.type == VariableType::float32)
    {
        outval->float32 = 3.1415926f;
    }
    else if (rpv.id == 0x1000 && rpv.type == VariableType::uint32)
    {
        outval->uint32 = g_u32_rpv1000;
    }
    else
    {
        return false;
    }

    return true;
}

static void trigger_callback(void)
{
    g_trigger_callback_count++;
}

class TestDatalogger : public ScrutinyTest
{
  protected:
    Timebase tb;
    MainHandler scrutiny_handler;
    Config config;
    datalogging::DataLogger datalogger;

    AddressRange readonly_ranges[2];
    AddressRange forbidden_ranges[2];
    RuntimePublishedValue rpvs[3];

    TestDatalogger() :
        ScrutinyTest(),
        tb(),
        scrutiny_handler(),
        config(),
        datalogger()
    {
        readonly_ranges[0] = tools::make_address_range(readonly_buffer, sizeof(readonly_buffer));
        readonly_ranges[1] = tools::make_address_range(readonly_buffer2, sizeof(readonly_buffer2));

        forbidden_ranges[0] = tools::make_address_range(forbidden_buffer, sizeof(forbidden_buffer));
        forbidden_ranges[1] = tools::make_address_range(forbidden_buffer2, sizeof(forbidden_buffer2));

        rpvs[0].id = 0x1234;
        rpvs[0].type = VariableType::uint32;
        rpvs[1].id = 0x5678;
        rpvs[1].type = VariableType::float32;
        rpvs[2].id = 0x1000;
        rpvs[2].type = VariableType::uint32;
    }

    virtual void SetUp()
    {
        config.set_buffers(_rx_buffer, sizeof(_rx_buffer), _tx_buffer, sizeof(_tx_buffer));
        config.set_readonly_address_range(readonly_ranges, sizeof(readonly_ranges) / sizeof(readonly_ranges[0]));
        config.set_forbidden_address_range(forbidden_ranges, sizeof(forbidden_ranges) / sizeof(forbidden_ranges[0]));
        config.set_published_values(rpvs, sizeof(rpvs) / sizeof(rpvs[0]), rpv_read_callback);

        scrutiny_handler.init(&config);
        datalogger.init(&scrutiny_handler, dlbuffer.data, sizeof(dlbuffer.data), trigger_callback);

#if CHAR_BIT == 8
        memset(dlbuffer.canary1, 0xAA, sizeof(dlbuffer.canary1));
        memset(dlbuffer.canary2, 0x55, sizeof(dlbuffer.canary2));
        memset(dlbuffer.data, 0xFF, sizeof(dlbuffer.data));
        memset(output_buffer.canary1, 0xAA, sizeof(output_buffer.canary1));
        memset(output_buffer.canary2, 0x55, sizeof(output_buffer.canary2));
        memset(output_buffer.data, 0xFF, sizeof(output_buffer.data));
#elif CHAR_BIT == 16
        memset(dlbuffer.canary1, 0xAAAA, sizeof(dlbuffer.canary1));
        memset(dlbuffer.canary2, 0x5555, sizeof(dlbuffer.canary2));
        memset(dlbuffer.data, 0xFFFF, sizeof(dlbuffer.data));
        memset(output_buffer.canary1, 0xAAAA, sizeof(output_buffer.canary1));
        memset(output_buffer.canary2, 0x5555, sizeof(output_buffer.canary2));
        memset(output_buffer.data, 0xFFFF, sizeof(output_buffer.data));
#endif
        g_trigger_callback_count = 0;
        g_last_rpv_callback_caller = SCRUTINY_NULL;
    }
};

#if CHAR_BIT == 8
#define CHECK_CANARIES                                                                                                                               \
    do                                                                                                                                               \
    {                                                                                                                                                \
        ASSERT_BUF_SET(dlbuffer.canary1, 0xAA, sizeof(dlbuffer.canary1)) << "dlbuffer canary died!";                                                 \
        ASSERT_BUF_SET(dlbuffer.canary2, 0x55, sizeof(dlbuffer.canary2)) << "dlbuffer canary died!";                                                 \
        ASSERT_BUF_SET(output_buffer.canary1, 0xAA, sizeof(output_buffer.canary1)) << "otuput_buffer canary died!";                                  \
        ASSERT_BUF_SET(output_buffer.canary2, 0x55, sizeof(output_buffer.canary2)) << "otuput_buffer canary died!";                                  \
    } while (0)

#elif CHAR_BIT == 16
#define CHECK_CANARIES                                                                                                                               \
    do                                                                                                                                               \
    {                                                                                                                                                \
        ASSERT_BUF_SET(dlbuffer.canary1, 0xAAAA, sizeof(dlbuffer.canary1)) << "dlbuffer canary died!";                                               \
        ASSERT_BUF_SET(dlbuffer.canary2, 0x5555, sizeof(dlbuffer.canary2)) << "dlbuffer canary died!";                                               \
        ASSERT_BUF_SET(output_buffer.canary1, 0xAAAA, sizeof(output_buffer.canary1)) << "otuput_buffer canary died!";                                \
        ASSERT_BUF_SET(output_buffer.canary2, 0x5555, sizeof(output_buffer.canary2)) << "otuput_buffer canary died!";                                \
    } while (0)
#endif

TEST_F(TestDatalogger, TriggerBasics)
{
    float my_var = 0.0;
    float logged_var = 0.0;

    datalogging::Configuration dlconfig;
    dlconfig.items_count = 1;
    dlconfig.items_to_log[0].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[0].data.memory.size = sizeof(logged_var);
    dlconfig.items_to_log[0].data.memory.address = &logged_var;
    dlconfig.decimation = 1;
    dlconfig.timeout_100ns = 0;
    dlconfig.probe_location = 128;
    dlconfig.trigger.hold_time_100ns = 0;
    dlconfig.trigger.operand_count = 2;
    dlconfig.trigger.condition = datalogging::SupportedTriggerConditions::Equal;

    dlconfig.trigger.operands[0].type = datalogging::OperandType::Var;
    dlconfig.trigger.operands[0].data.var.addr = &my_var;
    dlconfig.trigger.operands[0].data.var.datatype = scrutiny::VariableType::float32;

    dlconfig.trigger.operands[1].type = datalogging::OperandType::Literal;
    dlconfig.trigger.operands[1].data.literal.val = 3.1415926f;

    datalogger.config()->copy_from(&dlconfig);
    datalogger.configure(&tb);

    EXPECT_FALSE(datalogger.check_trigger());
    my_var = 3.1415926f;
    EXPECT_FALSE(datalogger.check_trigger());

    datalogger.arm_trigger();
    my_var = 0;
    EXPECT_FALSE(datalogger.check_trigger());
    my_var = 3.1415926f;
    EXPECT_TRUE(datalogger.check_trigger());

    CHECK_CANARIES;
}

TEST_F(TestDatalogger, TriggerHoldTime)
{
    float my_var = 0.0;
    float logged_var = 0.0;

    datalogging::Configuration dlconfig;
    dlconfig.items_count = 1;
    dlconfig.items_to_log[0].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[0].data.memory.size = sizeof(logged_var);
    dlconfig.items_to_log[0].data.memory.address = &logged_var;

    dlconfig.decimation = 1;
    dlconfig.timeout_100ns = 0;
    dlconfig.probe_location = 128;
    dlconfig.trigger.hold_time_100ns = 100;
    dlconfig.trigger.operand_count = 2;
    dlconfig.trigger.condition = datalogging::SupportedTriggerConditions::Equal;

    dlconfig.trigger.operands[0].type = datalogging::OperandType::Var;
    dlconfig.trigger.operands[0].data.var.addr = &my_var;
    dlconfig.trigger.operands[0].data.var.datatype = scrutiny::VariableType::float32;

    dlconfig.trigger.operands[1].type = datalogging::OperandType::Literal;
    dlconfig.trigger.operands[1].data.literal.val = 3.1415926f;

    datalogger.config()->copy_from(&dlconfig);
    datalogger.configure(&tb);
    datalogger.arm_trigger();

    EXPECT_FALSE(datalogger.check_trigger());
    my_var = 3.1415926f;
    EXPECT_FALSE(datalogger.check_trigger());
    tb.step(99);
    EXPECT_FALSE(datalogger.check_trigger());
    tb.step(1);
    EXPECT_TRUE(datalogger.check_trigger());

    CHECK_CANARIES;
}

TEST_F(TestDatalogger, BasicAcquisition)
{
    float my_var = 0.0;

    datalogging::Configuration dlconfig;
    dlconfig.items_count = 1;
    dlconfig.items_to_log[0].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[0].data.memory.size = sizeof(my_var);
    dlconfig.items_to_log[0].data.memory.address = &my_var;
    dlconfig.decimation = 2;
    dlconfig.timeout_100ns = 0;
    dlconfig.probe_location = 128;
    dlconfig.trigger.hold_time_100ns = 100;
    dlconfig.trigger.operand_count = 2;
    dlconfig.trigger.condition = datalogging::SupportedTriggerConditions::GreaterThan;

    dlconfig.trigger.operands[0].type = datalogging::OperandType::Var;
    dlconfig.trigger.operands[0].data.var.addr = &my_var;
    dlconfig.trigger.operands[0].data.var.datatype = scrutiny::VariableType::float32;

    dlconfig.trigger.operands[1].type = datalogging::OperandType::Literal;
    dlconfig.trigger.operands[1].data.literal.val = 100;

    datalogger.config()->copy_from(&dlconfig);
    datalogger.configure(&tb);

    datalogger.process();
    tb.step(100);
    datalogger.process();
    tb.step(100);
    EXPECT_FALSE(datalogger.data_acquired());

    my_var = 200.0f;

    for (unsigned int i = 0; i < 100; i++)
    {
        datalogger.process();
        tb.step(100);
        my_var += 1.0;
    }
    EXPECT_FALSE(datalogger.data_acquired());
    EXPECT_EQ(g_trigger_callback_count, 0u);
    datalogger.arm_trigger();

    for (unsigned int i = 0; i < 100; i++)
    {
        datalogger.process();
        tb.step(100);
        my_var += 1.0;
    }
    EXPECT_TRUE(datalogger.data_acquired());
    EXPECT_EQ(g_trigger_callback_count, 1u);

    CHECK_CANARIES;
}

TEST_F(TestDatalogger, ComplexAcquisition)
{
// Static to spare the stack a bit
#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW
    RawFormatParser parser;
#else
#error "Unsupported parser"
#endif

    float var1 = 0.0;
    int32_t var2 = 0;
    float trigger_val = 0.0f;

    FixedFrequencyLoopHandler loop_handler(100000, "testloop");
    datalogging::Configuration dlconfig;

    datalogger.set_owner(&loop_handler);

    dlconfig.items_count = 4;
    dlconfig.items_to_log[0].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[0].data.memory.size = sizeof(var1);
    dlconfig.items_to_log[0].data.memory.address = &var1;

    dlconfig.items_to_log[1].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[1].data.memory.size = sizeof(var2);
    dlconfig.items_to_log[1].data.memory.address = &var2;

    dlconfig.items_to_log[2].type = datalogging::LoggableType::Rpv;
    dlconfig.items_to_log[2].data.rpv.id = 0x1000;

    dlconfig.items_to_log[3].type = datalogging::LoggableType::Time;

    dlconfig.decimation = 2;

    dlconfig.trigger.hold_time_100ns = 50;
    dlconfig.trigger.operand_count = 2;
    dlconfig.trigger.condition = datalogging::SupportedTriggerConditions::Equal;

    dlconfig.trigger.operands[0].type = datalogging::OperandType::Var;
    dlconfig.trigger.operands[0].data.var.addr = &trigger_val;
    dlconfig.trigger.operands[0].data.var.datatype = scrutiny::VariableType::float32;

    dlconfig.trigger.operands[1].type = datalogging::OperandType::Literal;
    dlconfig.trigger.operands[1].data.literal.val = 100.0f;

    uint_least8_t probe_location = 0;
    for (uint16_t probe_loop = 0; probe_loop <= 255; probe_loop++)
    {
        var1 = 0.0;
        var2 = 0;
        trigger_val = 0.0f;
        probe_location = static_cast<uint_least8_t>(probe_loop & 0xFF);
        dlconfig.probe_location = probe_location;
#if SCRUTINYTEST_NO_OUTPUT
        char const *error_msg = "";
#else
        std::string error_msg = "probe_location=" + NumberToString(probe_location);
#endif
        datalogger.config()->copy_from(&dlconfig);
        datalogger.configure(&tb);
        if (probe_loop == 0)
        {
            EXPECT_NULL(g_last_rpv_callback_caller);
        }
        datalogger.process();
        tb.step(10);
        datalogger.process();
        tb.step(10);
        EXPECT_FALSE(datalogger.data_acquired()) << error_msg;

        for (unsigned int i = 0; i < 100; i++)
        {
            datalogger.process();
            tb.step(10);
        }
        EXPECT_EQ(g_last_rpv_callback_caller, &loop_handler);
        EXPECT_FALSE(datalogger.data_acquired()) << error_msg;

        datalogger.arm_trigger();

        float var1_at_trigger = 0;
        int32_t var2_at_trigger = 0;
        uint32_t rpv1000_at_trigger = 0;

        for (unsigned int i = 0; i < 100; i++)
        {
            datalogger.process();
            tb.step(10);
            var1 += 1.0f;
            var2--;
            g_u32_rpv1000 += 1;
        }

        trigger_val = 100.0f;

        for (unsigned int i = 0; i < 500; i++)
        {
            datalogger.process();
            if (i == 6) // 50us delay
            {
                var1_at_trigger = var1;
                var2_at_trigger = var2;
                rpv1000_at_trigger = g_u32_rpv1000;
            }
            tb.step(10);
            var1 += 1.0f;
            var2--;
            g_u32_rpv1000 += 1;
        }

        EXPECT_TRUE(datalogger.data_acquired()) << error_msg;
        ASSERT_FALSE(datalogger.get_encoder()->error()) << error_msg;
        CHECK_CANARIES;

        datalogging::DataReader *reader = datalogger.get_reader();
        reader->reset();

        uint32_t copied_count = 0;
        while (!reader->finished())
        {
            ASSERT_FALSE(reader->error()) << error_msg;
            copied_count += reader->read_dilate_8bits(&output_buffer.data[copied_count], 10);
            ASSERT_LE(copied_count, sizeof(output_buffer.data)) << error_msg;
        }

        CHECK_CANARIES;

        EXPECT_GE(copied_count, 9 * sizeof(dlbuffer.data) / 10) << error_msg; // 90% usage at least

        parser.init(&scrutiny_handler, &dlconfig, output_buffer.data, sizeof(output_buffer.data));
        parser.parse(reader->get_entry_count());

        ASSERT_FALSE(datalogger.in_error()) << error_msg;
        ASSERT_FALSE(parser.error()) << error_msg;

        // Validate data now;

        vector<unsigned char> *data = parser.get();
        uint32_t const entry_count = reader->get_entry_count();

        unsigned char *entry[4];
        unsigned char *last_entry[4];

        for (size_t i = 0; i < entry_count; i++)
        {
            if (i > 0)
            {
                entry[0] = parser.get_parsed_data_location(i, 0);
                entry[1] = parser.get_parsed_data_location(i, 1);
                entry[2] = parser.get_parsed_data_location(i, 2);
                entry[3] = parser.get_parsed_data_location(i, 3);

                last_entry[0] = parser.get_parsed_data_location(i - 1, 0);
                last_entry[1] = parser.get_parsed_data_location(i - 1, 1);
                last_entry[2] = parser.get_parsed_data_location(i - 1, 2);
                last_entry[3] = parser.get_parsed_data_location(i - 1, 3);

                // Make sure we do not misuse the vector
                ASSERT_NE(static_cast<void *>(entry[0]), static_cast<void *>(SCRUTINY_NULL));
                ASSERT_NE(static_cast<void *>(entry[1]), static_cast<void *>(SCRUTINY_NULL));
                ASSERT_NE(static_cast<void *>(entry[2]), static_cast<void *>(SCRUTINY_NULL));
                ASSERT_NE(static_cast<void *>(entry[3]), static_cast<void *>(SCRUTINY_NULL));
                ASSERT_NE(static_cast<void *>(last_entry[0]), static_cast<void *>(SCRUTINY_NULL));
                ASSERT_NE(static_cast<void *>(last_entry[1]), static_cast<void *>(SCRUTINY_NULL));
                ASSERT_NE(static_cast<void *>(last_entry[2]), static_cast<void *>(SCRUTINY_NULL));
                ASSERT_NE(static_cast<void *>(last_entry[3]), static_cast<void *>(SCRUTINY_NULL));

                float check_var1 = *reinterpret_cast<float *>(entry[0]);
                float check_last_var1 = *reinterpret_cast<float *>(last_entry[0]);

                int32_t check_var2 = *reinterpret_cast<int32_t *>(entry[1]);
                int32_t check_last_var2 = *reinterpret_cast<int32_t *>(last_entry[1]);

                uint32_t check_rpv1000 = codecs::decode_32_bits_big_endian_char(entry[2]);
                uint32_t check_last_rpv1000 = codecs::decode_32_bits_big_endian_char(last_entry[2]);

                uint32_t check_time = codecs::decode_32_bits_big_endian_char(entry[3]);
                uint32_t check_last_time = codecs::decode_32_bits_big_endian_char(last_entry[3]);

                EXPECT_EQ(check_var1 - check_last_var1, 2.0f) << "i=" << i << "," << error_msg;
                EXPECT_EQ(check_var2 - check_last_var2, -2) << "i=" << i << "," << error_msg;
                EXPECT_EQ(check_rpv1000 - check_last_rpv1000, 2u) << "i=" << i << "," << error_msg;
                EXPECT_EQ(check_time - check_last_time, 20u) << "i=" << i << "," << error_msg; // 2*10us
            }
        }

        uint32_t trigger_location = static_cast<uint32_t>(round(static_cast<float>(probe_location) / 255 * (reader->get_entry_count() - 1)));
        ASSERT_GT(data->size(), trigger_location * parser.get_entry_size_char());
        ASSERT_GT(data->size(), 0);
        ASSERT_GE(data->size(), datalogger.log_points_after_trigger() * parser.get_entry_size_char());

        EXPECT_NEAR(trigger_location, static_cast<uint32_t>(entry_count - datalogger.log_points_after_trigger()), 1u);

        float mid_var1 = *reinterpret_cast<float *>(parser.get_parsed_data_location(trigger_location, 0));
        int32_t mid_var2 = *reinterpret_cast<int32_t *>(parser.get_parsed_data_location(trigger_location, 1));
        uint32_t mid_rpv1000 = codecs::decode_32_bits_big_endian_char(parser.get_parsed_data_location(trigger_location, 2));

        // Validate position of trigger and end of graph. Allow a margin of 3 (1.5 entry because decimation=2)
        EXPECT_LE(std::abs(var1_at_trigger - mid_var1), 3.0f) << error_msg;
        EXPECT_LE(std::abs(var2_at_trigger - mid_var2), 3) << error_msg;
        EXPECT_LE(std::abs(static_cast<int32_t>(rpv1000_at_trigger - mid_rpv1000)), 3) << error_msg;
    }
}

TEST_F(TestDatalogger, TestAlwaysUseFullBuffer)
{
    float var1 = 0.0;
    int32_t var2 = 0;

    datalogging::Configuration dlconfig;
    dlconfig.items_count = 4;
    dlconfig.items_to_log[0].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[0].data.memory.size = sizeof(var1);
    dlconfig.items_to_log[0].data.memory.address = &var1;
    dlconfig.items_to_log[1].type = datalogging::LoggableType::Memory;
    dlconfig.items_to_log[1].data.memory.size = sizeof(var2);
    dlconfig.items_to_log[1].data.memory.address = &var2;
    dlconfig.items_to_log[2].type = datalogging::LoggableType::Rpv;
    dlconfig.items_to_log[2].data.rpv.id = 0x1000;
    dlconfig.items_to_log[3].type = datalogging::LoggableType::Time;

    dlconfig.decimation = 1;
    dlconfig.trigger.hold_time_100ns = 0;
    dlconfig.timeout_100ns = 0;
    dlconfig.trigger.operand_count = 0;
    dlconfig.trigger.condition = datalogging::SupportedTriggerConditions::AlwaysTrue;

    unsigned char probe_location = 0;
    for (uint16_t probe_loop = 0; probe_loop <= 255; probe_loop++)
    {
        var1 = 0.0;
        var2 = 0;
        probe_location = static_cast<unsigned char>(probe_loop);
        dlconfig.probe_location = probe_location;
#if SCRUTINYTEST_NO_OUTPUT
        char const *error_msg = "";
#else
        std::string error_msg = "probe_location=" + NumberToString(probe_location);
#endif
        datalogger.reset();
        datalogger.config()->copy_from(&dlconfig);
        datalogger.configure(&tb);
        datalogger.arm_trigger();

        for (unsigned int i = 0; i < sizeof(dlbuffer.data) / 4; i++)
        {
            datalogger.process();
            tb.step(10);
            if (datalogger.data_acquired())
            {
                break;
            }
        }
        ASSERT_TRUE(datalogger.data_acquired()) << error_msg;
        ASSERT_FALSE(datalogger.get_encoder()->error()) << error_msg;
        CHECK_CANARIES;

        datalogging::DataReader *reader = datalogger.get_reader();
        reader->reset();

        EXPECT_GE(reader->get_total_size(), 9 * sizeof(dlbuffer.data) / 10) << error_msg; // 90% usage at least
    }
}

TEST_F(TestDatalogger, TestAquireTimeCorrectly)
{
#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW
    RawFormatParser parser;
#else
#error "Unsupported parser"
#endif
    datalogging::Configuration dlconfig;
    dlconfig.items_count = 1;
    dlconfig.items_to_log[0].type = datalogging::LoggableType::Time;

    dlconfig.decimation = 1;
    dlconfig.timeout_100ns = 0;
    dlconfig.probe_location = 128;
    dlconfig.trigger.hold_time_100ns = 0;
    dlconfig.trigger.operand_count = 0;
    dlconfig.trigger.condition = datalogging::SupportedTriggerConditions::AlwaysTrue;

    datalogger.config()->copy_from(&dlconfig);
    datalogger.configure(&tb);
    uint32_t max_loop = sizeof(dlbuffer.data) * CHAR_BIT; // don't think we can beat 1 bit per sample
    uint32_t i = 0;
    while (!datalogger.get_encoder()->buffer_full() && i < max_loop)
    {
        datalogger.process();
        tb.step(5);
        i++;
    }
    datalogger.arm_trigger();
    datalogger.force_trigger();
    i = 0;
    while (datalogger.get_state() != scrutiny::datalogging::DataLogger::State::AcquisitionCompleted && i < max_loop)
    {
        datalogger.process();
        tb.step(5);
        i++;
    }
    datalogger.process();
    ASSERT_EQ(datalogger.get_state(), scrutiny::datalogging::DataLogger::State::AcquisitionCompleted);

    datalogger.get_reader()->reset();
    datalogger.get_reader()->read_dilate_8bits(output_buffer.data, sizeof(output_buffer.data));
    parser.init(&scrutiny_handler, &dlconfig, output_buffer.data, sizeof(output_buffer.data));
    uint16_t entry_count = datalogger.get_reader()->get_entry_count();
    parser.parse(entry_count);
    ASSERT_FALSE(parser.error());
    SCRUTINY_STATIC_ASSERT(sizeof(timestamp_t) == sizeof(uint32_t), "Expect timestamp to be 32 bits");

    uint16_t item0_size = parser.get_item_size_char(0);
    uint16_t entry_size = parser.get_entry_size_char();
    ASSERT_EQ(item0_size, entry_size);
    for (size_t i = 0; i < entry_count; i++)
    {
        scrutiny::timestamp_t timestamp = codecs::decode_32_bits_big_endian_char(parser.get_parsed_data_location(i, 0));
        if (i > 0)
        {
            scrutiny::timestamp_t last_timestamp = codecs::decode_32_bits_big_endian_char(parser.get_parsed_data_location(i - 1, 0));
            ASSERT_EQ(timestamp - last_timestamp, 5) << "Entry #" << i;
        }
    }

    CHECK_CANARIES;
}
