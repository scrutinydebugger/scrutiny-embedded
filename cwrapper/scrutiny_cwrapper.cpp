#include "scrutiny.hpp"
#include "scrutiny_cwrapper.h"
#include <new>
#include <cstddef>

static_assert(sizeof(scrutiny_c_runtime_published_value_t) == sizeof(scrutiny::RuntimePublishedValue), "C/C++ RPV type mismatch");
static_assert(offsetof(scrutiny_c_runtime_published_value_t, id) == offsetof(scrutiny::RuntimePublishedValue, id), "C/C++ RPV type mismatch");
static_assert(offsetof(scrutiny_c_runtime_published_value_t, type) == offsetof(scrutiny::RuntimePublishedValue, type), "C/C++ RPV type mismatch");

static inline scrutiny::Config *get_config(scrutiny_c_config_t *config)
{
    return reinterpret_cast<scrutiny::Config *>(config);
}

static inline scrutiny::MainHandler *get_main_handler(scrutiny_c_main_handler_t *mh)
{
    return reinterpret_cast<scrutiny::MainHandler *>(mh);
}

static inline scrutiny::FixedFrequencyLoopHandler *get_loop_handler_ff(scrutiny_c_loop_handler_ff_t *lh)
{
    return reinterpret_cast<scrutiny::FixedFrequencyLoopHandler *>(lh);
}

static inline scrutiny::VariableFrequencyLoopHandler *get_loop_handler_vf(scrutiny_c_loop_handler_vf_t *lh)
{
    return reinterpret_cast<scrutiny::VariableFrequencyLoopHandler *>(lh);
}

extern "C"
{
    size_t const SCRUTINY_C_MAIN_HANDLER_SIZE = sizeof(scrutiny::MainHandler);
    size_t const SCRUTINY_C_CONFIG_SIZE = sizeof(scrutiny::Config);
    size_t const SCRUTINY_C_LOOP_HANDLER_FF_SIZE = sizeof(scrutiny::FixedFrequencyLoopHandler);
    size_t const SCRUTINY_C_LOOP_HANDLER_VF_SIZE = sizeof(scrutiny::VariableFrequencyLoopHandler);

    scrutiny_c_main_handler_t *scrutiny_c_main_handler_construct(void *mem, size_t size)
    {
        if (size < SCRUTINY_C_MAIN_HANDLER_SIZE || mem == nullptr)
        {
            return nullptr;
        }
        return reinterpret_cast<scrutiny_c_main_handler_t *>(new (mem) scrutiny::MainHandler); // Placement new
    }

    void scrutiny_c_main_handler_init(scrutiny_c_main_handler_t *mh, scrutiny_c_config_t *config)
    {
        get_main_handler(mh)->init(get_config(config));
    }

    void scrutiny_c_main_handler_process(scrutiny_c_main_handler_t *mh, scrutiny_c_timediff_t timestep)
    {
        get_main_handler(mh)->process(timestep);
    }

    scrutiny_c_config_t *scrutiny_c_config_construct(void *mem, size_t bufsize)
    {
        if (bufsize < SCRUTINY_C_CONFIG_SIZE || mem == nullptr)
        {
            return nullptr;
        }
        return reinterpret_cast<scrutiny_c_config_t *>(new (mem) scrutiny::Config); // Placement new
    }

    void scrutiny_c_config_set_buffers(
        scrutiny_c_config_t *config,
        uint8_t *rx_buffer,
        uint16_t const rx_buffer_size,
        uint8_t *tx_buffer,
        uint16_t const tx_buffer_size)
    {
        get_config(config)->set_buffers(rx_buffer, rx_buffer_size, tx_buffer, tx_buffer_size);
    }

    void scrutiny_c_config_set_forbidden_address_range(scrutiny_c_config_t *config, scrutiny_c_address_range_t const *ranges, uint8_t const count)
    {
        get_config(config)->set_forbidden_address_range(reinterpret_cast<scrutiny::AddressRange const *>(ranges), count);
    }

    void scrutiny_c_config_set_readonly_address_range(scrutiny_c_config_t *config, scrutiny_c_address_range_t const *ranges, uint8_t const count)
    {
        get_config(config)->set_readonly_address_range(reinterpret_cast<scrutiny::AddressRange const *>(ranges), count);
    }

    void scrutiny_c_config_set_published_values(
        scrutiny_c_config_t *config,
        scrutiny_c_runtime_published_value_t *array,
        uint16_t nbr,
        scrutiny_c_rpv_read_callback_t rd_cb,
        scrutiny_c_rpv_write_callback_t wr_cb)
    {
        get_config(config)->set_published_values(
            reinterpret_cast<scrutiny::RuntimePublishedValue *>(array), // should match as per static_assert above
            nbr,
            reinterpret_cast<scrutiny::RpvReadCallback>(rd_cb),   // Expect signature to match
            reinterpret_cast<scrutiny::RpvWriteCallback>(wr_cb)); // Expect signature to match
    }

    void scrutiny_c_config_set_loops(scrutiny_c_config_t *config, scrutiny_c_loop_handler_t **loops, uint8_t const loop_count)
    {
        get_config(config)->set_loops(reinterpret_cast<scrutiny::LoopHandler **>(loops), loop_count);
    }

    void scrutiny_c_config_set_user_command_callback(scrutiny_c_config_t *config, scrutiny_c_user_command_callback_t callback)
    {
        get_config(config)->set_user_command_callback(reinterpret_cast<scrutiny::user_command_callback_t>(callback));
    }

#if SCRUTINY_ENABLE_DATALOGGING == 1
    void scrutiny_c_config_set_datalogging_buffers(scrutiny_c_config_t *config, uint8_t *buffer, scrutiny_c_datalogging_buffer_size_t size)
    {
        get_config(config)->set_datalogging_buffers(buffer, size);
    }

    void scrutiny_c_config_set_datalogging_trigger_callback(scrutiny_c_config_t *config, scrutiny_c_datalogging_trigger_callback_t callback)
    {
        get_config(config)->set_datalogging_trigger_callback(reinterpret_cast<scrutiny::datalogging::trigger_callback_t>(callback));
    }
#endif

    void scrutiny_c_config_set_max_bitrate(scrutiny_c_config_t *config, uint32_t const bitrate)
    {
        get_config(config)->max_bitrate = bitrate;
    }

    void scrutiny_c_config_set_session_counter_seed(scrutiny_c_config_t *config, uint32_t const seed)
    {
        get_config(config)->session_counter_seed = seed;
    }

    void scrutiny_c_config_set_display_name(scrutiny_c_config_t *config, char const *name)
    {
        get_config(config)->display_name = name;
    }

    void scrutiny_c_config_memory_write_enable(scrutiny_c_config_t *config, int const val)
    {
        get_config(config)->memory_write_enable = static_cast<bool>(val);
    }

    void scrutiny_c_main_handler_receive_data(scrutiny_c_main_handler_t *mh, uint8_t const *data, uint16_t const len)
    {
        get_main_handler(mh)->receive_data(data, len);
    }

    uint16_t scrutiny_c_main_handler_pop_data(scrutiny_c_main_handler_t *mh, uint8_t *buffer, uint16_t const len)
    {
        return get_main_handler(mh)->pop_data(buffer, len);
    }

    uint16_t scrutiny_c_main_handler_data_to_send(scrutiny_c_main_handler_t *mh)
    {
        return get_main_handler(mh)->data_to_send();
    }

    scrutiny_c_loop_handler_ff_t *scrutiny_c_loop_handler_fixed_freq_construct(void *mem, size_t const size, uint32_t const timestep_100ns, char const *name)
    {
        if (size < SCRUTINY_C_LOOP_HANDLER_FF_SIZE || mem == nullptr)
        {
            return nullptr;
        }
        return reinterpret_cast<scrutiny_c_loop_handler_ff_t *>(new (mem) scrutiny::FixedFrequencyLoopHandler(timestep_100ns, name)); // Placement new
    }

    void scrutiny_c_loop_handler_fixed_freq_process(scrutiny_c_loop_handler_ff_t *loop_handler)
    {
        get_loop_handler_ff(loop_handler)->process();
    }

    scrutiny_c_loop_handler_vf_t *scrutiny_c_loop_handler_variable_freq_construct(void *mem, size_t const size, const char *name)
    {
        if (size < SCRUTINY_C_LOOP_HANDLER_VF_SIZE || mem == nullptr)
        {
            return nullptr;
        }
        return reinterpret_cast<scrutiny_c_loop_handler_vf_t *>(new (mem) scrutiny::VariableFrequencyLoopHandler(name)); // Placement new
    }

    void scrutiny_c_loop_handler_variable_freq_process(scrutiny_c_loop_handler_vf_t *loop_handler, scrutiny_c_timediff_t timestep_100ns)
    {
        get_loop_handler_vf(loop_handler)->process(timestep_100ns);
    }
}
