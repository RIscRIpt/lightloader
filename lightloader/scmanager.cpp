#include "scmanager.hpp"

#include "win32_error.hpp"

SCManager::SCManager(LPCTSTR machine_name, LPCTSTR database_name, DWORD desired_access)
	: handle_(NULL)
{
	handle_ = OpenSCManager(machine_name, database_name, desired_access);
	if (!handle_) {
		ThrowLastWin32Error();
	}
}

SCManager::SCManager()
	: SCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS)
{}

SCManager::~SCManager() noexcept(false) {
	if (handle_) {
		if (!CloseServiceHandle(handle_)) {
			ThrowLastWin32Error();
		}
		handle_ = NULL;
	}
}
