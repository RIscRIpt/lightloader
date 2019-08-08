#pragma once

#include <Windows.h>

#include "scmanager.hpp"
#include "driver.hpp"

#include <functional>

class Service {
public:
    Service(SCManager const &sc_manager, Driver const &driver);
    ~Service() noexcept(false);

    SERVICE_STATUS status() const;
    DWORD type() const;
    bool is_loaded() const;
    void load();
    void unload();
    void wait(std::function<bool(SERVICE_STATUS const &status)> continue_predicate);
    void release();

private:
    SC_HANDLE handle_;
};
