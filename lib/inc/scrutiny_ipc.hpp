#ifndef ___SCRUTINY_IPC_H___
#define ___SCRUTINY_IPC_H___

#include <atomic>
#include <utility>

namespace scrutiny
{
    template <class T>
    class IPCMessage
    {
    public:
        T data;
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
}

#endif // ___SCRUTINY_IPC_H___