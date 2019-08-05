#pragma once

#include <filesystem>
#include <string>

#include "scmanager.hpp"

class Application {
public:
    Application();

    int run();

    static bool owns_console();

private:
    std::filesystem::path working_directory_;
    std::wstring program_name_;
};
