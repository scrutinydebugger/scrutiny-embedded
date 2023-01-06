//    scrutiny_datalogger.hpp
//        The definition of the datalogger object that should exist in each loop
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGER_H___
#define ___SCRUTINY_DATALOGGER_H___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#include "datalogging/scrutiny_datalogging_types.hpp"
#include "datalogging/scrutiny_datalogging_trigger.hpp"
#include "datalogging/scrutiny_datalogging_data_encoding.hpp"
#include "scrutiny_timebase.hpp"

namespace scrutiny
{
    class MainHandler;

    namespace datalogging
    {

        class DataLogger
        {
        public:
            /// @brief The internal state of the datalogger
            enum class State
            {
                IDLE,
                CONFIGURED,
                ARMED,
                ACQUISITION_COMPLETED,
                ERROR
            };

            /// @brief Initializes the datalogger
            /// @param main_handler A pointer to the main handler to be used to access memory and RPVs
            /// @param timebase The timebase used to keep track of time
            /// @param buffer The logging buffer
            /// @param buffer_size Size of the logging buffer
            /// @param trigger_callback A function pointer to call when the datalogging trigger condition trigs. Executed in the owner loop (no thread safety)
            void init(MainHandler *main_handler, Timebase *timebase, uint8_t *const buffer, const uint32_t buffer_sizem, trigger_callback_t trigger_callback = nullptr);

            /// @brief Configure the datalogger with a configuration received by the server
            /// @param timebase_for_log The timebase used for time logging
            /// @param config_id A configuration ID that will be attached to the acquisition for validation.
            void configure(Timebase *timebase_for_log, uint16_t config_id = 0);

            /// @brief Periodic process. To be called as fast as possible
            void process(void);

            /// @brief Put back the datalogger to its startup state.
            void reset(void);

            /// @brief Tells if data has been acquired and ready to be read
            /// @return True if data is acquired
            inline bool data_acquired(void) const { return m_state == State::ACQUISITION_COMPLETED; }

            /// @brief  Returns the acquisition ID of the last acquisition
            inline uint16_t get_acquisition_id(void) const { return m_acquisition_id; }

            /// @brief Returns the configuration ID attached with the acquisition
            inline uint16_t get_config_id(void) const { return m_config_id; }

            /// @brief Tells if the datalogger is armed and waiting for a trigger
            inline bool armed(void) const { return m_state == State::ARMED; }

            /// @brief Returns the Datalogger state
            inline DataLogger::State get_state(void) const { return m_state; }

            /// @brief Arm the trigger so that the datalogger actively check for trigger condition to start acquisition
            void arm_trigger(void);

            /// @brief Disarm the trigger, meaning it will stop looking for the trigger condition
            void disarm_trigger(void);

            /// @brief Check if the trigger condition is met.
            /// @return True if the condition is met
            bool check_trigger(void);

            /// @brief Returns a DataReader object that will iterate through each samples
            inline DataReader *get_reader(void) { return m_encoder.get_reader(); }

            /// @brief Returns the internal DataEncoder object used to write the samples in the datalogging buffer
            inline DataEncoder *get_encoder(void) { return &m_encoder; }

            /// @brief Returns a pointer to the internal configuration object
            inline Configuration *config(void) { return &m_config; }

            /// @brief Returns true if the datalogger is in error state
            inline bool in_error(void) const { return m_state == State::ERROR; }

            /// @brief Retruns true if the active configuration is valid. Must be called after a call to "configure"
            inline bool config_valid(void) const { return m_config_valid; }

            /// @brief Returns the number of point after the trigger, indicating the exact position of the trigger point in a acquisition
            inline uint32_t log_points_after_trigger(void) const { return m_log_points_after_trigger; }

            /// @brief Forces the trigger condition to be fulfilled, triggering an acquisition if the datalogger is armed.
            void force_trigger(void)
            {
                if (m_state == State::ARMED)
                {
                    m_manual_trigger = true;
                }
            }

        protected:
            void process_acquisition(void);
            void stamp_trigger_point(void);
            bool acquisition_completed(void);
            void write_uncompressed_entry(void);
            uint16_t read_next_entry_size(uint32_t *cursor);
            void write_diff_bits(uint8_t *new_entry, uint8_t *previous_entry);

            MainHandler *m_main_handler;           // A pointer to the main handler
            uint32_t m_buffer_size;                // The datalogging buffer size
            trigger_callback_t m_trigger_callback; // A function pointer to be called when the trigger trigs. Executed in the owner loop (no thread safety)

            Timebase *m_timebase;               // Pointer to the timebase for internal time tracking
            Timebase *m_timebase_for_log;       // Pointer to timebase for time logging (can be in a different time domain)
            State m_state;                      // Internal state
            bool m_trigger_point_stamped;       // True if the trigger point has been recorded
            timestamp_t m_trigger_timestamp;    // The timestamp at which the trigger happened
            uint32_t m_trigger_cursor_location; // Cursor location when trigger point has been recorded

            uint32_t m_remaining_data_to_write; // Amount of data that still need to be written before going to ACQUISITION_COMPLETE. Used to control probe location
            bool m_manual_trigger;              // Indicates if a manual trigger have been requested

            Configuration m_config;              // The datalogger configuration object
            bool m_config_valid;                 // Flag indicating whether the configuration is valid or not. Set after a call to `configure`
            DataEncoder m_encoder;               // The data encoder that reads the data and lay it into the datalogging buffer
            uint16_t m_decimation_counter;       // counter used for effective decimation
            uint16_t m_acquisition_id;           // The acquisition ID of the last acquired acquisition
            uint16_t m_config_id;                // The configuration ID given by the server
            uint32_t m_log_points_after_trigger; // Number of log entry counted after the trigger condition was fulfilled.

            struct
            {
                bool previous_val;                        // Trigger condition result of the previous cycle
                timestamp_t rising_edge_timestamp;        // Timestamp at which the condition passed from false to true
                trigger::ConditionSet conditions;         // All the conditons object in a union
                trigger::BaseCondition *active_condition; // A pointer to the active condition object.
            } m_trigger;                                  // Data related to the graph trigger
        };
    }
}
#endif // ___SCRUTINY_DATALOGGER_H___