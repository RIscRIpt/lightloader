#pragma once

#include <Windows.h>

class SCManager final {
public:
    SCManager(LPCTSTR machine_name, LPCTSTR database_name, DWORD desired_access);
    SCManager();
    ~SCManager() noexcept(false);

    inline operator SC_HANDLE() const { return handle_; }

private:
    SC_HANDLE handle_;
};
