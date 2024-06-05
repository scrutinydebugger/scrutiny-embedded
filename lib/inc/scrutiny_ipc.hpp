//    scrutiny_ipc.hpp
//        Some tools for inter-process communications
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_IPC_H___
#define ___SCRUTINY_IPC_H___

#include "scrutiny_setup.hpp"

#if !SCRUTINY_BUILD_AVR_GCC
#include <atomic>
#include <utility>
#endif
namespace scrutiny
{
#if SCRUTINY_BUILD_AVR_GCC

    /// @brief Message that can be sent to another time domain without race condition.
    /// It is designed for one producer and one consumer.  It is the responsibility of the sender to
    /// wait for message to be cleared before writing a new one
    /// @param T DataType to send
    template <class T>
    class IPCMessage
    {
    public:
        T data;
        IPCMessage() : m_written(false) {}
        inline bool has_content(void) const
        {
            return m_written;
        }

        inline void commit(void)
        {
            __asm__ __volatile__("cli" ::
                                     : "memory");
            m_written = true;
            __asm__ __volatile__("sei" ::
                                     : "memory");
        }

        inline void clear(void)
        {
            __asm__ __volatile__("cli" ::
                                     : "memory");
            m_written = false;
            __asm__ __volatile__("sei" ::
                                     : "memory");
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

#else
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
#endif
}

#endif // ___SCRUTINY_IPC_H___