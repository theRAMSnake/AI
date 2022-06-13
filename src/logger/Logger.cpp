#include "Logger.hpp"
#include <ctime>
#include <iomanip>

Logger& Logger::getInstance()
{
    static Logger l;
    return l;
}

void Logger::Log(const std::string& data)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    mMutex.lock();
    mFile << '[' << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "]   " << data << std::endl;
    mFile.flush();
    mMutex.unlock();
}

Logger::Logger()
{
    mFile.open("Logger.log", std::ofstream::out | std::ofstream::trunc);
}

LogFunc::LogFunc(const std::string& funcName)
    : mFuncName(funcName)
{
    LOG(mFuncName + " ENTER");
}

LogFunc::~LogFunc()
{
    LOG(mFuncName + " LEAVE");
}