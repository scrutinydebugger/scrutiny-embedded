//    scrutiny_ipc_std_atomic.hpp
//        An implementation of the Scrutiny IPC based on C++11 std::atomic.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_IPC_STD_ATOMIC_H___
#define ___SCRUTINY_IPC_STD_ATOMIC_H___

#include "scrutiny_setup.hpp"
#include <stdint.h>

#if !SCRUTINY_BUILD_TRICORE
#error "Only works for tricore architecture"
#endif

static inline void _scrutiny_ldmst(volatile void *address, uint32_t mask, uint32_t value)
{
    __asm__ volatile(
        "mov %H2,%1 \n\
                  ldmst [%0]0,%A2" ::"a"(address),
        "d"(mask),
        "d"((long long)value)
        : "memory");
}

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
        inline bool has_content(void) const { return m_written != 0; }

        /// @brief Mark the message as ready to be read using an atomic operation
        inline void commit(void) { _scrutiny_ldmst(&m_written, 1, 1); }

        /// @brief  Deletes the message content and leave rooms for the next one.
        inline void clear(void) { _scrutiny_ldmst(&m_written, 1, 0); }

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
        volatile uint32_t m_written;
    };
} // namespace scrutiny

#endif // ___SCRUTINY_IPC_STD_ATOMIC_H___
