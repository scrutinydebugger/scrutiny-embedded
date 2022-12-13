//    scrutiny_config.cpp
//        Implementation of the run-time Scrutiny configuration
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "scrutiny_config.hpp"
#include "scrutiny_tools.hpp"
#include <string.h>

namespace scrutiny
{
    Config::Config()
    {
        clear();
    }

    void Config::clear()
    {
        m_tx_buffer = nullptr;
        m_rx_buffer = nullptr;
        m_forbidden_address_ranges = nullptr;
        m_forbidden_range_count = 0;
        m_readonly_address_ranges = nullptr;
        m_readonly_range_count = 0;
        m_rpvs = nullptr;
        m_rpv_count = 0;
        m_rpv_read_callback = nullptr;
        m_rpv_write_callback = nullptr;
        display_name = "";
        max_bitrate = 0;
        user_command_callback = nullptr;
        prng_seed = 0;
        memory_write_enable = true;
    }

    void Config::set_buffers(uint8_t *rx_buffer, const uint16_t rx_buffer_size, uint8_t *tx_buffer, const uint16_t tx_buffer_size)
    {
        m_rx_buffer = rx_buffer;
        m_rx_buffer_size = rx_buffer_size;
        m_tx_buffer = tx_buffer;
        m_tx_buffer_size = tx_buffer_size;
    }

    void Config::set_forbidden_address_range(const AddressRange *range, const uint8_t count)
    {
        m_forbidden_address_ranges = range;
        m_forbidden_range_count = count;
    }

    void Config::set_readonly_address_range(const AddressRange *range, const uint8_t count)
    {
        m_readonly_address_ranges = range;
        m_readonly_range_count = count;
    }

    void Config::set_published_values(RuntimePublishedValue *array, uint16_t nbr, RpvReadCallback rd_cb, RpvWriteCallback wr_cb)
    {
        m_rpvs = array;
        m_rpv_count = nbr;
        m_rpv_read_callback = rd_cb;
        m_rpv_write_callback = wr_cb;
    }
}