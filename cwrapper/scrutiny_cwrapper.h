#ifndef SCRUTINY_C_WRAPPER_H
#define SCRUTINY_C_WRAPPER_H

#include "stddef.h"
#include "stdint.h"
#include "scrutiny_setup.hpp"
#include "scrutiny_c_compatible_types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void scrutiny_c_config_t;
    typedef void scrutiny_c_main_handler_t;
    typedef void scrutiny_c_loop_handler_t;
    typedef void scrutiny_c_loop_handler_ff_t;
    typedef void scrutiny_c_loop_handler_vf_t;

    extern size_t const SCRUTINY_C_MAIN_HANDLER_SIZE;
    extern size_t const SCRUTINY_C_CONFIG_SIZE;
    extern size_t const SCRUTINY_C_LOOP_HANDLER_FF_SIZE;
    extern size_t const SCRUTINY_C_LOOP_HANDLER_VF_SIZE;

    // MainHandler
    scrutiny_c_main_handler_t *scrutiny_c_main_handler_construct(void *buff, size_t bufsize);
    void scrutiny_c_main_handler_init(scrutiny_c_main_handler_t *main_handler, scrutiny_c_config_t *config);
    void scrutiny_c_main_handler_process(scrutiny_c_main_handler_t *main_handler, scrutiny_c_timediff_t timestep);
    // Comm
    void scrutiny_c_main_handler_receive_data(scrutiny_c_main_handler_t *main_handler, uint8_t const *data, uint16_t const len);
    uint16_t scrutiny_c_main_handler_pop_data(scrutiny_c_main_handler_t *main_handler, uint8_t *buffer, uint16_t len);
    uint16_t scrutiny_c_main_handler_data_to_send(scrutiny_c_main_handler_t *main_handler);

    // Config
    scrutiny_c_config_t *scrutiny_c_config_construct(void *buff, size_t bufsize);
    void scrutiny_c_config_set_buffers(
        scrutiny_c_config_t *config,
        uint8_t *rx_buffer,
        uint16_t const rx_buffer_size,
        uint8_t *tx_buffer,
        uint16_t const tx_buffer_size);
    void scrutiny_c_config_set_forbidden_address_range(scrutiny_c_config_t *config, scrutiny_c_address_range_t const *ranges, uint8_t const count);
    void scrutiny_c_config_set_readonly_address_range(scrutiny_c_config_t *config, scrutiny_c_address_range_t const *ranges, uint8_t const count);
    void scrutiny_c_config_set_published_values(
        scrutiny_c_config_t *config,
        scrutiny_c_runtime_published_value_t *array,
        uint16_t nbr,
        scrutiny_c_rpv_read_callback_t rd_cb,
        scrutiny_c_rpv_write_callback_t wr_cb);
    void scrutiny_c_config_set_loops(scrutiny_c_config_t *config, scrutiny_c_loop_handler_t **loops, uint8_t const loop_count);
    void scrutiny_c_config_set_user_command_callback(scrutiny_c_config_t *config, scrutiny_c_user_command_callback_t callback);

#if SCRUTINY_ENABLE_DATALOGGING == 1
    void scrutiny_c_config_set_datalogging_buffers(scrutiny_c_config_t *config, uint8_t *buffer, scrutiny_c_datalogging_buffer_size_t size);
    void scrutiny_c_config_set_datalogging_trigger_callback(scrutiny_c_config_t *config, scrutiny_c_datalogging_trigger_callback_t callback);
#endif
    void scrutiny_c_config_set_max_bitrate(scrutiny_c_config_t *config, uint32_t const bitrate);
    void scrutiny_c_config_set_session_counter_seed(scrutiny_c_config_t *config, uint32_t const seed);
    void scrutiny_c_config_set_display_name(scrutiny_c_config_t *config, char const *name);
    void scrutiny_c_config_memory_write_enable(scrutiny_c_config_t *config, int val);

    // LoopHandlers
    scrutiny_c_loop_handler_ff_t *scrutiny_c_loop_handler_fixed_freq_construct(void *mem, size_t const size, uint32_t const timestep_100ns, char const *name);
    void scrutiny_c_loop_handler_fixed_freq_process(scrutiny_c_loop_handler_ff_t *loop_handler);

    scrutiny_c_loop_handler_vf_t *scrutiny_c_loop_handler_variable_freq_construct(void *mem, size_t const size, const char *name);
    void scrutiny_c_loop_handler_variable_freq_process(scrutiny_c_loop_handler_vf_t *loop_handler, scrutiny_c_timediff_t timestep_100ns);

#ifdef __cplusplus
}
#endif

#endif // SCRUTINY_C_WRAPPER_H