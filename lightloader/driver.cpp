#include "driver.hpp"

#include <stdexcept>
#include <sstream>

Driver::Driver(std::filesystem::path const &driver_path)
    : path_(driver_path)
{
    if (!path_.wstring().ends_with(L".sys")) {
        throw std::runtime_error("Driver name must end with `.sys`.");
    }
    if (!std::filesystem::exists(path_)) {
        std::stringstream wss;
        wss << "Driver " << driver_path << " was not found.";
        throw std::runtime_error(wss.str());
    }
    name_ = path_.filename().wstring();
    name_ = name_.substr(0, name_.length() - 4);
}
