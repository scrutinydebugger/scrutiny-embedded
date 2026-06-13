//    scrutiny_datalogger_raw_encoder.cpp
//        Class that handles the encoding of the datalogger data. RawFormat just copy to memory,
//        no encoding scheme.
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "datalogging/scrutiny_datalogger_raw_encoder.hpp"
#include "scrutiny_common_codecs.hpp"
#include "scrutiny_main_handler.hpp"
#include "scrutiny_setup.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        RawFormatReader::RawFormatReader(RawFormatEncoder const *const encoder) :
            m_encoder(encoder),
            m_read_cursor(0),
            m_finished(false),
            m_read_started(false)
        {
        }

        /// @brief Reads a chunk of data from the datalogger buffer and copy it to the output buffer making sure there is 8bits per char
        /// @param buffer Output buffer
        /// @param max_size Maximum size to copy, in multiple of 8bits
        /// @return Number of bytes written
        datalogging::buffer_size_t RawFormatReader::read_dilate_8bits(unsigned char *const buffer_8bits, datalogging::buffer_size_t max_size_8bits)
        {
// Make sure we do not read half a char. We don't have a state variable to remember that. Would be innefficient.
#if CHAR_BIT == 8
#elif CHAR_BIT == 16
            max_size_8bits &= static_cast<datalogging::buffer_size_t>(-2);
#elif CHAR_BIT == 32
            max_size_8bits &= static_cast<datalogging::buffer_size_t>(-4);
#else
#error
#endif

            datalogging::buffer_size_t output_cursor_8bits = 0;
            if (error())
            {
                return 0;
            }

            datalogging::buffer_size_t const write_cursor = m_encoder->get_write_cursor();
            datalogging::buffer_size_t const buffer_end = m_encoder->get_buffer_effective_size(); // Encoder may not use the full buffer
            if (m_read_cursor == write_cursor && m_read_started)
            {
                m_finished = true;
                return 0;
            }

            // Will do a maximum of 2 loops only if there is a wrap in the buffer.
            // This will cause 2 memcpy   start to buffer_end & buffer start to end
            // Otherwise a 1 loop and 1 memcpy
            while (output_cursor_8bits < max_size_8bits)
            {
                datalogging::buffer_size_t transfer_size_8bits;
                datalogging::buffer_size_t const new_max_8bits = max_size_8bits - output_cursor_8bits;
                datalogging::buffer_size_t const right_hand_start_point = (write_cursor > m_read_cursor) ? write_cursor : buffer_end;
                transfer_size_8bits = (right_hand_start_point - m_read_cursor) * (CHAR_BIT / 8);
                transfer_size_8bits = SCRUTINY_MIN(transfer_size_8bits, new_max_8bits);
                tools::memcpy_dilate_8bits_native(&buffer_8bits[output_cursor_8bits], &m_encoder->m_buffer[m_read_cursor], transfer_size_8bits);
                m_read_cursor += transfer_size_8bits / (CHAR_BIT / 8);
                m_read_started = true;
                output_cursor_8bits += transfer_size_8bits;
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

            return output_cursor_8bits;
        }

        /// @brief Returns the total number of bytes that the reader will read in char
        datalogging::buffer_size_t RawFormatReader::get_total_size_char(void) const
        {
            if (error())
            {
                return 0;
            }

            return m_encoder->m_entries_count * m_encoder->m_entry_size;
        }

        /// @brief Reset the reader
        void RawFormatReader::reset(void)
        {
            m_read_started = false;
            m_finished = false;
            m_read_cursor = m_encoder->get_read_cursor();
        }

        RawFormatEncoder::RawFormatEncoder() :
            m_buffer(SCRUTINY_NULL),
            m_buffer_size(0),
            m_config(SCRUTINY_NULL),
            m_reader(this),
            m_main_handler(SCRUTINY_NULL),
            m_timebase(SCRUTINY_NULL),
            m_max_entries(0),
            m_next_entry_write_index(0),
            m_first_valid_entry_index(0),
            m_entry_write_counter(0),
            m_entry_size(0),
            m_entries_count(0),
            m_full(false),
            m_error(false)
        {
        }

        /// @brief Takes a snapshot of the data to log and write it into the datalogger buffer
        void RawFormatEncoder::encode_next_entry(LoopHandler *const caller)
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

            datalogging::buffer_size_t cursor = m_next_entry_write_index * m_entry_size;
            for (uint_fast8_t i = 0; i < m_config->items_count; i++)
            {
                if (m_config->items_to_log[i].type == datalogging::LoggableType::Memory)
                {
                    m_main_handler->read_memory(
                        &m_buffer[cursor],
                        m_config->items_to_log[i].data.memory.address,
                        m_config->items_to_log[i].data.memory.size);
                    cursor += m_config->items_to_log[i].data.memory.size; // We verified that this is not 0 in init
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::Rpv)
                {
                    RuntimePublishedValue rpv;
                    AnyType outval;
                    uint16_t const rpv_id = m_config->items_to_log[i].data.rpv.id;
                    m_main_handler->get_rpv(rpv_id, &rpv); // assumed valid because of config validation
                    bool const success = m_main_handler->get_rpv_read_callback()(
                        rpv,
                        &outval,
                        caller); // We assume that this is not nullptr. We rely on datalogger::configure

                    if (!success)
                    {
                        tools::set_biggest_uint(outval, 0);
                    }
                    cursor += codecs::encode_anytype_big_endian_char(&outval, rpv.type, &m_buffer[cursor]);
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::Time)
                {
                    // No check for m_timebase == nullptr.
                    // Expect the datalogger to set it.
                    unsigned char tmp[4];
                    codecs::encode_32_bits_big_endian_8bits(m_timebase->get_timestamp(), &tmp);
                    tools::memcpy_compress_from_8bits_big_endian(&m_buffer[cursor], tmp);
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

            m_entry_write_counter++;
        }

        /// @brief  Init the encoder
        void RawFormatEncoder::init(
            MainHandler const *const main_handler,
            datalogging::Configuration const *const config,
            unsigned char *const buffer,
            datalogging::buffer_size_t const buffer_size)
        {
            m_main_handler = main_handler;
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

            if (m_buffer == SCRUTINY_NULL || m_buffer_size == 0)
            {
                m_error = true;
            }

            for (uint_fast8_t i = 0; i < m_config->items_count; i++)
            {
                if (m_error)
                {
                    break;
                }
                uint_fast8_t elem_size = 0;
                if (m_config->items_to_log[i].type == datalogging::LoggableType::Memory)
                {
                    elem_size = m_config->items_to_log[i].data.memory.size; // Size if char
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::Rpv)
                {
                    RuntimePublishedValue rpv;

                    if (!m_main_handler->get_rpv(m_config->items_to_log[i].data.rpv.id, &rpv))
                    {
                        m_error = true;
                    }
                    else
                    {
                        elem_size = tools::get_type_size_char(rpv.type); // Size if char
                    }
                }
                else if (m_config->items_to_log[i].type == datalogging::LoggableType::Time)
                {
                    elem_size = sizeof(scrutiny::timestamp_t); // Size if char
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

            if (m_max_entries == 0)
            {
                m_error = true;
            }
            m_reader.reset();
        }

        datalogging::buffer_size_t RawFormatEncoder::remaining_bytes_to_full() const
        {
            if (m_full)
            {
                return 0;
            }

            return get_buffer_effective_size() - get_write_cursor();
        }
    } // namespace datalogging
} // namespace scrutiny
