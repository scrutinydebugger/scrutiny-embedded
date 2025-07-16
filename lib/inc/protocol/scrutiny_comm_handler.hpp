//    scrutiny_comm_handler.hpp
//        Manage the tranport-ish layer of the communication protocol. Converts Request/Response
//        to byte stream and validate CRC
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_COMM_HANDLER_H___
#define ___SCRUTINY_COMM_HANDLER_H___

#include <stdint.h>

#include "scrutiny_protocol.hpp"
#include "scrutiny_protocol_definitions.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_timebase.hpp"
#include "scrutiny_tools.hpp"

namespace scrutiny
{
    namespace protocol
    {
        /// @brief Class that handles the communication with the server
        /// Communication is half-duplex and works by polling with a request/response scheme.
        class CommHandler
        {
          public:
            /// @brief Initialize the CommHandler
            /// @param rx_buffer     Buffer for reception
            /// @param rx_buffer_size Reception buffer size
            /// @param tx_buffer Buffer for transmission
            /// @param tx_buffer_size Transmission buffer size
            /// @param timebase Pointer to a timebase object to keep track of time
            /// @param session_counter_seed Seed to initialize the session ID counter to avoid collision if multiple scrutiny enabled device are
            /// connected to the same channel
            void init(
                uint8_t *const rx_buffer,
                uint16_t const rx_buffer_size,
                uint8_t *const tx_buffer,
                uint16_t const tx_buffer_size,
                Timebase const *const timebase,
                uint32_t const session_counter_seed = 0);

            /// @brief Move data from the outside world (received by the server) to the scrutiny lib
            /// @param data Buffer containing the received data
            /// @param len Number of bytes to read
            void receive_data(uint8_t const *const data, uint16_t const len);

            /// @brief Send a response to the server
            /// @param response The response object
            /// @return true on success, false on failure
            bool send_response(Response const *const response);

            /// @brief Reset the communication handler and put it back to an idle state
            void reset(void);

            /// @brief Get a response object that will use the transmit buffer has a payload content
            Response *prepare_response(void);

            // Reads data from the scrutiny lib so that it can be sent to the outside world (to the server)
            uint16_t pop_data(uint8_t *const buffer, uint16_t len);

            /// @brief Returns the number of bytes pending to be sent.
            uint16_t data_to_send(void) const;

            /// @brief Tells if the Request object has a CRC that matches its payload content.
            /// @param req The request to check
            /// @return True if the CRC is valid. False otherwise
            static bool check_crc(Request const *const req);

            // Writes the CRC property of the response based on the payload content.
            void add_crc(Response *const response) const;

            /// @brief Trigger a heartbeat, resetting the watchdog timeout
            /// @param challenge A challenge value to be used to compute a response. This value must change at each call otherwise the heartbeat will
            /// be ignored
            /// @return True if the heartbeat was effective. False if it was ignored
            bool heartbeat(uint16_t const challenge);

            /// @brief Periodic process function to call
            void process(void);

            /// @brief Creates a session and put the CommHandler in a active state where it will listen for requests.
            bool connect(void);

            /// @brief Destroy the currently active session. Will cause the CommHandler to ignore all requests, except Discover and Connect requests.
            void disconnect(void);

            /// @brief Put the CommHandler in a state where the next request can be received.
            inline void wait_next_request(void) { reset_rx(); }

            /// @brief Returns true if a request has been received. Will stay true until call to wait_next_request()
            inline bool request_received(void) const { return m_request_received; }

            /// @brief Returns the request that has been received
            inline Request const *get_request(void) const { return &m_active_request; }

            /// @brief Gets the last error encountered in reception task
            inline RxError::eRxError get_rx_error(void) const { return m_rx_error; }

            /// @brief Gets the last error encountered in transmission task
            inline TxError::eTxError get_tx_error(void) const { return m_tx_error; }

            /// @brief Returns true if the CommHandler is presently transmitting. False otherwise
            inline bool transmitting(void) const { return (m_state == State::Transmitting); }

            /// @brief Returns true if the CommHandler is presently receiving. False otherwise
            inline bool receiving(void) const { return (m_state == State::Receiving); }

            /// @brief Returns true if the CommHandler is presently. Might be disabled if the configuration is invalid
            inline bool is_enabled(void) const { return m_enabled; }

            /// @brief Enables the CommHandler
            inline void enable(void) { m_enabled = true; }

            /// @brief Disables the CommHandler
            inline void disable(void)
            {
                reset();
                m_enabled = false;
            }

            /// @brief Return true if a session is active with a server
            inline bool is_connected(void) const { return m_session_active; }

            /// @brief Returns the session ID given to the server upon connection
            inline uint32_t get_session_id(void) const { return m_session_id; }

            /// @brief Returns the size of the reception buffer
            inline uint16_t rx_buffer_size(void) const { return m_rx_buffer_size; }

            /// @brief Returns the size of the transmission buffer
            inline uint16_t tx_buffer_size(void) const { return m_tx_buffer_size; }

          protected:
            void process_active_request(void);
            bool received_discover_request(void) const;
            bool received_connect_request(void) const;

            class RxFSMState
            {
              public:
                // clang-format off
                SCRUTINY_ENUM(eRxFSMState, uint_least8_t)
                {
                    WaitForCommand,
                    WaitForSubfunction,
                    WaitForLength,
                    WaitForData,
                    WaitForCRC,
                    WaitForProcess,
                    Error
                };
                // clang-format on
            };

            class State
            {
              public:
                // clang-format off
                SCRUTINY_ENUM(eState, uint_least8_t)
                {
                    Idle,
                    Receiving,
                    Transmitting
                };
                // clang-format on
            };

            void reset_rx();
            void reset_tx();

            Timebase const *m_timebase;          // Pointer to the timebase given by the MainHandler
            State::eState m_state;               // Internal state, idle, receiving, transmitting
            bool m_enabled;                      // Enable flag
            uint32_t m_session_id;               // Actual session ID
            bool m_session_active;               // Flag indicating if a session is active with the server
            timestamp_t m_heartbeat_timestamp;   // Timestamp of the last heartbeat gotten
            uint16_t m_last_heartbeat_challenge; // Challenge received by the last heartbeat
            bool m_first_heartbeat_received;     // Flag indicating if the first heartbeat has been received.

            // Reception
            uint8_t *m_rx_buffer;               // The reception buffer
            uint16_t m_rx_buffer_size;          // The reception buffer size
            uint8_t *m_tx_buffer;               // The transmission buffer
            uint16_t m_tx_buffer_size;          // The transmission buffer size
            Request m_active_request;           // The request presently being received
            RxFSMState::eRxFSMState m_rx_state; // Reception Finite State Machine state
            RxError::eRxError m_rx_error;       // Last reception error code
            bool m_request_received;            // Flag indicating if a full request has been received
            union
            {
                uint8_t crc_bytes_received;    // Number of bytes part of the CRC received up to now (from 0 to 4)
                uint8_t length_bytes_received; // Number of bytes part of the length received up to now (from 0 to 2)
                uint16_t data_bytes_received;  // Number of bytes part of the data payload received up to now
            } m_per_state_data;
            timestamp_t m_last_rx_timestamp; // Timestamp at which the last chunk of data was received

            // Transmission
            Response m_active_response;   // The response being transmitted
            uint16_t m_nbytes_to_send;    // Number of bytes to send in this response
            uint16_t m_nbytes_sent;       // Number of bytes sent up to now. Includes headers and CRC
            TxError::eTxError m_tx_error; // Last Transmission error code

          private:
            static uint32_t s_session_counter; // A counter to generate session ID
        };
    } // namespace protocol
} // namespace scrutiny

#endif //___SCRUTINY_COMM_HANDLER_H___
