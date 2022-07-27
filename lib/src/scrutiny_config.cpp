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


    void Config::copy_from(const Config* src)
    {
        clear();
        max_bitrate = src->max_bitrate;
        user_command_callback = src->user_command_callback;
        set_display_name(src->m_display_name);
        prng_seed = src->prng_seed;
        m_rpv_count = src->m_rpv_count;
        m_rpvs = src->m_rpvs;
        m_rpv_read_callback = src->m_rpv_read_callback;

        for (uint32_t i = 0; i < SCRUTINY_FORBIDDEN_ADDRESS_RANGE_COUNT; i++)
        {
            if (src->m_forbidden_address_ranges[i].set)
            {
                add_forbidden_address_range(src->m_forbidden_address_ranges[i].start, src->m_forbidden_address_ranges[i].end);
            }
        }

        for (uint32_t i = 0; i < SCRUTINY_READONLY_ADDRESS_RANGE_COUNT; i++)
        {
            if (src->m_readonly_address_ranges[i].set)
            {
                add_readonly_address_range(src->m_readonly_address_ranges[i].start, src->m_readonly_address_ranges[i].end);
            }
        }
    }

    void Config::clear()
    {
        max_bitrate = 0;
        m_rpv_count = 0;
        m_rpvs = nullptr;
        m_rpv_read_callback = nullptr;

        for (uint32_t i = 0; i < SCRUTINY_FORBIDDEN_ADDRESS_RANGE_COUNT; i++)
        {
            m_forbidden_address_ranges[i].set = false;
        }

        for (uint32_t i = 0; i < SCRUTINY_READONLY_ADDRESS_RANGE_COUNT; i++)
        {
            m_readonly_address_ranges[i].set = false;
        }

        m_forbidden_range_count = 0;
        m_readonly_range_count = 0;
        user_command_callback = nullptr;
        for (uint16_t i=0; i<SCRUTINY_DISPLAY_NAME_MAX_SIZE; i++)
        {
            m_display_name[i] = '\0';
        }
        prng_seed = 0;
    }

    bool Config::add_forbidden_address_range(void* start, void* end)
    {
        return add_forbidden_address_range(reinterpret_cast<uint64_t>(start), reinterpret_cast<uint64_t>(end));
    }

    bool Config::add_readonly_address_range(void* start, void* end)
    {
        return add_readonly_address_range(reinterpret_cast<uint64_t>(start), reinterpret_cast<uint64_t>(end));
    }

    bool Config::add_forbidden_address_range(const uint64_t start, const uint64_t end)
    {
        if (m_forbidden_range_count >= SCRUTINY_FORBIDDEN_ADDRESS_RANGE_COUNT)
        {
            return false;
        }

        m_forbidden_address_ranges[m_forbidden_range_count].start = start;
        m_forbidden_address_ranges[m_forbidden_range_count].end = end;
        m_forbidden_address_ranges[m_forbidden_range_count].set = true;
        m_forbidden_range_count++;
        return true;
    }

    bool Config::add_readonly_address_range(const uint64_t start, const uint64_t end)
    {
        if (m_readonly_range_count >= SCRUTINY_READONLY_ADDRESS_RANGE_COUNT)
        {
            return false;
        }

        m_readonly_address_ranges[m_readonly_range_count].start = start;
        m_readonly_address_ranges[m_readonly_range_count].end = end;
        m_readonly_address_ranges[m_readonly_range_count].set = true;
        m_readonly_range_count++;
        return true;
    }

    void Config::set_display_name(const char* name)
    {
        scrutiny::tools::strncpy(m_display_name, name, SCRUTINY_DISPLAY_NAME_MAX_SIZE);
    }

    void Config::set_published_values(RuntimePublishedValue* array, uint16_t nbr, RpvReadCallback rd_cb)
    {
        m_rpvs = array;
        m_rpv_count = nbr;
        m_rpv_read_callback = rd_cb;
    }

    bool Config::published_values_configured()
    {
        return (m_rpv_read_callback != nullptr && m_rpvs != nullptr && m_rpv_count > 0);
    }


}