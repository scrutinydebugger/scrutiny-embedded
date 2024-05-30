//    abstract_comm_channel.h
//        Define the interface for a communication channel used by the testapp to run the scrutiny
//        embedded lib
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___ABSTRACT_COMM_CHANNEL_H___
#define ___ABSTRACT_COMM_CHANNEL_H___

#include <stdint.h>

typedef enum
{
    COMM_CHANNEL_STATUS_success,
    COMM_CHANNEL_STATUS_error,
} comm_channel_status_e;

typedef comm_channel_status_e (*comm_channel_start_t)(void *channel);
typedef comm_channel_status_e (*comm_channel_stop_t)(void *channel);
typedef comm_channel_status_e (*comm_channel_send_t)(void *channel, const uint8_t *buffer, int len);
typedef comm_channel_status_e (*comm_channel_receive_t)(void *channel, uint8_t *buffer, int len, int* ret);

typedef struct
{
    void* handle;
    comm_channel_start_t start;
    comm_channel_stop_t stop;
    comm_channel_send_t send;
    comm_channel_receive_t receive;
} comm_channel_interface_t;

#endif // ___ABSTRACT_COMM_CHANNEL_H___
