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

    /// @brief C equivalent of the C++ `scrutiny::Config`
    typedef void scrutiny_c_config_t;
    /// @brief C equivalent of the C++ `scrutiny::MainHandler`
    typedef void scrutiny_c_main_handler_t;
    /// @brief C equivalent of the C++ `scrutiny::LoopHandler`
    typedef void scrutiny_c_loop_handler_t;
    /// @brief C equivalent of the C++ `scrutiny::FixedFrequencyLoopHandler`
    typedef void scrutiny_c_loop_handler_ff_t;
    /// @brief C equivalent of the C++ `scrutiny::VariableFrequencyLoopHandler`
    typedef void scrutiny_c_loop_handler_vf_t;

    /// @brief Amount of memory required to construct a `scrutiny::MainHandler`. Contains `sizeof(scrutiny::MainHandler)`
    extern size_t const SCRUTINY_C_MAIN_HANDLER_SIZE;
    /// @brief Amount of memory required to construct a `scrutiny::MainHandler`. Contains `sizeof(scrutiny::Config)`
    extern size_t const SCRUTINY_C_CONFIG_SIZE;
    /// @brief Amount of memory required to construct a `scrutiny::MainHandler`. Contains `sizeof(FixedFrequencyLoopHandler)`
    extern size_t const SCRUTINY_C_LOOP_HANDLER_FF_SIZE;
    /// @brief Amount of memory required to construct a `scrutiny::MainHandler`. Contains `sizeof(VariableFrequencyLoopHandler)`
    extern size_t const SCRUTINY_C_LOOP_HANDLER_VF_SIZE;

    // ==== MainHandler ====

    /// @brief Wrapper for `MainHandler::MainHandler()`.
    /// Construct `scrutiny::MainHandler` object at a given address. Fails if `mem` is NULL or if the size is not big enough.
    /// @param mem Address at which to initialize the `MainHandler`
    /// @param size Size of the allocated memory.
    /// @return A pointer to the `MainHandler` object or NULL in case of failure.
    scrutiny_c_main_handler_t *scrutiny_c_main_handler_construct(void *mem, size_t size);

    /// @brief Wrapper for `MainHandler::init()`.
    /// Initialize The MainHandler
    /// @param main_handler The `MainHandler` object to work on.
    /// @param config The configuration to apply on the `MainHandler`.
    void scrutiny_c_main_handler_init(scrutiny_c_main_handler_t *main_handler, scrutiny_c_config_t *config);

    /// @brief Wrapper for `MainHandler::process()`
    /// @param main_handler The `MainHandler` object to work on.
    /// @param timestep Amount of time elapsed since the last call to this function expressed as multiple of 100ns.
    void scrutiny_c_main_handler_process(scrutiny_c_main_handler_t *main_handler, scrutiny_c_timediff_t timestep);

    /// @brief Wrapper for `MainHandler::receive_data()`.
    /// Pass data received from the server to the scrutiny-embedded lib input stream.
    /// @param main_handler The `MainHandler` object to work on.
    /// @param data Pointer to the data buffer
    /// @param len Length of the data
    void scrutiny_c_main_handler_receive_data(scrutiny_c_main_handler_t *main_handler, uint8_t const *data, uint16_t const len);

    /// @brief Wrapper for `MainHandler::pop_data()`.
    /// Reads data from the scrutiny-embedded lib output stream so it can be sent to the server
    /// @param main_handler The `MainHandler` object to work on.
    /// @param buffer Buffer top write the data into
    /// @param len Maximum length of the data to read
    /// @return Number of bytes actually read
    uint16_t scrutiny_c_main_handler_pop_data(scrutiny_c_main_handler_t *main_handler, uint8_t *buffer, uint16_t len);

    /// @brief Wrapper for `MainHandler::data_to_send()`.
    /// Tells how much data is available in the scrutiny-embedded lib output stream
    /// @param main_handler The `MainHandler` object to work on.
    /// @return Number of bytes available
    uint16_t scrutiny_c_main_handler_data_to_send(scrutiny_c_main_handler_t *main_handler);

    // ==== Config ====

    /// @brief Wrapper for `Config::Config()`.
    /// Construct `scrutiny::Config` object at a given address. Fails if `mem` is NULL or if the size is not big enough.
    /// @param mem Address at which to initialize the `Config`
    /// @param size Size of the allocated memory.
    /// @return A pointer to the `Config` object or NULL in case of failure.
    scrutiny_c_config_t *scrutiny_c_config_construct(void *mem, size_t size);

    /// @brief Wrapper for `Config::set_buffers()`
    /// Set the buffers used for communications. The bigger the buffer, the more data can be processed by a single request.
    /// @param config The `scrutiny::Config` to work on
    /// @param rx_buffer Reception buffer
    /// @param rx_buffer_size Reception buffer size
    /// @param tx_buffer Transmission buffer
    /// @param tx_buffer_size Transmission buffer size
    void scrutiny_c_config_set_buffers(
        scrutiny_c_config_t *config,
        uint8_t *rx_buffer,
        uint16_t const rx_buffer_size,
        uint8_t *tx_buffer,
        uint16_t const tx_buffer_size);

    /// @brief Wrapper for `Config::set_forbidden_address_range`
    /// Defines some memory section that are to be left untouched
    /// @param config The `scrutiny::Config` object to work on
    /// @param ranges Array of ranges represented by the `AddressRange` object.
    /// This array must be allocated outside of Scrutiny and stay allocated forever as no copy will be made
    /// Consider using `scrutiny::tools::make_address_range` to generate these objects in a one-liner
    /// @param count Number of ranges in the given array
    void scrutiny_c_config_set_forbidden_address_range(scrutiny_c_config_t *config, scrutiny_c_address_range_t const *ranges, uint8_t const count);

    /// @brief Wrapper for `Config::set_readonly_address_range()
    /// Defines some memory sections that are read-only`
    /// @param config The `scrutiny::Config` object to work on
    /// @param ranges Array of ranges represented by the `scrutiny::AddressRange` object.
    /// This array must be allocated outside of Scrutiny and stay allocated forever as no copy will be made
    /// Consider using `scrutiny::tools::make_address_range()` to generate these objects in a one-liner
    /// @param count Number of ranges in the given array
    void scrutiny_c_config_set_readonly_address_range(scrutiny_c_config_t *config, scrutiny_c_address_range_t const *ranges, uint8_t const count);

    /// @brief Wrapper for `scrutiny::set_published_values()`
    /// Configures the Runtime Published Values
    /// @param config The `scrutiny::Config` object to work on
    /// @param array Array of `scrutiny::RuntimePublishedValues` that contains the definition of each RPV.
    /// This array must be allocated outside of Scrutiny and stay
    /// allocated forever as no copy will be made
    /// @param nbr Number of RPV in the array
    /// @param rd_cb Callback to call to read a RPV
    /// @param wr_cb Callback to call to write a RPV
    void scrutiny_c_config_set_published_values(
        scrutiny_c_config_t *config,
        scrutiny_c_runtime_published_value_t *array,
        uint16_t nbr,
        scrutiny_c_rpv_read_callback_t rd_cb,
        scrutiny_c_rpv_write_callback_t wr_cb);

    /// @brief Wrapper for `Config::set_loops`
    /// Defines the different loops (tasks) in the application.
    /// @param config The `scrutiny::Config` object to work on
    /// @param loops Arrays of pointer to the `scrutiny::LoopHandlers`.
    /// This array must be allocated outside of Scrutiny and stay
    /// allocated forever as no copy will be made
    /// @param loop_count Number of `scrutiny::LoopHandlers`
    void scrutiny_c_config_set_loops(scrutiny_c_config_t *config, scrutiny_c_loop_handler_t **loops, uint8_t const loop_count);

    /// @brief Wrapper for `Config::set_user_command_callback`
    /// Sets a callback to be called by Scrutiny after a request to the UserCommand function.
    /// This generic feature allows the integrator to pass down some custom request/data to the application by leveraging the already
    /// existing Scrutiny protocol.
    /// @param config The `scrutiny::Config` object to work on
    /// @param callback The callback
    void scrutiny_c_config_set_user_command_callback(scrutiny_c_config_t *config, scrutiny_c_user_command_callback_t callback);

#if SCRUTINY_ENABLE_DATALOGGING == 1
    /// @brief Wrapper for `Config::set_datalogging_buffers()`
    /// Sets the buffer used to store data when doing a datalogging acquisition
    /// @param config The `scrutiny::Config` object to work on
    /// @param buffer The datalogging buffer
    /// @param buffer_size The datalogging buffer size
    void scrutiny_c_config_set_datalogging_buffers(scrutiny_c_config_t *config, uint8_t *buffer, scrutiny_c_datalogging_buffer_size_t buffer_size);

    /// @brief Wrapper for `Config::set_datalogging_trigger_callback`
    /// Sets a callback to be called by Scrutiny when a datalogging trigger condition is triggered. This callback will be called from the
    /// context of the LoopHandler using the datalogger with no thread safety. This means that if data are to be passed to another task, it is
    /// the integrator responsibility to ensure thread safety
    /// @param config The `scrutiny::Config` object to work on
    /// @param callback The callback to be call upon datalogging trigger
    void scrutiny_c_config_set_datalogging_trigger_callback(scrutiny_c_config_t *config, scrutiny_c_datalogging_trigger_callback_t callback);
#endif
    /// @brief Setter for `Config::max_bitrate`
    /// @param config The `scrutiny::Config` object to work on
    /// @param bitrate The bitrate value
    void scrutiny_c_config_set_max_bitrate(scrutiny_c_config_t *config, uint32_t const bitrate);

    /// @brief Setter for `Config::session_counter_seed`
    /// @param config The `scrutiny::Config` object to work on
    /// @param bitrate The seed value
    void scrutiny_c_config_set_session_counter_seed(scrutiny_c_config_t *config, uint32_t const seed);

    /// @brief Setter for `Config::display_name`
    /// @param config The `scrutiny::Config` object to work on
    /// @param bitrate The name
    void scrutiny_c_config_set_display_name(scrutiny_c_config_t *config, char const *name);

    /// @brief Setter for `Config::memory_write_enable`
    /// @param config The `scrutiny::Config` object to work on
    /// @param val Enable value
    void scrutiny_c_config_memory_write_enable(scrutiny_c_config_t *config, int val);

    // ==== LoopHandlers ====

    /// @brief Wrapper for `FixedFrequencyLoopHandler::FixedFrequencyLoopHandler()`.
    /// Construct `scrutiny::FixedFrequencyLoopHandler` object at a given address. Fails if `mem` is NULL or if the size is not big enough.
    /// @param mem Address at which to initialize the `FixedFrequencyLoopHandler`
    /// @param size Size of the allocated memory.
    /// @param timestep_100ns Time delta between each call to process() in multiple of 100ns
    /// @param name The name of the loop
    /// @return A pointer to the `FixedFrequencyLoopHandler` object or NULL in case of failure.
    scrutiny_c_loop_handler_ff_t *scrutiny_c_loop_handler_fixed_freq_construct(void *mem, size_t const size, uint32_t const timestep_100ns, char const *name);

    /// @brief Wrapper for `FixedFrequencyLoopHandler::process()`
    /// Process function be called at each iteration of the loop.
    /// @param loop_handler The `FixedFrequencyLoopHandler` object to work on
    void scrutiny_c_loop_handler_fixed_freq_process(scrutiny_c_loop_handler_ff_t *loop_handler);

    /// @brief Wrapper for `VariableFrequencyLoopHandler::VariableFrequencyLoopHandler()`.
    /// Construct `scrutiny::VariableFrequencyLoopHandler` object at a given address. Fails if `mem` is NULL or if the size is not big enough.
    /// @param mem Address at which to initialize the `VariableFrequencyLoopHandler`
    /// @param size Size of the allocated memory.
    /// @param name The name of the loop
    /// @return A pointer to the `VariableFrequencyLoopHandler` object or NULL in case of failure.
    scrutiny_c_loop_handler_vf_t *scrutiny_c_loop_handler_variable_freq_construct(void *mem, size_t const size, const char *name);

    /// @brief Wrapper for `VariableFrequencyLoopHandler::process()`
    /// Process function be called at each iteration of the loop.
    /// @param loop_handler The `VariableFrequencyLoopHandler` object to work on
    /// @param timestep_100ns Time delta since last call to process() in multiple of 100ns
    void scrutiny_c_loop_handler_variable_freq_process(scrutiny_c_loop_handler_vf_t *loop_handler, scrutiny_c_timediff_t timestep_100ns);

#ifdef __cplusplus
}
#endif

#endif // SCRUTINY_C_WRAPPER_H
