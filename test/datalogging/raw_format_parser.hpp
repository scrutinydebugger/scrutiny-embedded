//    raw_format_parser.hpp
//        Class that can reads the data encoded by the datalogging encoder. It does what the
//        server would do for testing purpose
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___RAW_FORMAT_PARSER_HPP___
#define ___RAW_FORMAT_PARSER_HPP___

#include "scrutiny.hpp"
#include <stdint.h>
#include <vector>

class RawFormatParser
{
  public:
    void init(scrutiny::MainHandler *main_handler, scrutiny::datalogging::Configuration *config, unsigned char *buffer, uint32_t buffer_size);
    void parse(uint32_t entry_count);
    inline std::vector<unsigned char> *get(void) { return &m_data; };
    bool error(void) const { return m_error; }
    unsigned char *get_parsed_data_location(uint16_t entry_index, uint16_t item_index);
    uint16_t get_entry_count();
    uint16_t get_item_size_char(uint16_t item_index);
    uint16_t get_entry_size_char();

  protected:
    scrutiny::MainHandler *m_main_handler;
    unsigned char *m_buffer;
    uint32_t m_buffer_size;
    scrutiny::datalogging::Configuration *m_config;
    std::vector<unsigned char> m_data;
    bool m_error;
};

#endif // ___RAW_FORMAT_PARSER_HPP___
