//    scrutiny_ipc_ti_c28x.hpp
//        An implementation of the Scrutiny IPC based on AVR intrinsics that disables interrupt
//        for atomic store/load
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_IPC_TI_C28X_H___
#define ___SCRUTINY_IPC_TI_C28X_H___

#include "scrutiny_setup.hpp"

#if !SCRUTINY_BUILD_TI_C28x
#error "Can only be built for Texas Instrument TMS320C28x"
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
            m_written = true;
        }

        inline void clear(void)
        {
            m_written = false;
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

#endif // ___SCRUTINY_IPC_TI_C28X_H___
