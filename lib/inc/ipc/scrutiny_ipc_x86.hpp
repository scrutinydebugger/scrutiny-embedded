
#ifndef ___SCRUTINY_IPC_X86_H___
#define ___SCRUTINY_IPC_X86_H___

#include "scrutiny_setup.hpp"

#if !(SCRUTINY_BUILD_X64 || SCRUTINY_BUILD_X86)
#error "Can only be run on x86 instruction set"
#endif

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
        IPCMessage() : m_written(false) {}
        inline bool has_content(void) const
        {
            return m_written;
        }

        inline void commit(void)
        {
            __asm__ __volatile__("movl $1, %0" : "=m"(m_written) :: "memory");
        }

        inline void clear(void)
        {
            __asm__ __volatile__("movl $0, %0" : "=m"(m_written) :: "memory");
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

}

#endif // ___SCRUTINY_IPC_H___