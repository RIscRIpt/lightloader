#include <Windows.h>

#include "exceptions.hpp"

void ex::throw_last_win32_error() {
    throw std::system_error(GetLastError(), std::generic_category());
}
