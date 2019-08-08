#include "application.hpp"

#include <Windows.h>
#include <conio.h>

#include <iostream>
#include <vector>

#include "driver.hpp"
#include "service.hpp"
#include "exceptions.hpp"
#include "exit_codes.hpp"

Application::Application() {
    std::vector<wchar_t> raw_path(32768);
    if (!GetModuleFileName(NULL, raw_path.data(), static_cast<DWORD>(raw_path.size() - 1))) {
        ex::throw_last_win32_error();
    }
    std::filesystem::path path(raw_path.data());
    working_directory_ = path.parent_path();
    program_name_ = path.filename().wstring();
}

int Application::run() {
    if (!program_name_.ends_with(L".exe")) {
        std::wcerr << "The name of this (`" << program_name_ << "`) program must end with `.exe`\n";
        return ExitCode::InvalidProgramName;
    }

    std::wstring driver_name = program_name_.substr(0, program_name_.length() - 4) + L".sys";
    std::filesystem::path driver_path = working_directory_ / driver_name;

    SCManager sc_manager;
    Driver driver(driver_path);
    Service service(sc_manager, driver);

    if (service.is_loaded()) {
        std::wcout << "Driver is loaded.\n";
    } else {
        std::wcout << "Driver is not loaded.\n";
    }
    std::wcout << "Press any key to toggle the driver state.\n"
        << "Press `ESC` to exit.\n";

    while (true) {
        if (_getch() == 27) {
            break;
        }
        std::wcout << '\n';
        if (service.is_loaded()) {
            std::wcout << "Unloading driver `" << driver.name() << "`.\n";
            service.unload();
            std::wcout << "Driver `" << driver.name() << "` has been successfully unloaded.\n";
        } else {
            std::wcout << "Loading driver `" << driver.name() << "`.\n";
            service.load();
            std::wcout << "Driver `" << driver.name() << "` has been successfully loaded.\n";
        }
        std::wcout << "Press any key to toggle the driver state.\n"
            << "Press `ESC` to exit.\n";
    }

    if (service.is_loaded()) {
        service.release();
    }

    return 0;
}

bool Application::owns_console() {
    DWORD pid;
    GetWindowThreadProcessId(GetConsoleWindow(), &pid);
    return GetCurrentProcessId() == pid;
}
