#include "Logger.h"
#include <stdarg.h>
#include <thread>
#include <chrono>
#include "HYBuffer.h"
#include "HYFunc.h"

static CLoggerMgr s_logMgr;



CLoggerMgr::CLoggerMgr()
{
    Init();
}

CLoggerMgr::~CLoggerMgr()
{
   SaveAll();
}

bool CLoggerMgr::Init()
{
	std::thread th(&CLoggerMgr::WorkThreadFunc, this);
	th.detach();
    m_nLastCheckTime = 0;
    return true;
}

unsigned long CLoggerMgr::WorkThreadFunc()
{
    while (true)
    {
        time_t tNow = GetTimeStamp();
        if (tNow - m_nLastCheckTime > LOG_INTERVAL_CHECK)
        {
            // check
            m_nLastCheckTime = tNow;
			AutoLock lock(m_lockListFile);
            for (auto itr = m_listFile.begin(); itr != m_listFile.end();)
            {
                (*itr)->Check();
                if ((*itr)->nStatus == CLOSE)
                { 
                    delete (*itr);
                    itr = m_listFile.erase(itr);
                }
                else
                {
                    itr++;
                }
            }
        }
        else
        {
            int nSpleep = m_nLastCheckTime + LOG_INTERVAL_CHECK - tNow;
			std::this_thread::sleep_for(std::chrono::milliseconds(nSpleep));
        }
    }
    return 0;
}

LogFile* CLoggerMgr::CreateFile(std::string filePath)
{
    LogFile* file = new (std::nothrow) LogFile();
    if (file == nullptr)
    {
        return nullptr;
    }
    file->filePath = filePath;

	AutoLock lock(m_lockListFile);
    m_listFile.insert(file);
    return file;
}

bool CLoggerMgr::SaveAll()
{
	AutoLock lock(m_lockListFile);
    for (auto itr = m_listFile.begin(); itr != m_listFile.end();)
    {
        (*itr)->Save();
        itr++;
    }
    return true;
}

CLogger::CLogger(void)
    : m_logFile(nullptr)
{
}


CLogger::~CLogger(void)
{
    Close();
}

bool CLogger::Init(std::string filePath)
{
    if (m_logFile == nullptr)
    {
        m_logFile = s_logMgr.CreateFile(filePath);
        if(m_logFile)return true;
    }
    return false;
}


bool CLogger::WriteText(const std::string &strLog)
{
	return  m_logFile->WriteText(strLog);
}


bool CLogger::WriteText(const char *szLogFormat, ...)
{
    if (!m_logFile) return false;

    CHYBuffer bufLog;

    va_list arglist;
    va_start(arglist, szLogFormat);
    bool bRet = bufLog.AppendFormatStringV(szLogFormat,arglist);
    va_end(arglist);

    if (!bRet) return false;
    
    return  m_logFile->WriteText(bufLog.GetBufPtr());
}
# include <sstream>
#include <iostream>
bool CLogger::WriteLog(std::string strPre, const char *szLogFormat, ...)
{
    if (!m_logFile) return false;

	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	struct tm tm;
#ifdef _WIN32
	localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

	struct tm* ptm = &tm;
	char timeStr[128] = { 0 };
    snprintf(timeStr,128, "%02d/%02d/%02d %02d:%02d:%02d %08X ", 
		(int)ptm->tm_mon + 1,
		(int)ptm->tm_mday,
        ((int)ptm->tm_year) % 100,
		(int)ptm->tm_hour,
		(int)ptm->tm_min,
		(int)ptm->tm_sec,
		std::this_thread::get_id()
        );

    CHYBuffer bufLog;
    bufLog.AppendText(timeStr);
    if (strPre.length() > 0)
    {
        bufLog.AppendText(strPre.c_str());
    }

    va_list arglist;
    va_start(arglist, szLogFormat);
    bool bRet = bufLog.AppendFormatStringV(szLogFormat,arglist);
    va_end(arglist);

    if (!bRet) return false;

    return  m_logFile->WriteText(bufLog.GetBufPtr());
}

bool CLogger::Flush()
{
    if (!m_logFile) return false;
    
    return m_logFile->Flush();
}


bool CLogger::Close()
{
    if (!m_logFile) return false;
    m_logFile->Close();
    m_logFile = nullptr;
    return true;
}

bool LogFile::WriteText(const std::string strLog)
{
	AutoLock lock(m_lock);
    if (nStatus == CLOSE) return false;
    
    logs.push_back(strLog);
    nCacheSize += strLog.length();
    return true;
}

bool LogFile::Flush()
{
	AutoLock lock(m_lock);
    if (nStatus == CLOSE) return false;

    nStatus = FLUSH;
    return true;
}

bool LogFile::Close()
{
	AutoLock lock(m_lock);
    nStatus = CLOSE;
    return true;
}

bool LogFile::Check()
{
	int nStatusTmp = 0;
	int nCacheSizeTmp = 0;
	time_t tLastSaveTimeTmp = 0;
	{
		AutoLock lock(m_lock);
		nStatusTmp = nStatus;
		nCacheSizeTmp = nCacheSize;
		tLastSaveTimeTmp = lastSaveTime;
	}
	
    if (nStatusTmp == FLUSH || nStatusTmp == CLOSE)
    {
        return Save();
    }

    if (nCacheSizeTmp >= LOG_BYTE_COUNT_SAVE)
    {
        return Save();
    }
 
    if (GetTimeStamp() - tLastSaveTimeTmp >= LOG_INTERVAL_SAVE)
    {
        return Save();
    }
    return true;
}

bool LogFile::Save()
{
	AutoLock lock(m_lock);
    lastSaveTime = GetTimeStamp();
    FILE *pf;

    //errno_t err = fopen_s(&pf, filePath.c_str(), fileOpenMode.c_str());
    pf = fopen( filePath.c_str(), fileOpenMode.c_str());
    if (pf == nullptr)
    {
        return false;
    }

    for (unsigned int i = 0; i < logs.size(); ++i)
    {
        fputs(logs[i].c_str(), pf);
        fputs("\n", pf);
    }
    logs.clear();
    nCacheSize = 0;
    if(nStatus == FLUSH) nStatus = NORMAL;
    fclose(pf);

    return true;
}