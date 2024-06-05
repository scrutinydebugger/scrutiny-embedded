//    win_serial_port_bridge.c
//        Serial interface fopr the C testapp. Used to talk with the server
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#include "scrutiny_cwrapper.h"

#if !SCRUTINY_BUILD_WINDOWS
#error "File designed for windows"
#endif


#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <windows.h>
#include <winbase.h>
#include <fileapi.h>
#include <errhandlingapi.h>
#include <inttypes.h>

#include "win_serial_port_bridge.h"
#include "tools.h"

comm_channel_status_e win_serial_port_init(win_serial_port_t *serial_port, char *const port_name, uint32_t const baudrate)
{
    serial_port->m_portname = port_name;
    serial_port->m_baudrate = baudrate;
    serial_port->m_serial_handle = INVALID_HANDLE_VALUE;
    return COMM_CHANNEL_STATUS_success;
}


comm_channel_status_e win_serial_port_start(win_serial_port_t *serial_port)
{
    char const* prefix = "\\\\.\\";
    size_t const prefix_len = strlen(prefix);
    char portname[255] = {0};
    strcpy_s(portname, sizeof(portname), prefix);
    c_testapp_strncpy(&portname[prefix_len], serial_port->m_portname, sizeof(portname)-prefix_len);

    serial_port->m_serial_handle = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (serial_port->m_serial_handle == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Cannot open port %s. err=%" PRIu32, serial_port->m_portname, GetLastError());
        return COMM_CHANNEL_STATUS_error;
    }

    DCB serial_params;
    memset(&serial_params, 0, sizeof(DCB));
    serial_params.DCBlength = sizeof(serial_params);

    BOOL HResult;
    HResult = GetCommState(serial_port->m_serial_handle, &serial_params);
    if (HResult == FALSE)
    {
        win_serial_port_stop(serial_port);
        fprintf(stderr, "Cannot open port %s. GetCommState failed", serial_port->m_portname);
        return COMM_CHANNEL_STATUS_error;
    }

    serial_params.BaudRate = serial_port->m_baudrate;
    serial_params.ByteSize = 8;
    serial_params.StopBits = ONESTOPBIT;
    serial_params.Parity = NOPARITY;
    serial_params.fRtsControl = RTS_CONTROL_DISABLE;
    serial_params.fDtrControl = DTR_CONTROL_DISABLE;
    HResult = SetCommState(serial_port->m_serial_handle, &serial_params);
    if (HResult == FALSE)
    {
        win_serial_port_stop(serial_port);
        fprintf(stderr, "Cannot open port %s. SetCommState failed", serial_port->m_portname);
        return COMM_CHANNEL_STATUS_error;
    }

    HResult = PurgeComm(serial_port->m_serial_handle, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
    if (HResult == FALSE)
    {
        win_serial_port_stop(serial_port);
        fprintf(stderr, "Cannot open port %s. PurgeComm failed", serial_port->m_portname);
        return COMM_CHANNEL_STATUS_error;
    }

    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e win_serial_port_stop(win_serial_port_t *serial_port)
{
    if (serial_port->m_serial_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(serial_port->m_serial_handle);
    }

    serial_port->m_serial_handle = INVALID_HANDLE_VALUE;
    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e win_serial_port_receive(win_serial_port_t *serial_port, uint8_t *buffer, int len, int *ret)
{
    DWORD nbRead = 0;
    BOOL HResult;
    if (serial_port->m_serial_handle != INVALID_HANDLE_VALUE)
    {
        HResult = ReadFile(serial_port->m_serial_handle, buffer, len, &nbRead, NULL);
        if (HResult == FALSE)
        {
            win_serial_port_stop(serial_port);
            fprintf(stderr, "Cannot read port %s.", serial_port->m_portname);
            return COMM_CHANNEL_STATUS_error;
        }
    }

    *ret = nbRead;
    return COMM_CHANNEL_STATUS_success;
}

comm_channel_status_e win_serial_port_send(win_serial_port_t *serial_port, uint8_t const *buffer, int len)
{
    BOOL HResult;

    if (serial_port->m_serial_handle != INVALID_HANDLE_VALUE)
    {
        HResult = WriteFile(serial_port->m_serial_handle, buffer, len, NULL, NULL);
        if (HResult == FALSE)
        {
            win_serial_port_stop(serial_port);
            fprintf(stderr, "Cannot write port %s.", serial_port->m_portname);
            return COMM_CHANNEL_STATUS_error;
        }

        FlushFileBuffers(serial_port->m_serial_handle); // Do not check result as this may fail on virtual driver.
    }

    return COMM_CHANNEL_STATUS_success;
}
