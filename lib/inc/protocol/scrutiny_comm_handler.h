//    scrutiny_comm_handler.h
//        Manage the tranport-ish layer of the communication protocol. Converts Request/Response
//        to byte stream and validate CRC
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny)
//
//   Copyright (c) 2021-2022 scrutinydebugger

#ifndef ___SCRUTINY_COMM_HANDLER_H___
#define ___SCRUTINY_COMM_HANDLER_H___

#include <cstdint>

#include "scrutiny_setup.h"
#include "scrutiny_timebase.h"
#include "scrutiny_protocol.h"

#include "scrutiny_protocol_definitions.h"

namespace scrutiny
{
	namespace protocol
	{
		class CommHandler
		{
		public:

			void init(Timebase* timebase);
			void receive_data(uint8_t* data, uint32_t len);
			bool send_response(Response* response);
			void reset();
			Response* prepare_response();

			uint32_t pop_data(uint8_t* buffer, uint32_t len);
			uint32_t data_to_send();

			bool check_crc(const Request* req);
			void add_crc(Response* response);

			bool heartbeat(uint16_t challenge);
			void process();
			bool connect();
			void disconnect();

			inline void wait_next_request() { reset_rx(); }

			inline bool request_received() { return m_request_received; }
			inline Request* get_request() { return &m_active_request; }
			inline RxError get_rx_error() const { return m_rx_error; }
			inline bool transmitting() const { return (m_state == State::Transmitting); }
			inline bool receiving() const { return (m_state == State::Receiving); }

			inline bool is_enabled() const { return m_enabled; }
			inline void enable() { m_enabled = true; }
			inline void disable() { reset(); m_enabled = false; }
			inline bool is_connected() { return m_session_active; }
			inline uint32_t get_session_id() { return m_session_id; }
			inline uint16_t rx_buffer_size() { return SCRUTINY_RX_BUFFER_SIZE; }
			inline uint16_t tx_buffer_size() { return SCRUTINY_TX_BUFFER_SIZE; }

		protected:

			void process_active_request();
			bool received_discover_request();
			bool received_connect_request();

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

			Timebase* m_timebase;
			State m_state;
			bool m_enabled;
			uint32_t m_session_id;
			bool m_session_active;
			uint32_t m_heartbeat_timestamp;
			uint16_t m_last_heartbeat_challenge;
			bool m_heartbeat_received;

			// Reception
			uint8_t m_rx_buffer[SCRUTINY_RX_BUFFER_SIZE];
			uint8_t m_tx_buffer[SCRUTINY_TX_BUFFER_SIZE];
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
			uint32_t m_nbytes_to_send;
			uint32_t m_nbytes_sent;
			TxError m_tx_error;
		};
	}
}



#endif //___SCRUTINY_COMM_HANDLER_H___