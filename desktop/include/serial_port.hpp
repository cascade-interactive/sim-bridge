#pragma once

#include <windows.h>
#include <string>

class SerialPort {
public:
    SerialPort();
    ~SerialPort();

    // Delete copy
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;

    // Allow move
    SerialPort(SerialPort&& other) noexcept;
    SerialPort& operator=(SerialPort&& other) noexcept;

    // Setup
    bool open(const std::string& port, int baudRate);
    void close();

    // I/O
    bool write(const void* data, int size);
    int read(void* buffer, int bufferSize);

    // State
    bool isOpen() const;

private:
    HANDLE m_handle;
    DCB m_dcb;
    COMMTIMEOUTS m_timeouts;
};