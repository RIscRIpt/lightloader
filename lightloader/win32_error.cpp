#include "win32_error.hpp"

#include <vector>

#include <Windows.h>

Win32Error::Win32Error(uint32_t error)
    : error(error)
{
}

void ThrowLastWin32Error()
{
    throw Win32Error(GetLastError());
}
