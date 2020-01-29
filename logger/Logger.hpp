#pragma once
#include <string>
#include <mutex>
#include <fstream>

#define LOG(x) Logger::getInstance().Log(x);
#define LOG_WARNING(x) LOG(std::string("Warning: ") + x);

class Logger
{
public:
    static Logger& getInstance();
    void Log(const std::string& data);

private:
    std::mutex mMutex;
    std::ofstream mFile;
    Logger();
};

class LogFunc
{
public:
    LogFunc(const std::string& funcName);
    ~LogFunc();

private:
    const std::string mFuncName;
};

#ifndef __FUNCTION_NAME__
#ifdef WIN32   //WINDOWS
#define __FUNCTION_NAME__   __FUNCTION__  
#else          //*NIX
#define __FUNCTION_NAME__   __func__ 
#endif
#endif

#define LOG_FUNC LogFunc __lgfnc(__FUNCTION_NAME__);