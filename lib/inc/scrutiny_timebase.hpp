//    scrutiny_timebase.hpp
//        Minimalist helper to keep track of time
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_TIMEBASE_H___
#define ___SCRUTINY_TIMEBASE_H___

#include <stdint.h>

namespace scrutiny
{
    class Timebase
    {
    public:
        Timebase() : m_time_us(0) {}

        inline void step(const uint32_t timestep_us)
        {
            m_time_us += timestep_us;
        }

        inline uint32_t get_timestamp() const
        {
            return m_time_us;
        };

        inline bool has_expired(const uint32_t timestamp, const uint32_t timeout_us) const
        {
            bool expired = false;
            const uint32_t diff = m_time_us - timestamp;
            if (diff >= timeout_us)
            {
                expired = true;
            }

            return expired;
        }

        inline void reset(const uint32_t val = 0)
        {
            m_time_us = val;
        }

    protected:
        uint32_t m_time_us;
    };
}

#endif