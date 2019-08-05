#include "scmanager.hpp"

#include "exceptions.hpp"

SCManager::SCManager(LPCTSTR machine_name, LPCTSTR database_name, DWORD desired_access)
    : handle_(NULL)
{
    handle_ = OpenSCManager(machine_name, database_name, desired_access);
    if (!handle_) {
        ex::throw_last_win32_error();
    }
}

SCManager::SCManager()
    : SCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)
{}

SCManager::~SCManager() noexcept(false) {
    if (handle_) {
        if (!CloseServiceHandle(handle_)) {
            ex::throw_last_win32_error();
        }
        handle_ = NULL;
    }
}
