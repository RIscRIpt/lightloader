#pragma once

#include <cstdint>
#include <stdexcept>

class Win32Error : public std::runtime_error {
public:
    Win32Error(uint32_t error);

    static std::string ErrorToMessage(uint32_t error);

    uint32_t const error;
};

void ThrowLastWin32Error();
