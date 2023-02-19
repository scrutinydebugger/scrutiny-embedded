//    scrutiny_comm_handler.cpp
//        Manage the tranport-ish layer of the communication protocol. Converts Request/Response
//        to byte stream and validate CRC
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#include <string.h>

#include "scrutiny_setup.hpp"
#include "protocol/scrutiny_comm_handler.hpp"
#include "scrutiny_tools.hpp"

namespace scrutiny
{
    namespace protocol
    {

        uint32_t CommHandler::s_session_counter = 0;

        void CommHandler::init(uint8_t *rx_buffer, const uint16_t rx_buffer_size, uint8_t *tx_buffer, const uint16_t tx_buffer_size, Timebase *timebase, const uint32_t session_counter_seed)
        {
            m_rx_buffer = rx_buffer;
            m_rx_buffer_size = rx_buffer_size;
            m_tx_buffer = tx_buffer;
            m_tx_buffer_size = tx_buffer_size;
            m_timebase = timebase;
            s_session_counter = session_counter_seed;
            m_active_request.data = m_rx_buffer; // Half duplex comm. Share buffer
            m_active_request.data_max_length = m_rx_buffer_size;
            m_active_response.data = m_tx_buffer; // Half duplex comm. Share buffer
            m_active_response.data_max_length = m_tx_buffer_size;
            m_enabled = true;

            if (m_rx_buffer_size < MINIMUM_RX_BUFFER_SIZE || m_rx_buffer_size > MAXIMUM_RX_BUFFER_SIZE)
            {
                m_enabled = false;
            }

            if (m_tx_buffer_size < MINIMUM_TX_BUFFER_SIZE || m_tx_buffer_size > MAXIMUM_TX_BUFFER_SIZE)
            {
                m_enabled = false;
            }

            reset();
        }

        void CommHandler::receive_data(uint8_t *data, uint16_t len)
        {
            uint16_t i = 0;

            if (m_enabled == false)
            {
                m_rx_error = RxError::Disabled;
                return;
            }

            if (m_state == State::Transmitting)
            {
                return; // Half duplex comm. Discard data;
            }

            // Handle rx timeouts. Start a new reception if no data for too long
            if (m_rx_state != RxFSMState::WaitForCommand && len != 0)
            {
                if (m_timebase->has_expired(m_last_rx_timestamp, SCRUTINY_COMM_RX_TIMEOUT_US * 10))
                {
                    reset_rx();
                    m_state = State::Idle;
                }
            }

            // Update rx timestamp
            if (len != 0)
            {
                m_last_rx_timestamp = m_timebase->get_timestamp();

                if (m_state == State::Idle)
                {
                    m_state = State::Receiving;
                }
            }

            // Process each bytes
            while (i < len && !m_request_received && m_rx_state != RxFSMState::Error)
            {

                switch (m_rx_state) // FSM
                {
                case RxFSMState::WaitForCommand:
                {
                    m_active_request.command_id = data[i] & 0x7F;
                    m_rx_state = RxFSMState::WaitForSubfunction;
                    i += 1;
                    break;
                }

                case RxFSMState::WaitForSubfunction:
                {
                    m_active_request.subfunction_id = data[i];
                    m_rx_state = RxFSMState::WaitForLength;
                    i += 1;
                    break;
                }

                case RxFSMState::WaitForLength:
                {
                    bool next_state = false;
                    if (m_length_bytes_received == 0)
                    {
                        if ((len - i) >= 2)
                        {
                            m_active_request.data_length = (static_cast<uint16_t>(data[i]) << 8u) | (static_cast<uint16_t>(data[i + 1]));
                            m_length_bytes_received = 2;
                            i += 2;
                            next_state = true;
                        }
                        else
                        {
                            m_active_request.data_length = static_cast<uint16_t>(data[i]) << 8u;
                            m_length_bytes_received = 1;
                            i += 1;
                        }
                    }
                    else
                    {
                        m_active_request.data_length |= static_cast<uint16_t>(data[i]);
                        m_length_bytes_received = 2;
                        i += 1;
                        next_state = true;
                    }

                    if (next_state)
                    {
                        if (m_active_request.data_length == 0)
                        {
                            m_rx_state = RxFSMState::WaitForCRC;
                        }
                        else
                        {
                            m_rx_state = RxFSMState::WaitForData;
                        }
                    }
                    break;
                }

                case RxFSMState::WaitForData:
                {
                    if (m_active_request.data_length > m_rx_buffer_size)
                    {
                        m_rx_error = RxError::Overflow;
                        m_rx_state = RxFSMState::Error;
                        break;
                    }

                    const uint16_t available_bytes = static_cast<uint16_t>(len - i);
                    const uint16_t missing_bytes = m_active_request.data_length - m_data_bytes_received;
                    const uint16_t data_bytes_to_read = (available_bytes >= missing_bytes) ? missing_bytes : available_bytes;

                    memcpy(&m_rx_buffer[m_data_bytes_received], &data[i], data_bytes_to_read);
                    m_data_bytes_received += data_bytes_to_read;
                    i += data_bytes_to_read;

                    if (m_data_bytes_received >= m_active_request.data_length)
                    {
                        m_rx_state = RxFSMState::WaitForCRC;
                    }

                    break;
                }

                case RxFSMState::WaitForCRC:
                {
                    if (m_crc_bytes_received == 0)
                    {
                        m_active_request.crc = static_cast<uint32_t>(data[i]) << 24u;
                    }
                    else if (m_crc_bytes_received == 1)
                    {
                        m_active_request.crc |= static_cast<uint32_t>(data[i]) << 16u;
                    }
                    else if (m_crc_bytes_received == 2)
                    {
                        m_active_request.crc |= static_cast<uint32_t>(data[i]) << 8u;
                    }
                    else if (m_crc_bytes_received == 3)
                    {
                        m_active_request.crc |= static_cast<uint32_t>(data[i]) << 0;
                        m_state = State::Idle;

                        if (check_crc(&m_active_request))
                        {
                            process_active_request();
                        }
                        else
                        {
                            reset_rx();
                        }
                    }

                    m_crc_bytes_received++;
                    i += 1;
                    break;
                }

                default:
                    break;
                }
            }
        }

        void CommHandler::process_active_request(void)
        {
            bool must_process = false;
            if (m_session_active == false)
            {
                if (received_discover_request() || received_connect_request()) // Check if we received a valid discover message
                {
                    must_process = true;
                }
            }
            else
            {
                must_process = true;
            }

            if (must_process)
            {
                m_rx_state = RxFSMState::WaitForProcess;
                m_request_received = true;
            }
            else
            {
                reset_rx();
            }
        }

        Response *CommHandler::prepare_response(void)
        {
            m_active_response.reset();
            return &m_active_response;
        }

        bool CommHandler::send_response(Response *response)
        {
            m_tx_error = TxError::None;
            if (m_enabled == false)
            {
                m_tx_error = TxError::Disabled;
                return false;
            }

            if (m_state != State::Idle)
            {
                m_tx_error = TxError::Busy;
                return false; // Half duplex comm. Discard data;
            }

            if (response->data_length > m_tx_buffer_size)
            {
                reset_tx();
                m_tx_error = TxError::Overflow;
                return false;
            }

            m_active_response.command_id = response->command_id | 0x80;
            m_active_response.subfunction_id = response->subfunction_id;
            m_active_response.response_code = response->response_code;
            m_active_response.data_length = response->data_length;
            m_active_response.data = response->data;

            add_crc(&m_active_response);

            // cmd8 + subfn8 + code8 + len16 + data + crc32
            m_nbytes_to_send = 1 + 1 + 1 + 2 + m_active_response.data_length + 4;

            m_state = State::Transmitting;
            return true;
        }

        uint16_t CommHandler::pop_data(uint8_t *buffer, uint16_t len)
        {
            static_assert(protocol::MAXIMUM_TX_BUFFER_SIZE <= 0xFFFF - 9, "Cannot parse successfully with 16bits counters");

            if (m_state != State::Transmitting)
            {
                return 0u;
            }

            uint16_t i = 0u;

            const uint16_t nbytes_to_send = static_cast<uint16_t>(m_nbytes_to_send - m_nbytes_sent);
            if (len > nbytes_to_send)
            {
                len = nbytes_to_send;
            }

            while (m_nbytes_sent < 5 && i < len)
            {
                if (m_nbytes_sent == 0u)
                {
                    buffer[i] = m_active_response.command_id;
                }
                else if (m_nbytes_sent == 1u)
                {
                    buffer[i] = m_active_response.subfunction_id;
                }
                else if (m_nbytes_sent == 2u)
                {
                    buffer[i] = m_active_response.response_code;
                }
                else if (m_nbytes_sent == 3u)
                {
                    buffer[i] = static_cast<uint8_t>((m_active_response.data_length >> 8) & 0xFFu);
                }
                else if (m_nbytes_sent == 4u)
                {
                    buffer[i] = static_cast<uint8_t>(m_active_response.data_length & 0xFFu);
                }

                i++;
                m_nbytes_sent++;
            }

            if (m_nbytes_sent >= 5u && i < len)
            {
                const uint16_t data_byte_sent = (m_nbytes_sent - 5);
                if (data_byte_sent < m_active_response.data_length)
                {
                    const uint16_t remaining_data_bytes = m_active_response.data_length - (m_nbytes_sent - 5);
                    const uint16_t user_request_remaining = (len - i);
                    const uint16_t data_bytes_to_copy = (user_request_remaining < remaining_data_bytes) ? user_request_remaining : remaining_data_bytes; // Don't read more than available.
                    memcpy(&buffer[i], &m_active_response.data[m_active_response.data_length - remaining_data_bytes], data_bytes_to_copy);

                    i += data_bytes_to_copy;
                    m_nbytes_sent += data_bytes_to_copy;
                }

                const uint16_t crc_position = m_active_response.data_length + 5u; // Will fit as per static_assert above.
                while (i < len)
                {
                    if (m_nbytes_sent == crc_position)
                    {
                        buffer[i] = (m_active_response.crc >> 24u) & 0xFFu;
                    }
                    else if (m_nbytes_sent == crc_position + 1u)
                    {
                        buffer[i] = (m_active_response.crc >> 16u) & 0xFFu;
                    }
                    else if (m_nbytes_sent == crc_position + 2u)
                    {
                        buffer[i] = (m_active_response.crc >> 8) & 0xFFu;
                    }
                    else if (m_nbytes_sent == crc_position + 3u)
                    {
                        buffer[i] = (m_active_response.crc >> 0u) & 0xFFu;
                    }
                    else
                    {
                        break; // Should never go here.
                    }
                    m_nbytes_sent++;
                    i++;
                }
            }

            if (m_nbytes_sent >= m_nbytes_to_send)
            {
                reset_tx();
            }

            return i;
        }

        // Check if the last request received is a valid "Comm Discover request".
        bool CommHandler::received_discover_request(void)
        {
            if (m_active_request.command_id != static_cast<uint8_t>(CommandId::CommControl))
            {
                return false;
            }

            if (m_active_request.subfunction_id != static_cast<uint8_t>(CommControl::Subfunction::Discover))
            {
                return false;
            }

            if (m_active_request.data_length < sizeof(CommControl::DISCOVER_MAGIC))
            {
                return false;
            }

            if (memcmp(CommControl::DISCOVER_MAGIC, m_active_request.data, sizeof(CommControl::DISCOVER_MAGIC)) != 0)
            {
                return false;
            }

            return true;
        }

        // Check if the last request received is a valid "Comm Discover request".
        bool CommHandler::received_connect_request(void)
        {
            if (m_active_request.command_id != static_cast<uint8_t>(CommandId::CommControl))
            {
                return false;
            }

            if (m_active_request.subfunction_id != static_cast<uint8_t>(CommControl::Subfunction::Connect))
            {
                return false;
            }

            return true;
        }

        void CommHandler::process(void)
        {
            if (m_session_active)
            {
                if (m_timebase->has_expired(m_heartbeat_timestamp, SCRUTINY_COMM_HEARTBEAT_TIMEOUT_US * 10))
                {
                    reset(); // Disable and reset all internal vars
                }
            }
        }

        bool CommHandler::heartbeat(const uint16_t challenge)
        {
            bool success = false;
            if (!m_session_active || !m_enabled)
            {
                return false;
            }

            if ((challenge != m_last_heartbeat_challenge || m_first_heartbeat_received == false))
            {
                m_first_heartbeat_received = true;
                m_heartbeat_timestamp = m_timebase->get_timestamp();
                m_last_heartbeat_challenge = challenge;
                success = true;
            }
            return success;
        }

        uint16_t CommHandler::data_to_send(void) const
        {
            if (m_state != State::Transmitting)
            {
                return 0;
            }

            return m_nbytes_to_send - m_nbytes_sent;
        }

        bool CommHandler::check_crc(const Request *req) const
        {
            uint32_t crc = 0;
            uint8_t header_data[4];
            header_data[0] = req->command_id;
            header_data[1] = req->subfunction_id;
            header_data[2] = (req->data_length >> 8) & 0xFF;
            header_data[3] = req->data_length & 0xFF;
            crc = tools::crc32(header_data, sizeof(header_data));
            crc = tools::crc32(req->data, req->data_length, crc);
            return (crc == req->crc);
        }

        void CommHandler::add_crc(Response *response) const
        {
            if (response->data_length > m_tx_buffer_size)
                return;

            uint8_t header[5];
            header[0] = response->command_id;
            header[1] = response->subfunction_id;
            header[2] = response->response_code;
            header[3] = (response->data_length >> 8) & 0xFF;
            header[4] = response->data_length & 0xFF;

            uint32_t crc = tools::crc32(header, sizeof(header));
            response->crc = tools::crc32(response->data, response->data_length, crc);
        }

        void CommHandler::reset(void)
        {
            m_state = State::Idle;
            m_heartbeat_timestamp = m_timebase->get_timestamp();
            m_last_heartbeat_challenge = 0;
            m_first_heartbeat_received = false;
            m_session_id = 0;
            m_session_active = false;

            reset_rx();
            reset_tx();
        }

        void CommHandler::reset_rx(void)
        {
            m_active_request.reset();
            m_rx_state = RxFSMState::WaitForCommand;
            m_request_received = false;
            m_crc_bytes_received = 0;
            m_length_bytes_received = 0;
            m_data_bytes_received = 0;
            m_rx_error = RxError::None;
            m_last_rx_timestamp = m_timebase->get_timestamp();

            if (m_state == State::Receiving)
            {
                m_state = State::Idle;
            }
        }

        void CommHandler::reset_tx(void)
        {
            m_active_response.reset();
            m_nbytes_to_send = 0;
            m_nbytes_sent = 0;
            m_tx_error = TxError::None;

            if (m_state == State::Transmitting)
            {
                m_state = State::Idle;
            }
        }

        bool CommHandler::connect(void)
        {
            if (!m_enabled)
            {
                return false;
            }

            if (m_session_active)
            {
                return false;
            }

            m_session_id = s_session_counter++;
            m_session_active = true;
            m_first_heartbeat_received = false;
            m_heartbeat_timestamp = m_timebase->get_timestamp();
            reset_rx();

            return true;
        }

        void CommHandler::disconnect(void)
        {
            m_session_id = 0;
            m_session_active = false;
            m_first_heartbeat_received = false;
            reset_rx();
            reset_tx();
        }

    } // namespace protocol
} // namespace scrutiny