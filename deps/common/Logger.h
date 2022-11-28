#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <mutex>

const int LOG_INTERVAL_CHECK = 50;    // 日志文件定时检测周期
const int LOG_INTERVAL_SAVE = 30000;  // 日志默认保存间隔
const int LOG_BYTE_COUNT_SAVE = 1000; // 日志超过此字节后保存

//typedef std::recursive_mutex Mutex;
typedef std::mutex Mutex;
typedef std::lock_guard<Mutex> AutoLock;

enum FileStatus
{
    NORMAL, // 普通
    FLUSH,  // 下次检测，立即刷新到文件
    CLOSE,  // 下次检测，关闭文件
};

class LogFile
{
public:
    LogFile()
    {
        filePath = "";
        nCacheSize = 0;
        nStatus = NORMAL;
        fileOpenMode = "a";
        lastSaveTime = 0;
    };
public:
    bool WriteText(const std::string strLog);
    bool Flush();
    bool Close();
    bool Check();
    bool Save();
public:
    int                       nStatus;      // 文件状态 FileStatus
    int                       nCacheSize;   // 缓存字节数
    std::string               filePath;     // 文件全路径
    std::string               fileOpenMode; // 文件打开方式，默认追加
    std::vector<std::string>  logs;         // 日志缓存队列
   
    time_t                    lastSaveTime;// 上次保存到文件的tick值
	Mutex                     m_lock;       // 日志锁
};

class CLoggerMgr
{
public:
    CLoggerMgr();
    ~CLoggerMgr();

    bool Init();
public:
   unsigned long	WorkThreadFunc();

   LogFile* CreateFile(std::string filePath);

   bool SaveAll();
private:
    std::unordered_set<LogFile*> m_listFile;
	Mutex                        m_lockListFile;
    time_t                       m_nLastCheckTime;
};

class CLogger
{
public:
    CLogger(void);
    virtual ~CLogger(void);

public:
    bool Init(std::string filePath);              // 提供全路径文件名
	bool WriteText(const std::string &strLog);	  // 直接记录文本
    bool WriteText(const char *szLogFormat, ...); // 直接记录文本
    bool WriteLog(std::string strPre, const char *szLogFormat, ...);  // 默认加时间戳和进程号
    bool Flush();
    bool Close();

private:
   LogFile* m_logFile;
};

