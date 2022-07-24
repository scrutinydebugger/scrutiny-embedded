//    win_serial_port_bridge.cpp
//        Class that make a Windows Serial port available as a communication channel for the
//        Testapp
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#if  !defined(_WIN32)
#error "File designed for windows"
#endif

#include "win_serial_port_bridge.h"
#include <cstdlib>
#include <cstdint>

#include <windows.h>
#include <winbase.h>
#include <string>
#include <system_error>
#include <fileapi.h>

WinSerialPortBridge::WinSerialPortBridge(const std::string& port_name, uint32_t baudrate) : 
    m_port_name(port_name),
    m_baudrate(baudrate),
    m_serial_handle(INVALID_HANDLE_VALUE)
{
    
}

void WinSerialPortBridge::start()
{
    std::string comport = std::string("\\\\.\\") + m_port_name;
    m_serial_handle = CreateFile(comport.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (m_serial_handle == INVALID_HANDLE_VALUE)
    {
        throw_system_error(std::string("Cannot open port ") + comport);
    }

    DCB serial_params = { 0 };
    serial_params.DCBlength = sizeof(serial_params);

    BOOL HResult;
    HResult = GetCommState(m_serial_handle, &serial_params);
    if (HResult == FALSE)
    {
        stop();
        throw_system_error("Cannot open port. GetCommState failed");
    }
    serial_params.BaudRate = m_baudrate;
    serial_params.ByteSize = 8;
    serial_params.StopBits = ONESTOPBIT;
    serial_params.Parity = NOPARITY;
    serial_params.fRtsControl = RTS_CONTROL_DISABLE;
    serial_params.fDtrControl = DTR_CONTROL_DISABLE;
    HResult = SetCommState(m_serial_handle, &serial_params);
    if (HResult == FALSE)
    {
        stop();
        throw_system_error("Cannot open port. SetCommState failed");
    }

    HResult = PurgeComm(m_serial_handle, PURGE_RXABORT |  PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
    if (HResult == FALSE)
    {
        stop();
        throw_system_error("Cannot open port. PurgeComm failed");
    }
}


void WinSerialPortBridge::stop()
{   
    if (m_serial_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_serial_handle);
    }

    m_serial_handle = INVALID_HANDLE_VALUE;
}

void WinSerialPortBridge::throw_system_error(const std::string &msg)
{
    throw std::system_error(GetLastError(), std::system_category(), msg.c_str());
}

int WinSerialPortBridge::receive(uint8_t* buffer, int len)
{
    DWORD nbRead=0;
    BOOL HResult;
    if (m_serial_handle != INVALID_HANDLE_VALUE)
    {
        HResult = ReadFile(m_serial_handle, buffer, len, &nbRead, NULL);
        if (HResult == FALSE)
        {
            stop();
            throw_system_error("Cannot read port");
        }
    }

    return static_cast<int>(nbRead);
}

void WinSerialPortBridge::send(const uint8_t* buffer, int len)
{
    BOOL HResult;

    if (m_serial_handle != INVALID_HANDLE_VALUE)
    {
        HResult = WriteFile(m_serial_handle, buffer, len, NULL, NULL);
        if (HResult == FALSE)
        {
            stop();
            throw_system_error("Cannot write port");
        }

        FlushFileBuffers(m_serial_handle);  // Do not check result as this may fail on virtual driver.
    }
}


