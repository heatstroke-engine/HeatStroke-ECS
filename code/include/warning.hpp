#pragma once
#include <cassert>

//#define DEBUG_MODE

#ifdef DEBUG_MODE

namespace Bsp
{
    void warning_imp(const char* file, const char* function, int line);

    void runTimeAssertion_imp(bool b, const char* expr, const char* file, const char* function, int line);
}

#define warning() Bsp::warning_imp(__FILE__, __func__, __LINE__)

#define runTimeAssertion(b) Bsp::runTimeAssertion_imp(static_cast<bool>(b), #b, __FILE__, __func__, __LINE__)

#define compileTimeAssertion(b, ...) static_assert(b, ##__VA_ARGS__)

#endif

#ifndef DEBUG_MODE

#define warning() static_cast<void>(0)

#define runTimeAssertion(b) static_cast<void>(static_cast<bool>(b))

#define compileTimeAssertion(b, ...) static_cast<void>(static_cast<bool>(b))


#endif
