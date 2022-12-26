#pragma once
#include "Logger.h"

class Log{

private:
    Log(){};

public:
    static Log* GetInstance()
    {
        static Log instance;
        return &instance;
    }

    bool Init(std::string fileName)
    {
        logger.Init(fileName);
        return true;
    }

public:
    CLogger logger;
};

enum LogLevel{
    LL_FATAL = 1,
    LL_ERROR,
    LL_WARN,
    LL_INFO,
    LL_DEBUG,
    LL_TRACE,
};

const int nLogLevel = LL_TRACE;

#define LOGINIT(fileName)  Log::GetInstance()->Init(fileName);
#define HYLOG(szLogFormat, ...) {Log::GetInstance()->logger.WriteLog("", szLogFormat,##__VA_ARGS__);Log::GetInstance()->logger.Flush();}
//
#define LogTrace(szLogFormat, ...) if(nLogLevel >= LogLevel::LL_TRACE){Log::GetInstance()->logger.WriteLog("[Trace]", szLogFormat,##__VA_ARGS__);}
#define LogDebug(szLogFormat, ...) if(nLogLevel >= LogLevel::LL_DEBUG){Log::GetInstance()->logger.WriteLog("[Debug]", szLogFormat,##__VA_ARGS__);}
#define LogInfo(szLogFormat, ...) if(nLogLevel >= LogLevel::LL_INFO){Log::GetInstance()->logger.WriteLog("[Info]", szLogFormat,##__VA_ARGS__);}
#define LogWarn(szLogFormat, ...) if(nLogLevel >= LogLevel::LL_WARN){Log::GetInstance()->logger.WriteLog("[Warn]", szLogFormat,##__VA_ARGS__);}
#define LogError(szLogFormat, ...) if(nLogLevel >= LogLevel::LL_ERROR){Log::GetInstance()->logger.WriteLog("[Error]", szLogFormat,##__VA_ARGS__);}
#define LogFatal(szLogFormat, ...) if(nLogLevel >= LogLevel::LL_FATAL){Log::GetInstance()->logger.WriteLog("[Fatal]", szLogFormat,##__VA_ARGS__);}