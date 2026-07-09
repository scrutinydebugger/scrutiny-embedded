//    scrutiny_datalogger.hpp
//        The definition of the datalogger object that should exist in each loop
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGER_H___
#define ___SCRUTINY_DATALOGGER_H___

#include "datalogging/scrutiny_datalogging_data_encoding.hpp"
#include "datalogging/scrutiny_datalogging_trigger.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"
#include "scrutiny_compiler.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_timebase.hpp"
#include <stdint.h>

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    class MainHandler;
    class LoopHandler;

    namespace datalogging
    {

        class DataLogger
        {
          public:
            /// @brief The internal state of the datalogger

            class State
            {
              public:
                // clang-format off
                SCRUTINY_ENUM(eState, uint_least8_t)
                {
                    Idle,
                    Configured,
                    Armed,
                    Triggered,
                    AcquisitionCompleted,
                    Error
                };
                // clang-format on
            };

            /// @brief Initializes the datalogger
            /// @param main_handler A pointer to the main handler to be used to access memory and RPVs
            /// @param buffer The logging buffer
            /// @param buffer_size Size of the logging buffer
            /// @param trigger_callback A function pointer to call when the datalogging trigger condition trigs. Executed in the owner loop (no thread
            /// safety)
            Status::eStatus init(
                MainHandler const *const main_handler,
                unsigned char *const buffer,
                buffer_size_t const buffer_size,
                trigger_callback_t trigger_callback = SCRUTINY_NULL_FN_PTR(trigger_callback_t));

            /// @brief Configure the datalogger with a configuration received by the server
            /// @param timebase The timebase used for time logging & trigger management
            /// @param config_id A configuration ID that will be attached to the acquisition for validation.
            void configure(Timebase *timebase, uint16_t config_id = 0);

            /// @brief Periodic process. To be called as fast as possible
            void process(void);

            /// @brief Put back the datalogger to its startup state.
            void reset(void);

            /// @brief Tells if data has been acquired and ready to be read
            /// @return True if data is acquired
            inline bool data_acquired(void) const { return m_state == State::AcquisitionCompleted; }

            /// @brief  Returns the acquisition ID of the last acquisition
            inline uint16_t get_acquisition_id(void) const { return m_acquisition_id; }

            /// @brief Returns the configuration ID attached with the acquisition
            inline uint16_t get_config_id(void) const { return m_config_id; }

            /// @brief Tells if the datalogger is armed and waiting for a trigger
            inline bool armed(void) const { return m_state == State::Armed; }

            /// @brief Returns the Datalogger state
            inline DataLogger::State::eState get_state(void) const { return m_state; }

            /// @brief Arm the trigger so that the datalogger actively check for trigger condition to start acquisition
            void arm_trigger(void);

            /// @brief Disarm the trigger, meaning it will stop looking for the trigger condition
            void disarm_trigger(void);

            /// @brief Check if the trigger is fulfilled. Trigger condition must be true for the given hold time.
            /// @return True if the condition is met
            bool check_trigger(void);

            /// @brief Returns a DataReader object that will iterate through each samples
            inline DataReader *get_reader(void) { return m_encoder.get_reader(); }

            /// @brief Returns the internal DataEncoder object used to write the samples in the datalogging buffer
            inline DataEncoder *get_encoder(void) { return &m_encoder; }

            /// @brief Returns a pointer to the internal configuration object
            inline Configuration *config(void) { return &m_config; }

            /// @brief Returns true if the datalogger is in error state
            inline bool in_error(void) const { return m_state == State::Error; }

            /// @brief Returns true if the active configuration is valid. Must be called after a call to "configure"
            inline bool config_valid(void) const { return m_config_valid; }

            /// @brief Returns the number of points after the trigger, indicating the exact position of the trigger point in an acquisition
            inline buffer_size_t log_points_after_trigger(void) const { return m_log_points_after_trigger; }

            /// @brief Returns the number of bytes that needs to be acquired since trigger so that the acquisition is considered complete
            inline buffer_size_t get_bytes_to_acquire_from_trigger_to_completion(void) const
            {
                return (m_state == State::Triggered) ? m_remaining_data_to_write : 0;
            }

            /// @brief Returns the number of bytes acquired since the trigger event.
            inline buffer_size_t data_counter_since_trigger(void) const
            {
                // This counter gets reset when trigger happens.
                return (m_state == State::Triggered) ? m_encoder.get_data_write_counter() : 0;
            }

            /// @brief Return the LoopHandler that owns the datalogger. Null if owned by the MainHandler. This value is updated by the owner himself.
            inline LoopHandler *get_owner(void) const { return m_owner; }
            /// @brief Sets the LoopHandler that owns the datalogger. Null if owned by the MainHandler. This value is updated by the owner himself.
            inline void set_owner(LoopHandler *const owner) { m_owner = owner; }

            /// @brief Forces the trigger condition to be fulfilled, triggering an acquisition if the datalogger is armed.
            void force_trigger(void)
            {
                if (m_state == State::Armed)
                {
                    m_manual_trigger = true;
                }
            }

          protected:
            void process_acquisition(void);
            void stamp_trigger_point(void);
            bool acquisition_completed(void);
            void write_uncompressed_entry(void);
            uint16_t read_next_entry_size(buffer_size_t *cursor);

            MainHandler const *m_main_handler; // A pointer to the main handler
            buffer_size_t m_buffer_size;       // The datalogging buffer size
            trigger_callback_t
                m_trigger_callback; // A function pointer to be called when the trigger trigs. Executed in the owner loop (no thread safety)

            Timebase const *m_timebase; // Pointer to the timebase of the owning loop. Used for logging at trigger handling (hold time & timeouts)
            State::eState m_state;      // Internal state
            timestamp_t m_trigger_timestamp;         // The timestamp at which the trigger happened
            buffer_size_t m_trigger_cursor_location; // Cursor location when trigger point has been recorded

            // Amount of data that still need to be written before going to ACQUISITION_COMPLETE. Used to control probe location
            buffer_size_t m_remaining_data_to_write;
            bool m_manual_trigger; // Indicates if a manual trigger have been requested

            Configuration m_config;                   // The datalogger configuration object
            bool m_config_valid;                      // Flag indicating whether the configuration is valid or not. Set after a call to `configure`
            DataEncoder m_encoder;                    // The data encoder that reads the data and lay it into the datalogging buffer
            uint16_t m_decimation_counter;            // counter used for effective decimation
            uint16_t m_acquisition_id;                // The acquisition ID of the last acquired acquisition
            uint16_t m_config_id;                     // The configuration ID given by the server
            buffer_size_t m_log_points_after_trigger; // Number of log entry counted after the trigger condition was fulfilled.

            LoopHandler *m_owner; // A pointer to the loop owning the datalogger. Should reflect MainHandler::m_datalogging.owner.
            struct
            {
                bool previous_val;                           // Trigger condition result of the previous cycle
                timestamp_t rising_edge_timestamp;           // Timestamp at which the condition passed from false to true
                trigger::ActiveCondition active_condition;   // The active condition object.
                trigger::ConditionSharedData condition_data; // Persistent data across trigger evaluation
            } m_trigger;                                     // Data related to the graph trigger

            union
            {
                struct
                {
                    AnyType opvals[MAX_OPERANDS];
                    VariableType::eVariableType optypes[MAX_OPERANDS];
                } check_trigger;
            } m_stack_data; // Move some data out of the stack.
        };
    } // namespace datalogging
} // namespace scrutiny
#endif // ___SCRUTINY_DATALOGGER_H___
