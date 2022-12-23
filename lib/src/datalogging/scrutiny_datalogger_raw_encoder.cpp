#include "datalogging/scrutiny_datalogger_raw_encoder.hpp"

#ifndef MIN
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#endif
namespace scrutiny
{
    namespace datalogging
    {
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

        void RawFormatReader::reset(void)
        {
            m_read_cursor = m_encoder->get_read_cursor();
        }

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

            uint32_t cursor = m_next_entry_write_index * m_blocksize_sum;
            for (uint_fast8_t i = 0; i < m_config->block_count; i++)
            {
                memcpy(&m_buffer[cursor], m_config->memblocks[i], m_config->blocksizes[i]);
                cursor += m_config->blocksizes[i];
            }

            m_next_entry_write_index++;
            if (m_next_entry_write_index >= m_max_entries)
            {
                m_full = true;
                m_next_entry_write_index = 0;
            }

            m_write_counter += m_blocksize_sum;
        }

        void RawFormatEncoder::init()
        {
            reset_write_counter();
            m_next_entry_write_index = 0;
            m_first_valid_entry_index = 0;
            m_blocksize_sum = 0;
            m_entries_count = 0;
            m_full = false;
            for (uint_fast8_t i = 0; i < m_config->block_count; i++)
            {
                m_blocksize_sum += m_config->blocksizes[i];
            }

            m_max_entries = m_buffer_size / m_blocksize_sum;
        }
    }
}