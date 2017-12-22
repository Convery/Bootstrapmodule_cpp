/*
    Initial author: Convery (tcn@ayria.se)
    Started: 22-12-2017
    License: MIT

    A single header for all modules used.
*/

#pragma once

// The configuration settings.
#include "Configuration/Defines.hpp"

// Standard libraries.
#include <string_view>
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <string>
#include <mutex>

// Platformspecific libraries.
#if defined(_WIN32)
    #include <Windows.h>
    #include <direct.h>
    #include <intrin.h>
    #undef min
    #undef max
#else
    #include <sys/stat.h>
#endif

// Utility modules.
#include "Utility/Variadicstring.hpp"
#include "Utility/Logfile.hpp"