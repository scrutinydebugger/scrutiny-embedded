//    scrutiny_ipc_avr.hpp
//        An implementation of the Scrutiny IPC based on AVR intrinsics that disables interrupt
//        for atomic store/load
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_IPC_AVR_H___
#define ___SCRUTINY_IPC_AVR_H___

#include "scrutiny_setup.hpp"

#if !SCRUTINY_BUILD_AVR_GCC
#error "Can only be built for AVR GCC"
#endif

namespace scrutiny
{

    /// @brief Message that can be sent to another time domain without race condition.
    /// It is designed for one producer and one consumer.  It is the responsibility of the sender to
    /// wait for message to be cleared before writing a new one
    /// @param T DataType to send
    template <class T> class IPCMessage
    {
      public:
        T data;
        IPCMessage() :
            m_written(false)
        {
        }
        inline bool has_content(void) const { return m_written; }

        inline void commit(void)
        {
            __asm__ __volatile__("cli" ::: "memory");
            m_written = true;
            __asm__ __volatile__("sei" ::: "memory");
        }

        inline void clear(void)
        {
            __asm__ __volatile__("cli" ::: "memory");
            m_written = false;
            __asm__ __volatile__("sei" ::: "memory");
        }

        inline void send(T const &indata)
        {
            data = indata;
            commit();
        }

        inline T pop(void)
        {
            T outdata = data;
            clear();
            return outdata;
        }

      protected:
        volatile bool m_written;
    };

} // namespace scrutiny

#endif // ___SCRUTINY_IPC_H___
