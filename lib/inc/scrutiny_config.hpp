//    scrutiny_config.hpp
//        Definition of a run-time configuration of the scrutiny module.
//        To be filled in startup phase
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_CONFIG_H___
#define ___SCRUTINY_CONFIG_H___

#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"
#include "scrutiny_loop_handler.hpp"

namespace scrutiny
{
    class MainHandler;

    class Config
    {
    public:
        friend class MainHandler;

        Config();

        /// @brief Clear the configuration content
        void clear();

        /// @brief Set the buffers used for communications. The bigger the buffer, the more data can be processed by a single request.
        /// @param rx_buffer Reception buffer
        /// @param rx_buffer_size Reception buffer size
        /// @param tx_buffer Transmission buffer
        /// @param tx_buffer_size Transmission buffer size
        void set_buffers(uint8_t *rx_buffer, uint16_t const rx_buffer_size, uint8_t *tx_buffer, uint16_t const tx_buffer_size);

        /// @brief Define some memory section that are to be left untouched
        /// @param range Array of ranges represented by the `AddressRange` object.
        /// This array must be allocated outside of Scrutiny and stay allocated forever as no copy will be made
        /// Consider using `scrutiny::tools::make_address_range` to generate these objects in a one-liner
        /// @param count Number of ranges in the given array
        void set_forbidden_address_range(AddressRange const *range, uint8_t const count);

        /// @brief Defines some memory sections that are read-only.
        /// @param ranges Array of ranges represented by the `scrutiny::AddressRange` object.
        /// This array must be allocated outside of Scrutiny and stay allocated forever as no copy will be made
        /// Consider using `scrutiny::tools::make_address_range` to generate these objects in a one-liner
        /// @param count Number of ranges in the given array
        void set_readonly_address_range(AddressRange const *ranges, uint8_t const count);

        /// @brief Configures the Runtime Published Values
        /// @param array Array of `scrutiny::RuntimePublishedValues` that contains the definition of each RPV.
        /// This array must be allocated outside of Scrutiny and stay
        /// allocated forever as no copy will be made
        /// @param nbr Number of RPV in the array
        /// @param rd_cb Callback to call to read a RPV
        /// @param wr_cb Callback to call to write a RPV
        void set_published_values(RuntimePublishedValue const *array, uint16_t const nbr, RpvReadCallback const rd_cb = nullptr, RpvWriteCallback const wr_cb = nullptr);

        /// @brief Defines the different loops (tasks) in the application.
        /// @param loops Arrays of pointer to the `scrutiny::LoopHandlers`.
        /// This array must be allocated outside of Scrutiny and stay
        /// allocated forever as no copy will be made
        /// @param loop_count Number of `scrutiny::LoopHandlers`
        void set_loops(LoopHandler **loops, uint8_t loop_count);

        /// @brief Sets a callback to be called by Scrutiny after a request to the UserCommand function.
        /// This generic feature allows the integrator to pass down some custom request/data to the application by leveraging the already
        /// existing Scrutiny protocol.
        /// @param callback The callback
        inline void set_user_command_callback(user_command_callback_t callback)
        {
            m_user_command_callback = callback;
        };

        /// @brief Returns the actual user command callback. nullptr if unset
        /// @return The callback
        inline user_command_callback_t get_user_command_callback(void)
        {
            return m_user_command_callback;
        };

#if SCRUTINY_ENABLE_DATALOGGING

        /// @brief Sets the buffer used to store data when doing a datalogging acquisition
        /// @param buffer The datalogging buffer
        /// @param buffer_size The datalogging buffer size
        void set_datalogging_buffers(uint8_t *buffer, datalogging::buffer_size_t const buffer_size);

        /// @brief Sets a callback to be called by Scrutiny when a datalogging trigger condition is triggered. This callback will be called from the
        /// context of the LoopHandler using the datalogger with no thread safety. This means that if data are to be passed to another task, it is
        /// the integrator responsibility to ensure thread safety
        /// @param callback The callback to be call upon datalogging trigger
        inline void set_datalogging_trigger_callback(datalogging::trigger_callback_t callback)
        {
            m_datalogger_trigger_callback = callback;
        };
#endif
        /// @brief Returns true if a callback has been set to support the UserCallback service call
        inline bool is_user_command_callback_set(void) const
        {
            return m_user_command_callback != nullptr;
        }

        /// @brief Returns true if the communication buffers were sets
        inline bool is_buffer_set(void) const { return (m_rx_buffer != nullptr) && (m_tx_buffer != nullptr); }

        /// @brief Returns true if forbidden regions have been defined
        inline bool is_forbidden_address_range_set(void) const { return m_forbidden_address_ranges != nullptr; }

        /// @brief Returns true if read-only regions have been defined
        inline bool is_readonly_address_range_set(void) const { return m_readonly_address_ranges != nullptr; }

        /// @brief Returns true if Runtime Published Values (RPV) were defined and a Read callback has been given
        inline bool is_read_published_values_configured(void) const { return (m_rpv_read_callback != nullptr && m_rpvs != nullptr && m_rpv_count > 0); };

        /// @brief Returns true if Runtime Published Values (RPV) were defined and a Write callback has been given
        inline bool is_write_published_values_configured(void) const { return (m_rpv_write_callback != nullptr && m_rpvs != nullptr && m_rpv_count > 0); };

        /// @brief Returns true if a list of loops (tasks) were defined
        inline bool is_loop_handlers_configured(void) const { return m_loops != nullptr && m_loop_count > 0; }
#if SCRUTINY_ENABLE_DATALOGGING

        /// @brief Returns true if the datalogging feature has been configured to a working point.
        inline bool is_datalogging_configured(void) const
        {
            return (m_datalogger_buffer != nullptr && m_datalogger_buffer_size != 0);
        };

        /// @brief Returns true if at least one loop support datalogging
        bool has_at_least_one_loop_with_datalogging(void) const;
#endif

        /// @brief Returns the list of forbidden address ranges. A forbidden range is never read or written by Scrutiny.
        inline AddressRange const *forbidden_ranges(void) const
        {
            return m_forbidden_address_ranges;
        }

        /// @brief Returns the number of forbidden ranges configured
        inline uint8_t forbidden_ranges_count(void) const { return m_forbidden_range_count; }

        /// @brief Returns the list of read-only address ranges. A read-only range is never written by Scrutiny.
        inline AddressRange const *readonly_ranges(void) const { return m_readonly_address_ranges; }

        /// @brief Returns the number of read-only ranges configured
        inline uint8_t readonly_ranges_count(void) const { return m_readonly_range_count; }

        /// @brief Returns the pointer to the array of Runtime Published Values (RPV)
        inline RuntimePublishedValue const *get_rpvs_array(void) const { return m_rpvs; }

        /// @brief  Return the number of Runtime Published Values (RPV) configured
        inline uint16_t get_rpv_count(void) const { return m_rpv_count; }

        /// @brief Return the Runtime Published Value (RPV) read callback
        inline RpvReadCallback get_rpv_read_callback(void) const { return m_rpv_read_callback; }

        /// @brief Return the Runtime Published Value (RPV) write callback
        inline RpvWriteCallback get_rpv_write_callback(void) const { return m_rpv_write_callback; }

        /// @brief Maximum bitrate in bit/sec. This value is given to the server and enforced by the server only.
        uint32_t max_bitrate;

        /// @brief A seed to initialize the session counter to avoid having collision in case multiple scrutiny enabled devices uses the same communication channel
        uint32_t session_counter_seed;

        /// @brief The display name to be broadcasted during discovery phase. This value will be shown to the user.
        char const *display_name;

        /// @brief When true, memory write are enabled. When false, no memory write is permitted.
        bool memory_write_enable;

    private:
        uint8_t *m_rx_buffer;                           // The comm Rx buffer
        uint16_t m_rx_buffer_size;                      // The comm Rx buffer size
        uint8_t *m_tx_buffer;                           // The comm Tx buffer
        uint16_t m_tx_buffer_size;                      // The comm Tx buffer size
        AddressRange const *m_forbidden_address_ranges; // The forbidden address range array pointer. nullptr if unset
        uint8_t m_forbidden_range_count;                // The forbidden address range count
        AddressRange const *m_readonly_address_ranges;  // The read-only address range array pointer. nullptr if unset
        uint8_t m_readonly_range_count;                 // The read-only address range count
        RuntimePublishedValue const *m_rpvs;            // The array of Runtime Published Values. nullptr if unset
        uint16_t m_rpv_count;                           // The number of Runtime Published Values in the RPV array
        RpvReadCallback m_rpv_read_callback;            // The callback to perform read operation on a Runtime Published Value (RPV)
        RpvWriteCallback m_rpv_write_callback;          // The callback to perform write operation on a Runtime Published Value (RPV)
        LoopHandler **m_loops;                          // The array of Loop Handler pointers
        uint8_t m_loop_count;                           // Number of Loop Handler in the array

        /// @brief Callback to be called on a User Command request.
        user_command_callback_t m_user_command_callback; // Callback to call when a User Command service call is requested by the server

#if SCRUTINY_ENABLE_DATALOGGING
        uint8_t *m_datalogger_buffer;                                  // Buffer that stores the datalogging data
        datalogging::buffer_size_t m_datalogger_buffer_size;           // size of the datalogging buffer
        datalogging::trigger_callback_t m_datalogger_trigger_callback; // Callback to call upon datalogging acquisition triggers
#endif
    };
}

#endif // ___SCRUTINY_CONFIG_H___