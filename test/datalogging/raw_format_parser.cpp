//    raw_format_parser.cpp
//        Class that can reads the data encoded by the datalogging encoder. It does what the
//        server would do for testing purpose
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "datalogging/raw_format_parser.hpp"
#include "scrutiny.hpp"

void RawFormatParser::init(
    scrutiny::MainHandler *main_handler,
    scrutiny::datalogging::Configuration *config,
    unsigned char *buffer,
    uint32_t buffer_size)
{
    m_error = false;
    m_main_handler = main_handler;
    m_config = config;
    m_buffer = buffer;
    m_buffer_size = buffer_size;
    m_data.clear();

    if (m_buffer_size < 4)
    {
        m_error = true;
    }
}

uint16_t RawFormatParser::get_item_size_char(uint16_t item_index)
{
    if (m_config->items_to_log[item_index].type == scrutiny::datalogging::LoggableType::Memory)
    {
        return m_config->items_to_log[item_index].data.memory.size;
    }
    else if (m_config->items_to_log[item_index].type == scrutiny::datalogging::LoggableType::Rpv)
    {
        return scrutiny::tools::get_type_size_char(m_main_handler->get_rpv_type(m_config->items_to_log[item_index].data.rpv.id));
    }
    else if (m_config->items_to_log[item_index].type == scrutiny::datalogging::LoggableType::Time)
    {
        return sizeof(scrutiny::timestamp_t);
    }

    return 0;
}

unsigned char* RawFormatParser::get_parsed_data_location(uint16_t entry_index, uint16_t item_index)
{
    uint16_t entry_size_char = get_entry_size_char();
    uint16_t elem_offset = 0;

    if (item_index >= m_config->items_count)
    {
        return SCRUTINY_NULL;
    }
    for (uint16_t j = 0; j < item_index; j++)
    {       
        uint16_t size = get_item_size_char(j);
        if (size == 0){
            return SCRUTINY_NULL;
        }
        elem_offset += size ;
    }

    uint16_t size = get_item_size_char(item_index);
    if (size == 0)
    {
        return SCRUTINY_NULL;

    }

    uint16_t elem_pos = entry_index * entry_size_char + elem_offset;
    if (m_data.size() < elem_pos+size)
    {
        return SCRUTINY_NULL;
    }

    return &m_data[entry_index * entry_size_char + elem_offset];
}

uint16_t  RawFormatParser::get_entry_size_char()
{
    uint16_t entry_size_char = 0;
    for (uint16_t i = 0; i < m_config->items_count; i++)
    {
        uint16_t elem_size_char = get_item_size_char(i);

        if (elem_size_char == 0)
        {
            return 0 ;
        }

        entry_size_char += elem_size_char;
    }

    return entry_size_char;
}

void RawFormatParser::parse(uint32_t entry_count)
{
    if (m_error)
    {
        return;
    }
    uint32_t src_cursor = 0;
    uint32_t entry_size_char = get_entry_size_char();

    if (entry_size_char == 0)
    {
        m_error = true;
        return;
    }
    
    if (entry_count > (m_buffer_size / entry_size_char / (CHAR_BIT/8)))
    {
        m_error = true;
        return;
    }

    m_data.resize(entry_count * entry_size_char);
    if (m_data.size() != entry_count * entry_size_char)
    {
        m_data.clear();
        m_error = true;
        return;
    }

    for (uint16_t i = 0; i < entry_count; i++)
    {
        for (uint16_t j = 0; j < m_config->items_count; j++)
        {
            uint16_t elem_size_char = get_item_size_char(j);
            unsigned char* dst_ptr = get_parsed_data_location(i, j);

            if (dst_ptr == SCRUTINY_NULL || elem_size_char == 0)
            {
                m_error = true;
                m_data.clear();
                return;
            }

            // The parser reads back what has been written in the output buffer by the reader.
            // the output buffer is dilated. Compress back to use full char
            scrutiny::tools::memcpy_compress_from_8bits(dst_ptr, &m_buffer[src_cursor], elem_size_char * (CHAR_BIT/8));
            src_cursor += elem_size_char * (CHAR_BIT/8);
        }
    }
}
