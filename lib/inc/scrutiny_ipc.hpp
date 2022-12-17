//    scrutiny_ipc.hpp
//        Some tools for inter-=process communications
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_IPC_H___
#define ___SCRUTINY_IPC_H___

#ifndef __AVR_ARCH__
#include <atomic>
#include <utility>
#endif
namespace scrutiny
{
#ifdef __AVR_ARCH__
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
            m_written = true;
        }

        inline void clear(void)
        {
            m_written = false;
        }

        inline void set_data(const T &indata)
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
    template <class T>
    class IPCMessage
    {
    public:
        T data;
        IPCMessage()
        {
            clear();
        }

        inline bool has_content(void) const
        {
            return m_written.load();
        }

        inline void commit(void)
        {
            m_written.store(true);
        }

        inline void clear(void)
        {
            m_written.store(false);
        }

        inline void set_data(const T &indata)
        {
            data = indata;
            commit();
        }

        inline void set_data(const T &&indata)
        {
            data = std::move(indata);
            commit();
        }

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