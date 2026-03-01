#include "serial_port.hpp"

SerialPort::SerialPort()
    : m_handle(INVALID_HANDLE_VALUE)
    , m_dcb{}
    , m_timeouts{}
{
}

SerialPort::~SerialPort()
{
    close();
}

SerialPort::SerialPort(SerialPort&& other) noexcept
    : m_handle(other.m_handle)
    , m_dcb(other.m_dcb)
    , m_timeouts(other.m_timeouts)
{
    other.m_handle = INVALID_HANDLE_VALUE;
}

SerialPort& SerialPort::operator=(SerialPort&& other) noexcept
{
    if (this != &other) {
        close();
        m_handle = other.m_handle;
        m_dcb = other.m_dcb;
        m_timeouts = other.m_timeouts;
        other.m_handle = INVALID_HANDLE_VALUE;
    }
    return *this;
}

bool SerialPort::open(const std::string& port, int baudRate)
{
    m_handle = CreateFileA(
        port.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (!isOpen()) return false;

    // Configure port
    m_dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(m_handle, &m_dcb)) {
        close();
        return false;
    }

    m_dcb.BaudRate = baudRate;
    m_dcb.ByteSize = 8;
    m_dcb.StopBits = ONESTOPBIT;
    m_dcb.Parity = NOPARITY;
    m_dcb.fDtrControl = DTR_CONTROL_ENABLE;
    m_dcb.fRtsControl = RTS_CONTROL_ENABLE;

    if (!SetCommState(m_handle, &m_dcb)) {
        close();
        return false;
    }

    // Set timeouts (non-blocking reads)
    m_timeouts.ReadIntervalTimeout = MAXDWORD;
    m_timeouts.ReadTotalTimeoutMultiplier = 0;
    m_timeouts.ReadTotalTimeoutConstant = 0;
    m_timeouts.WriteTotalTimeoutMultiplier = 0;
    m_timeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(m_handle, &m_timeouts)) {
        close();
        return false;
    }

    // Clear any existing data
    PurgeComm(m_handle, PURGE_RXCLEAR | PURGE_TXCLEAR);

    return true;
}

void SerialPort::close()
{
    if (isOpen()) {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
}

bool SerialPort::write(const void* data, int size)
{
    if (!isOpen()) return false;

    DWORD bytesWritten = 0;
    return WriteFile(m_handle, data, size, &bytesWritten, nullptr)
        && bytesWritten == static_cast<DWORD>(size);
}

int SerialPort::read(void* buffer, int bufferSize)
{
    if (!isOpen()) return -1;

    DWORD bytesRead = 0;
    if (!ReadFile(m_handle, buffer, bufferSize, &bytesRead, nullptr)) {
        return -1;
    }

    return static_cast<int>(bytesRead);
}

bool SerialPort::isOpen() const
{
    return m_handle != INVALID_HANDLE_VALUE;
}