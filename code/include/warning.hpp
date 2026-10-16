#pragma once
#include <cassert>
#include <source_location>
#include <string_view>

#define compileTimeAssertion(b, ...) static_assert(b, ##__VA_ARGS__)

#define DEBUG_MODE

#ifdef DEBUG_MODE

namespace Bsp
{
    void warning_imp(std::string_view message = "",
                     std::source_location location = std::source_location::current());

    void runTimeAssertion_imp(bool b, const char* expr, std::string_view message = "",
                              std::source_location location = std::source_location::current());
}

#define warning(...) Bsp::warning_imp(__VA_ARGS__)    // Warning will perform a log warning whetever is reached by the code

#define runTimeAssertion(b, ...) Bsp::runTimeAssertion_imp(static_cast<bool>(b), #b, ##__VA_ARGS__) // runTimeAssertion will perform a debug assertion and stop the execiuton if fails.

#define debugAssertion(b, ...)  Bsp::runTimeAssertion_imp(static_cast<bool>(b), #b, ##__VA_ARGS__) // debugAssertion will perform a debug assertion and stop the execiuton if fails.

#endif

#ifndef DEBUG_MODE

#define warning(...) static_cast<void>(0)

#define runTimeAssertion(b, ...) static_cast<void>(static_cast<bool>(b)) // When the boolen expresion stills needs to be evaluated.

#define debugAssertion(b, ...) static_cast<void>(0) // debugAssertion will not prevel in production mode


#endif
