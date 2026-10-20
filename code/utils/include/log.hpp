#pragma once

#include <iostream>
#include <source_location>
#include <string_view>


namespace HeatStroke
{
    /*
    * Log will provide the utility to print and debug into de clog buffer, saving the report
    */
    struct Log
    {

        static void initialize();

        ~Log();

        static void printTrace(const std::string_view message,
                const std::source_location location = 
                        std::source_location::current());

        static void print(const std::string_view message);

        Log(const Log& rhs) = delete;
        const Log& operator=(const Log& rhs) = delete;

        private:
            Log();
    };
}

// HS_DEBUG_MODE is defined by CMake in Debug builds. In Release the macros expand to nothing,
// so their arguments (e.g. a std::format) are not even evaluated.
#ifdef HS_DEBUG_MODE

#define logInit()       HeatStroke::Log::initialize()   // Redirects the log to a file of this execution
#define logPrint(msg)   HeatStroke::Log::print(msg)     // Prints the message
#define logTrace(msg)   HeatStroke::Log::printTrace(msg) // Prints the message with the location of the call

#else

#define logInit()       static_cast<void>(0)
#define logPrint(msg)   static_cast<void>(0)
#define logTrace(msg)   static_cast<void>(0)

#endif

