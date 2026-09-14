#include <warning.hpp>
#include <cstdlib>
#include <iostream>

namespace Bsp
{
    void warning_imp(const char* file, const char* function, int line)
    {
        std::cout << "[WARNING] " << file << ":" << line << " (" << function << ")" << "\n";
    }

    void runTimeAssertion_imp(bool b, const char* expr, const char* file, const char* function, int line)
    {
        if(!b)
        {
            std::cout << "[ASSERTION FAILED] (" << expr << ") " << file << ":" << line << " (" << function << ")" << "\n";
            std::exit(1);
        }
    }
}
