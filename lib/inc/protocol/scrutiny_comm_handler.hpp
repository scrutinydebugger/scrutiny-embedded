//    scrutiny_comm_handler.hpp
//        Manage the tranport-ish layer of the communication protocol. Converts Request/Response
//        to byte stream and validate CRC
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_COMM_HANDLER_H___
#define ___SCRUTINY_COMM_HANDLER_H___

#include <stdint.h>

#include "scrutiny_setup.hpp"
#include "scrutiny_timebase.hpp"
#include "scrutiny_protocol.hpp"
#include "scrutiny_protocol_definitions.hpp"
#include "scrutiny_tools.hpp"

namespace scrutiny
{
    namespace protocol
    {
        class CommHandler
        {
        public:
            void init(uint8_t *rx_buffer, const uint16_t rx_buffer_size, uint8_t *tx_buffer, const uint16_t tx_buffer_size, Timebase *timebase, const uint32_t session_counter_seed = 0);
            void receive_data(uint8_t *data, uint16_t len);
            bool send_response(Response *response);
            void reset();
            Response *prepare_response(void);

            uint16_t pop_data(uint8_t *buffer, uint16_t len);
            uint16_t data_to_send(void) const;

            bool check_crc(const Request *req) const;
            void add_crc(Response *response) const;

            bool heartbeat(const uint16_t challenge);
            void process(void);
            bool connect(void);
            void disconnect(void);

            inline void wait_next_request(void) { reset_rx(); }

            inline bool request_received(void) { return m_request_received; }
            inline Request *get_request(void) { return &m_active_request; }
            inline RxError get_rx_error(void) const { return m_rx_error; }
            inline TxError get_tx_error(void) const { return m_tx_error; }
            inline bool transmitting(void) const { return (m_state == State::Transmitting); }
            inline bool receiving(void) const { return (m_state == State::Receiving); }

            inline bool is_enabled(void) const { return m_enabled; }
            inline void enable(void) { m_enabled = true; }
            inline void disable(void)
            {
                reset();
                m_enabled = false;
            }

            inline bool is_connected(void) const { return m_session_active; }
            inline uint32_t get_session_id(void) const { return m_session_id; }
            inline uint16_t rx_buffer_size(void) const { return m_rx_buffer_size; }
            inline uint16_t tx_buffer_size(void) const { return m_tx_buffer_size; }

        protected:
            void process_active_request(void);
            bool received_discover_request(void);
            bool received_connect_request(void);

            enum class RxFSMState
            {
                WaitForCommand,
                WaitForSubfunction,
                WaitForLength,
                WaitForData,
                WaitForCRC,
                WaitForProcess,
                Error
            };

            enum class State
            {
                Idle,
                Receiving,
                Transmitting,
            };

            void reset_rx();
            void reset_tx();

            Timebase *m_timebase;
            State m_state;
            bool m_enabled;
            uint32_t m_session_id;
            bool m_session_active;
            uint32_t m_heartbeat_timestamp;
            uint16_t m_last_heartbeat_challenge;
            bool m_heartbeat_received;
            bool m_reception_paused;

            // Reception
            uint8_t *m_rx_buffer;
            uint16_t m_rx_buffer_size;
            uint8_t *m_tx_buffer;
            uint16_t m_tx_buffer_size;
            Request m_active_request;
            RxFSMState m_rx_state;
            RxError m_rx_error;
            bool m_request_received;
            uint8_t m_crc_bytes_received;
            uint8_t m_length_bytes_received;
            uint16_t m_data_bytes_received;
            uint32_t m_last_rx_timestamp;

            // Transmission
            Response m_active_response;
            uint16_t m_nbytes_to_send;
            uint16_t m_nbytes_sent;
            TxError m_tx_error;

        private:
            static uint32_t s_session_counter;
        };
    }
}

#endif //___SCRUTINY_COMM_HANDLER_H___