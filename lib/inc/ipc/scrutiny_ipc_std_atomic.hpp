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

#if !SCRUTINY_HAS_CPP11
#error "C++11 is required"
#endif

#include <atomic>
#include <utility>

namespace scrutiny
{
    /// @brief Message that can be sent to another time domain without race condition.
    /// It is designed for one producer and one consumer.  It is the responsibility of the sender to
    /// wait for message to be cleared before writing a new one
    /// @param T DataType to send
    template <class T>
    class IPCMessage
    {
    public:
        T data;
        IPCMessage()
        {
            clear();
        }

        /// @brief Tells if the message content is valid and can be read.
        inline bool has_content(void) const
        {
            return m_written.load();
        }

        /// @brief Mark the message as ready to be read using an atomic operation
        inline void commit(void)
        {
            m_written.store(true);
        }

        /// @brief  Deletes the message content and leave rooms for the next one.
        inline void clear(void)
        {
            m_written.store(false);
        }

        /// @brief Sends a message to the receiver. Meant to be used by the producer
        /// @param indata Data to be sent
        inline void send(T const &indata)
        {
            data = indata;
            commit();
        }

        /// @brief Sends a message to the receiver. Meant to be used by the producer
        /// @param indata Data to be sent
        inline void send(T const &&indata)
        {
            data = std::move(indata);
            commit();
        }

        /// @brief Reads the message and clear it. Meant to be used by the consumer
        /// @return The message sent by the sender
        inline T pop(void)
        {
            T outdata = std::move(data);
            clear();
            return outdata;
        }

    protected:
        std::atomic<bool> m_written;
    };
}

#endif // ___SCRUTINY_IPC_STD_ATOMIC_H___
