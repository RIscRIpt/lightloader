#pragma once

#include "scmanager.hpp"

#include <filesystem>

class Driver {
public:
    Driver(std::filesystem::path const &driver_path);

    inline std::filesystem::path const& path() const { return path_; }
    inline std::wstring const& name() const { return name_; }

private:
    std::filesystem::path path_;
    std::wstring name_;
};
