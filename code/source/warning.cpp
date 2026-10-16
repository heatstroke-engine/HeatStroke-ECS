#include <warning.hpp>
#include <cstdlib>
#include <iostream>
#include <log.hpp>

namespace Bsp
{
    void warning_imp(std::string_view message, std::source_location location)
    {
        std::clog << "[WARNING] ";
        Log::printTrace(message, location);
    }

    void runTimeAssertion_imp(bool b, const char* expr, std::string_view message, std::source_location location)
    {
        if(!b)
        {
            std::clog << "[ASSERTION FAILED] (" << expr << ") ";
            Log::printTrace(message, location);
            std::exit(1);
        }
    }
}
