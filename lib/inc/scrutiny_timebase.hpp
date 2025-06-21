//    scrutiny_timebase.hpp
//        Minimalist helper to keep track of time
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_TIMEBASE_H___
#define ___SCRUTINY_TIMEBASE_H___

#include "scrutiny_setup.hpp"
#include <stdint.h>

namespace scrutiny
{
    typedef uint32_t timestamp_t;
    typedef uint32_t timediff_t;
    class Timebase
    {
      public:
        Timebase() :
            m_time_100ns(0)
        {
        }

        /// @brief Move the time forward by a step
        /// @param timestep_100ns Time step to do, in multiple of 100ns.
        inline void step(timediff_t const timestep_100ns) { m_time_100ns += timestep_100ns; }

        /// @brief Returns a timestamp that can be used to measure time delta with has_expired() and elapsed_since()
        inline timestamp_t get_timestamp(void) const { return m_time_100ns; };

        /// @brief Returns the number of 100ns elapsed since the timestamp has been taken
        /// @param timestamp The timestamp
        /// @return Time delta in multiple of 100ns
        inline timediff_t elapsed_since(timestamp_t const timestamp) const { return m_time_100ns - timestamp; }

        /// @brief Returns the number of microseconds elapsed since the timestamp has been taken
        /// @param timestamp The timestamp
        /// @return Time delta in multiple of 100ns
        inline timediff_t elapsed_us_since(timestamp_t const timestamp) const { return elapsed_since(timestamp) / 10; }

        /// @brief Returns true if the given timeout has elapsed since the given timestamp
        /// @param timestamp Timestamp to check against
        /// @param timeout_100ns Maximum time delta since the timestamp
        /// @return true if expired
        inline bool has_expired(timestamp_t const timestamp, timediff_t const timeout_100ns) const { return (elapsed_since(timestamp) >= timeout_100ns); }

        /// @brief Put back the timebase at the given timestamp (default 0)
        /// @param val timestamp to use a actual value
        inline void reset(timestamp_t const val = 0) { m_time_100ns = val; }

      protected:
        timestamp_t m_time_100ns;
    };
} // namespace scrutiny

#endif
