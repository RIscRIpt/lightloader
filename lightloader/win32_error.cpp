#include "win32_error.hpp"

#include <vector>

#include <Windows.h>

Win32Error::Win32Error(uint32_t error)
    : std::runtime_error(ErrorToMessage(error))
    , error(error)
{
}

std::string Win32Error::ErrorToMessage(uint32_t error)
{
    std::vector<char> buffer(4096);
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                   nullptr,
                   error,
                   LANG_ENGLISH,
                   buffer.data(),
                   static_cast<DWORD>(buffer.size()),
                   nullptr);
    return buffer.data();
}

void ThrowLastWin32Error()
{
    throw Win32Error(GetLastError());
}
