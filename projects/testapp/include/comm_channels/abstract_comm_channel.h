#ifndef ___ABSTRACT_COMM_CHANNEL_H___
#define ___ABSTRACT_COMM_CHANNEL_H___

#include <cstdint>
#include <cstdlib>

class AbstractCommChannel
{
    public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void send(const uint8_t* buffer, size_t len) = 0;
    virtual int receive(uint8_t* buffer, size_t len) = 0;
};

#endif  // ___ABSTRACT_COMM_CHANNEL_H___