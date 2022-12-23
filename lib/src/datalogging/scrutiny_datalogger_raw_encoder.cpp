#include "datalogging/scrutiny_datalogger_raw_encoder.hpp"
#include "scrutiny_main_handler.hpp"

#ifndef MIN
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#endif
namespace scrutiny
{
    namespace datalogging
    {
        /// @brief Reads a chunk of data from the datalogger buffer and copy it to the output buffer
        /// @param buffer Output buffer
        /// @param max_size Maximum size to copy
        /// @return Number of bytes written
        uint32_t RawFormatReader::read(uint8_t *buffer, const uint32_t max_size)
        {
            uint32_t output_size = 0;
            const uint32_t write_cursor = m_encoder->get_write_cursor();
            if (m_read_cursor == write_cursor)
            {
                return 0;
            }
            while (output_size < max_size)
            {
                uint32_t transfer_size;
                const uint32_t new_max = max_size - output_size;
                const uint32_t right_hand_start_point = (write_cursor > m_read_cursor) ? write_cursor : m_encoder->m_buffer_size;
                transfer_size = right_hand_start_point - m_read_cursor;
                transfer_size = MIN(transfer_size, new_max);
                memcpy(buffer, &m_encoder->m_buffer[m_read_cursor], transfer_size);

                m_read_cursor += transfer_size;
                output_size += transfer_size;
                if (m_read_cursor > write_cursor)
                {
                    if (m_read_cursor >= m_encoder->m_buffer_size)
                    {
                        m_read_cursor -= m_encoder->m_buffer_size;
                    }
                }
                else if (m_read_cursor == write_cursor)
                {
                    break;
                }
            }

            return output_size;
        }

        /// @brief Reset the reader
        void RawFormatReader::reset(void)
        {
            m_read_cursor = m_encoder->get_read_cursor();
        }

        /// @brief Takes a snapshot of the data to log and write it into the datalogger buffer
        void RawFormatEncoder::encode_next_entry(void)
        {
            if (m_next_entry_write_index == m_first_valid_entry_index && m_full)
            {
                m_first_valid_entry_index++;
                if (m_first_valid_entry_index >= m_max_entries)
                {
                    m_first_valid_entry_index = 0;
                }
            }

            uint32_t cursor = m_next_entry_write_index * m_entry_size;
            for (uint_fast8_t i = 0; i < m_config->items_count; i++)
            {
                if (m_config->items_to_log[i].type == datalogging::LoggableType::MEMORY)
                {
                    m_main_handler->read_memory(&m_buffer[cursor], m_config->items_to_log[i].memory.address, m_config->items_to_log[i].memory.size);
                    cursor += m_config->items_to_log[i].memory.size;
                }
            }

            m_next_entry_write_index++;
            if (m_next_entry_write_index >= m_max_entries)
            {
                m_full = true;
                m_next_entry_write_index = 0;
            }

            m_write_counter += m_entry_size;
        }

        /// @brief  Init the encoder
        void RawFormatEncoder::init()
        {
            reset_write_counter();
            m_next_entry_write_index = 0;
            m_first_valid_entry_index = 0;
            m_entry_size = 0;
            m_entries_count = 0;
            m_full = false;
            for (uint_fast8_t i = 0; i < m_config->items_count; i++)
            {
                if (m_config->items_to_log[i].type == datalogging::LoggableType::MEMORY)
                {
                    m_entry_size += m_config->items_to_log[i].memory.size;
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::RPV)
                {
                    const scrutiny::VariableType rpv_type = m_main_handler->get_rpv_type(m_config->items_to_log[i].rpv.id);
                    m_entry_size += tools::get_type_size(rpv_type);
                }
            }

            m_max_entries = m_buffer_size / m_entry_size;
        }
    }
}