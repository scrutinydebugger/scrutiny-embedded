//    abstract_comm_channel.hpp
//        Define the interface for a communication channel used by the testapp to run the scrutiny
//        embedded lib
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___ABSTRACT_COMM_CHANNEL_H___
#define ___ABSTRACT_COMM_CHANNEL_H___

#include <cstdlib>
#include <stdint.h>

class AbstractCommChannel
{
  public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void send(uint8_t const *buffer, int len) = 0;
    virtual int receive(uint8_t *buffer, int len) = 0;
};

#endif // ___ABSTRACT_COMM_CHANNEL_H___