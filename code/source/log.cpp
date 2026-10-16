#include <log.hpp>

#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>

namespace
{
    std::ofstream logFile;
    std::streambuf* old = nullptr;
}

void Log::initialize()
{
    static Log dummy;
}

Log::Log()
{
    auto t = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y-%Hh-%Mm-%Ss");
    std::string str = oss.str();

    const std::filesystem::path directory = "log/execution";
    const std::filesystem::path filename  = directory / (str + "_log.txt");

    std::error_code ec;
    std::filesystem::create_directories(directory, ec);

    logFile.open(filename);
    if (!logFile.is_open())
    {
        std::cerr << "[HEATSTROKE LOG] Could not open " << filename.string() << ", logging to terminal.\n";
        return;
    }

    old = std::clog.rdbuf(logFile.rdbuf());

    printNoTrace("Log succesfully initialized.");
}

Log::~Log()
{
    if (old)
    {
        printNoTrace("Clearing log.");
        std::clog.rdbuf(old);
    }
}
