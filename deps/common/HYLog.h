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
    FATAL = 1,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TRACE,
};

const int nLogLevel = TRACE;

#define LOGINIT(fileName)  Log::GetInstance()->Init(fileName);
#define HYLOG(szLogFormat, ...) {Log::GetInstance()->logger.WriteLog("", szLogFormat,##__VA_ARGS__);Log::GetInstance()->logger.Flush();}
//
#define LogTrace(szLogFormat, ...) if(nLogLevel >= LogLevel::TRACE){Log::GetInstance()->logger.WriteLog("[Trace]", szLogFormat,##__VA_ARGS__);}
#define LogDebug(szLogFormat, ...) if(nLogLevel >= LogLevel::DEBUG){Log::GetInstance()->logger.WriteLog("[Debug]", szLogFormat,##__VA_ARGS__);}
#define LogInfo(szLogFormat, ...) if(nLogLevel >= LogLevel::INFO){Log::GetInstance()->logger.WriteLog("[Info]", szLogFormat,##__VA_ARGS__);}
#define LogWarn(szLogFormat, ...) if(nLogLevel >= LogLevel::WARN){Log::GetInstance()->logger.WriteLog("[Warn]", szLogFormat,##__VA_ARGS__);}
#define LogError(szLogFormat, ...) if(nLogLevel >= LogLevel::ERROR){Log::GetInstance()->logger.WriteLog("[Error]", szLogFormat,##__VA_ARGS__);}
#define LogFatal(szLogFormat, ...) if(nLogLevel >= LogLevel::FATAL){Log::GetInstance()->logger.WriteLog("[Fatal]", szLogFormat,##__VA_ARGS__);}