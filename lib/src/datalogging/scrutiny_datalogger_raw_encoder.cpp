#include "datalogging/scrutiny_datalogger_raw_encoder.hpp"
#include "scrutiny_main_handler.hpp"
#include "scrutiny_common_codecs.hpp"

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
            if (m_error)
            {
                return 0;
            }
            if (!m_count_written)
            {
                if (max_size < 4)
                {
                    m_error = true;
                    return 0;
                }

                codecs::encode_32_bits_big_endian(m_encoder->m_entries_count, buffer);
                output_size += 4;
                m_count_written = true;
            }

            const uint32_t write_cursor = m_encoder->get_write_cursor();
            const uint32_t buffer_end = m_encoder->get_buffer_effective_end();
            if (m_read_cursor == write_cursor && m_read_started)
            {
                m_finished = true;
                return 0;
            }
            while (output_size < max_size)
            {
                uint32_t transfer_size;
                const uint32_t new_max = max_size - output_size;
                const uint32_t right_hand_start_point = (write_cursor > m_read_cursor) ? write_cursor : buffer_end;
                transfer_size = right_hand_start_point - m_read_cursor;
                transfer_size = MIN(transfer_size, new_max);
                memcpy(&buffer[output_size], &m_encoder->m_buffer[m_read_cursor], transfer_size);
                m_read_cursor += transfer_size;
                m_read_started = true;
                output_size += transfer_size;
                if (m_read_cursor > write_cursor)
                {
                    if (m_read_cursor >= buffer_end)
                    {
                        m_read_cursor -= buffer_end;
                    }
                }

                if (m_read_cursor == write_cursor)
                {
                    m_finished = true;
                    break;
                }
            }

            return output_size;
        }

        /// @brief Reset the reader
        void RawFormatReader::reset(void)
        {
            m_read_started = false;
            m_count_written = false;
            m_error = m_encoder->error();
            m_finished = false;
            m_read_cursor = m_encoder->get_read_cursor();
        }

        /// @brief Takes a snapshot of the data to log and write it into the datalogger buffer
        void RawFormatEncoder::encode_next_entry(void)
        {
            if (m_error)
            {
                return;
            }

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
                    m_main_handler->read_memory(&m_buffer[cursor], m_config->items_to_log[i].data.memory.address, m_config->items_to_log[i].data.memory.size);
                    cursor += m_config->items_to_log[i].data.memory.size; // We verified that this is not 0 in init
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::RPV)
                {
                    RuntimePublishedValue rpv;
                    AnyType outval;
                    const uint16_t rpv_id = m_config->items_to_log[i].data.rpv.id;
                    m_main_handler->get_rpv(rpv_id, &rpv);
                    const uint8_t typesize = tools::get_type_size(rpv.type);
                    m_main_handler->get_rpv_read_callback()(rpv, &outval);
                    codecs::encode_anytype_big_endian(&outval, typesize, &m_buffer[cursor]);
                    cursor += typesize;
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::TIME)
                {
                    codecs::encode_32_bits_big_endian(m_timebase->get_timestamp(), &m_buffer[cursor]);
                    cursor += sizeof(scrutiny::timestamp_t);
                }
            }

            if (!m_full)
            {
                m_entries_count++;
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
        void RawFormatEncoder::init(MainHandler *main_handler, Timebase *timebase, datalogging::Configuration *config, uint8_t *buffer, uint32_t buffer_size)
        {
            m_main_handler = main_handler;
            m_timebase = timebase;
            m_config = config;
            m_buffer = buffer;
            m_buffer_size = buffer_size;

            reset();
        }

        void RawFormatEncoder::reset(void)
        {
            reset_write_counter();
            m_error = false;
            m_next_entry_write_index = 0;
            m_first_valid_entry_index = 0;
            m_entry_size = 0;
            m_entries_count = 0;
            m_full = false;
            m_max_entries = 0;

            for (uint_fast8_t i = 0; i < m_config->items_count; i++)
            {
                if (m_error)
                {
                    break;
                }
                uint_fast8_t elem_size = 0;
                if (m_config->items_to_log[i].type == datalogging::LoggableType::MEMORY)
                {
                    elem_size = m_config->items_to_log[i].data.memory.size;
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::RPV)
                {
                    RuntimePublishedValue rpv;

                    if (!m_main_handler->get_rpv(m_config->items_to_log[i].data.rpv.id, &rpv))
                    {
                        m_error = true;
                    }
                    else
                    {
                        elem_size = tools::get_type_size(rpv.type);
                    }
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::TIME)
                {
                    elem_size = sizeof(scrutiny::timestamp_t);
                }

                if (elem_size == 0 && !m_error)
                {
                    m_error = true;
                }
                else
                {
                    m_entry_size += elem_size;
                }
            }
            if (m_entry_size > 0)
            {
                m_max_entries = m_buffer_size / m_entry_size;
            }
            else
            {
                m_error = true;
            }
        }
    }
}