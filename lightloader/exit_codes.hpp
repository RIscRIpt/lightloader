#pragma once

enum ExitCode {
    Success = 0,
    GenericException = -1,
    RunTimeError = -2,

    InvalidProgramName = 1,
    DriverNotFound = 2,
};
