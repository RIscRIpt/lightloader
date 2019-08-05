#include "service.hpp"

#include <sstream>

#include "exceptions.hpp"

Service::Service(SCManager const &sc_manager, Driver const &driver) {
    std::wstringstream path;
    path << std::quoted(driver.path().wstring(), L'"', L'^');
    handle_ = CreateService(
        sc_manager,
        driver.name().c_str(),
        driver.name().c_str(),
        SERVICE_ALL_ACCESS,
        SERVICE_KERNEL_DRIVER,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        path.str().c_str(),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );
    if (!handle_ && GetLastError() == ERROR_SERVICE_EXISTS) {
        handle_ = OpenService(
            sc_manager,
            driver.name().c_str(),
            SERVICE_ALL_ACCESS
        );
    }
    if (!handle_) {
        ex::throw_last_win32_error();
    }
    if (type() != SERVICE_KERNEL_DRIVER) {
        throw std::runtime_error("Existing service is not kernel driver.");
    }
}

Service::~Service() noexcept(false) {
    if (handle_) {
        if (!CloseServiceHandle(handle_)) {
            ex::throw_last_win32_error();
        }
        handle_ = NULL;
    }
}

SERVICE_STATUS Service::status() const {
    SERVICE_STATUS status;
    if (!QueryServiceStatus(handle_, &status)) {
        ex::throw_last_win32_error();
    }
    return status;
}

DWORD Service::type() const {
    return status().dwServiceType;
}

bool Service::is_loaded() const {
    switch (status().dwCurrentState) {
    case SERVICE_RUNNING:
    case SERVICE_START_PENDING:
    case SERVICE_CONTINUE_PENDING:
        return true;
    case SERVICE_PAUSED:
    case SERVICE_PAUSE_PENDING:
    case SERVICE_STOPPED:
    case SERVICE_STOP_PENDING:
        return false;
    }
    throw std::logic_error("Unexpected status.");
}

void Service::load() {
    SERVICE_STATUS status = this->status();
    switch (status.dwCurrentState) {
    case SERVICE_STOP_PENDING:
        wait([](SERVICE_STATUS const &status) {
            return status.dwCurrentState == SERVICE_STOP_PENDING;
            });
    case SERVICE_STOPPED:
        if (!StartService(handle_, 0, NULL)) {
            ex::throw_last_win32_error();
        }
        wait([](SERVICE_STATUS const &status) {
            return status.dwCurrentState == SERVICE_START_PENDING;
            });
        break;
    case SERVICE_PAUSE_PENDING:
        wait([](SERVICE_STATUS const &status) {
            return status.dwCurrentState == SERVICE_STOP_PENDING;
            });
    case SERVICE_PAUSED:
        if (!ControlService(handle_, SERVICE_CONTROL_CONTINUE, &status)) {
            ex::throw_last_win32_error();
        }
        wait([](SERVICE_STATUS const &status) {
            return status.dwCurrentState == SERVICE_CONTINUE_PENDING;
            });
        break;
    case SERVICE_RUNNING:
    case SERVICE_START_PENDING:
    case SERVICE_CONTINUE_PENDING:
        throw std::logic_error("Cannot load already loaded driver.");
    }
}

void Service::unload() {
    SERVICE_STATUS status = this->status();
    switch (status.dwCurrentState) {
    case SERVICE_START_PENDING:
    case SERVICE_CONTINUE_PENDING:
    case SERVICE_PAUSE_PENDING:
    case SERVICE_PAUSED:
    case SERVICE_RUNNING:
        if (!ControlService(handle_, SERVICE_CONTROL_STOP, &status)) {
            ex::throw_last_win32_error();
        }
    case SERVICE_STOP_PENDING:
        wait([](SERVICE_STATUS const &status) {
            return status.dwCurrentState == SERVICE_STOP_PENDING;
            });
    case SERVICE_STOPPED:
        break;
    }
}

void Service::wait(std::function<bool(SERVICE_STATUS const &status)> continue_predicate) {
    auto start = GetTickCount64();
    SERVICE_STATUS s = status();
    DWORD previous_checkpoint = s.dwCheckPoint;
    while (continue_predicate(s)) {
        auto wait_time = s.dwWaitHint / 10;
        wait_time = max(wait_time, 1000);
        wait_time = min(wait_time, 10000);
        Sleep(wait_time);
        s = status();
        if (s.dwCheckPoint > previous_checkpoint) {
            start = GetTickCount64();
            previous_checkpoint = s.dwCheckPoint;
        } else {
            if (GetTickCount64() - start > s.dwWaitHint) {
                throw std::runtime_error("wait timed out");
            }
        }
    }
}