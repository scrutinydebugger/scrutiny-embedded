//    scrutiny_timebase.hpp
//        Minimalist helper to keep track of time
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_TIMEBASE_H___
#define ___SCRUTINY_TIMEBASE_H___

#include <stdint.h>

namespace scrutiny
{
    typedef uint32_t timestamp_t;
    typedef uint32_t timediff_t;
    class Timebase
    {
    public:
        Timebase() : m_time_100ns(0) {}

        inline void step(const timediff_t timestep_100ns)
        {
            m_time_100ns += timestep_100ns;
        }

        inline timestamp_t get_timestamp() const
        {
            return m_time_100ns;
        };

        inline bool has_expired(const timestamp_t timestamp, const timediff_t timeout_100ns) const
        {
            bool expired = false;
            const timediff_t diff = m_time_100ns - timestamp;
            if (diff >= timeout_100ns)
            {
                expired = true;
            }

            return expired;
        }

        /// @brief Returns the number of 100ns elapsed since the timestamp has been taken
        /// @param timestamp The timestamp
        /// @return Time delta in multiple of 100ns
        inline timediff_t elapsed_since(const timestamp_t timestamp)
        {
            return m_time_100ns - timestamp;
        }

        /// @brief Returns the number of microsecs elapsed since the timestamp has been taken
        /// @param timestamp The timestamp
        /// @return Time delta in multiple of 100ns
        inline timediff_t elapsed_us_since(const timestamp_t timestamp)
        {
            return elapsed_since(timestamp) / 10;
        }

        inline void reset(const timestamp_t val = 0)
        {
            m_time_100ns = val;
        }

    protected:
        timestamp_t m_time_100ns;
    };
}

#endif