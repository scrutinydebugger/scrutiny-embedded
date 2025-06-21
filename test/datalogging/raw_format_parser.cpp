//    raw_format_parser.cpp
//        Class that can reads the data encoded by the datalogging encoder. It does what the
//        server would do for testing purpose
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "datalogging/raw_format_parser.hpp"
#include "scrutiny.hpp"

void RawFormatParser::init(scrutiny::MainHandler *main_handler, scrutiny::datalogging::Configuration *config, uint8_t *buffer, uint32_t buffer_size)
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

void RawFormatParser::parse(uint32_t entry_count)
{
    if (m_error)
    {
        return;
    }
    uint32_t cursor = 0;
    uint32_t entry_size = 0;
    for (uint16_t i = 0; i < m_config->items_count; i++)
    {
        std::vector<uint8_t> item_data;
        uint32_t elem_size = 0;

        if (m_config->items_to_log[i].type == scrutiny::datalogging::LoggableType::MEMORY)
        {
            elem_size = m_config->items_to_log[i].data.memory.size;
        }
        else if (m_config->items_to_log[i].type == scrutiny::datalogging::LoggableType::RPV)
        {
            elem_size = scrutiny::tools::get_type_size(m_main_handler->get_rpv_type(m_config->items_to_log[i].data.rpv.id));
        }
        else if (m_config->items_to_log[i].type == scrutiny::datalogging::LoggableType::TIME)
        {
            elem_size = sizeof(scrutiny::timestamp_t);
        }

        if (elem_size == 0)
        {
            m_error = true;
            return;
        }

        entry_size += elem_size;
    }

    if (entry_count > (m_buffer_size / entry_size))
    {
        m_error = true;
        return;
    }

    m_data.resize(entry_count);

    for (uint32_t i = 0; i < entry_count; i++)
    {
        std::vector< std::vector<uint8_t> > entry(m_config->items_count);
        for (uint16_t j = 0; j < m_config->items_count; j++)
        {
            uint32_t elem_size = 0;

            if (m_config->items_to_log[j].type == scrutiny::datalogging::LoggableType::MEMORY)
            {
                elem_size = m_config->items_to_log[j].data.memory.size;
            }
            else if (m_config->items_to_log[j].type == scrutiny::datalogging::LoggableType::RPV)
            {
                elem_size = scrutiny::tools::get_type_size(m_main_handler->get_rpv_type(m_config->items_to_log[j].data.rpv.id));
            }
            else if (m_config->items_to_log[j].type == scrutiny::datalogging::LoggableType::TIME)
            {
                elem_size = sizeof(scrutiny::timestamp_t);
            }

            if (elem_size == 0)
            {
                m_error = true;
                m_data.clear();
                return;
            }

            std::vector<uint8_t> item_data(elem_size);
            for (uint32_t k = 0; k < elem_size; k++)
            {
                item_data[k] = m_buffer[cursor++];
            }

            entry[j] = item_data;
        }
        m_data[i] = entry;
    }
}
