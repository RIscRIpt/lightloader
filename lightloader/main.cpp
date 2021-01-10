#include <Windows.h>
#include <conio.h>

#include <iostream>
#include <stdexcept>
#include <system_error>

#include "application.hpp"
#include "exit_codes.hpp"
#include "win32_error.hpp"

int wmain() {
    std::ios::sync_with_stdio(false);

    int exit_code = ExitCode::Success;

    try {
        exit_code = Application().run();
    } catch (Win32Error const &e) {
		std::cerr << "System error #" << e.error << '\n';
        exit_code = e.error;
    } catch (std::runtime_error const &e) {
        std::cerr << "Runtime error: " << e.what() << '\n';
        exit_code = ExitCode::RunTimeError;
    } catch (std::exception const &e) {
        std::cerr << "Exception: " << e.what() << '\n';
        exit_code = ExitCode::GenericException;
    }

    if (Application::owns_console()) {
        std::cout << "Press any key to exit ...\n";
        (void)_getch();
    }

    return exit_code;
}
