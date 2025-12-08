//    scrutiny_config.cpp
//        Implementation of the run-time Scrutiny configuration
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_config.hpp"
#include "scrutiny_loop_handler.hpp"
#include "scrutiny_setup.hpp"
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
        m_tx_buffer = SCRUTINY_NULL;
        m_rx_buffer = SCRUTINY_NULL;
        m_rx_buffer_size = 0;
        m_tx_buffer_size = 0;
        m_forbidden_address_ranges = SCRUTINY_NULL;
        m_forbidden_range_count = 0;
        m_readonly_address_ranges = SCRUTINY_NULL;
        m_readonly_range_count = 0;
        m_rpvs = SCRUTINY_NULL;
        m_rpv_count = 0;
        m_rpv_read_callback = SCRUTINY_NULL;
        m_rpv_write_callback = SCRUTINY_NULL;
        display_name = "";
        max_bitrate = 0;
        m_user_command_callback = SCRUTINY_NULL;
        session_counter_seed = 0;
        memory_write_enable = true;
        m_loops = SCRUTINY_NULL;
        m_loop_count = 0;

#if SCRUTINY_ENABLE_DATALOGGING
        m_datalogger_buffer = SCRUTINY_NULL;
        m_datalogger_buffer_size = 0;
        m_datalogger_trigger_callback = SCRUTINY_NULL;
#endif
    }

    void Config::set_buffers(uint8_t *rx_buffer, uint16_t const rx_buffer_size, uint8_t *tx_buffer, uint16_t const tx_buffer_size)
    {
        m_rx_buffer = rx_buffer;
        m_rx_buffer_size = rx_buffer_size;
        m_tx_buffer = tx_buffer;
        m_tx_buffer_size = tx_buffer_size;
    }

    void Config::set_forbidden_address_range(AddressRange const *range, uint8_t const count)
    {
        m_forbidden_address_ranges = range;
        m_forbidden_range_count = count;
    }

    void Config::set_readonly_address_range(AddressRange const *range, uint8_t const count)
    {
        m_readonly_address_ranges = range;
        m_readonly_range_count = count;
    }

    void Config::set_published_values(
        RuntimePublishedValue const *const array,
        uint16_t const nbr,
        RpvReadCallback const rd_cb,
        RpvWriteCallback const wr_cb)
    {
        m_rpvs = array;
        m_rpv_count = nbr;
        m_rpv_read_callback = rd_cb;
        m_rpv_write_callback = wr_cb;
    }

    void Config::set_loops(LoopHandler **loops, uint8_t loop_count)
    {
        m_loops = loops;
        m_loop_count = loop_count;
    }

#if SCRUTINY_ENABLE_DATALOGGING
    void Config::set_datalogging_buffers(uint8_t *buffer, datalogging::buffer_size_t const buffer_size)
    {
        m_datalogger_buffer = buffer;
        m_datalogger_buffer_size = buffer_size;
    }

    bool Config::has_at_least_one_loop_with_datalogging(void) const
    {
        for (uint8_t i = 0; i < m_loop_count; i++)
        {
            if (m_loops[i]->datalogging_allowed())
            {
                return true;
            }
        }
        return false;
    }
#endif
} // namespace scrutiny
