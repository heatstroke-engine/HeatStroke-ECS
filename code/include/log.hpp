#pragma once

#include <iostream>
#include <source_location>
#include <string_view>

struct Log
{

    static void initialize();

    ~Log();

    inline static void printTrace(const std::string_view message,
            const std::source_location location = 
                    std::source_location::current())
    {
        std::clog << "file: "
            << location.file_name() << '('
            << location.line() << ':'
            << location.column() << ") `"
            << location.function_name() << "`: "
            << message << '\n';
    }

    inline static void printNoTrace(const std::string_view message)
    {
        std::clog << "[HEATSTROKE LOG] " << message << '\n';
    }

    Log(const Log& rhs) = delete;
    const Log& operator=(const Log& rhs) = delete;

    private:
        Log();
};
