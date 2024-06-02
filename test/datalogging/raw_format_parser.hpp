//    raw_format_parser.hpp
//        Class that can reads the data encoded by the datalogging encoder. It does what the
//        server would do for testing purpose
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___RAW_FORMAT_PARSER_HPP___
#define ___RAW_FORMAT_PARSER_HPP___

#include <stdint.h>
#include <vector>
#include "scrutiny.hpp"

class RawFormatParser
{
public:
    void init(scrutiny::MainHandler *main_handler, scrutiny::datalogging::Configuration *config, uint8_t *buffer, uint32_t buffer_size);
    void parse(uint32_t entry_count);
    inline std::vector<std::vector<std::vector<uint8_t>>> get(void) const { return m_data; };
    bool error(void) const { return m_error; }

protected:
    scrutiny::MainHandler *m_main_handler;
    uint8_t *m_buffer;
    uint32_t m_buffer_size;
    scrutiny::datalogging::Configuration *m_config;
    std::vector<std::vector<std::vector<uint8_t>>> m_data;
    bool m_error;
};

#endif // ___RAW_FORMAT_PARSER_HPP___