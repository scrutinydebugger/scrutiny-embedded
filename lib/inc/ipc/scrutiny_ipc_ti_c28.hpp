//    scrutiny_ipc_ti_c28.hpp
//        An implementation of the Scrutiny IPC for Texas Instrument C200 family
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_IPC_TI_C28_H___
#define ___SCRUTINY_IPC_TI_C28_H___

#include "scrutiny_setup.hpp"
#include <stdint.h>
#if !SCRUTINY_BUILD_TI_C28
#error "Works only on Texas Instrument C28 series"
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
        IPCMessage() { clear(); }

        /// @brief Tells if the message content is valid and can be read.
        inline bool has_content(void) const
        {
            uint16_t primask = __disable_interrupts();
            bool const written = m_written;
            __restore_interrupts(primask);
            return written;
        }

        /// @brief Mark the message as ready to be read using an atomic operation
        inline void commit(void)
        {
            uint16_t primask = __disable_interrupts();
            m_written = true;
            __restore_interrupts(primask);
        }

        /// @brief  Deletes the message content and leave rooms for the next one.
        inline void clear(void)
        {
            uint16_t primask = __disable_interrupts();
            m_written = false;
            __restore_interrupts(primask);
        }

        /// @brief Sends a message to the receiver. Meant to be used by the producer
        /// @param indata Data to be sent
        inline void send(T const &indata)
        {
            data = indata;
            commit();
        }

        /// @brief Reads the message and clear it. Meant to be used by the consumer
        /// @return The message sent by the sender
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

#endif // ___SCRUTINY_IPC_STD_ATOMIC_H___
