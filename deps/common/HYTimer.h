#pragma once
#include <time.h>
#include <functional>
#include <mutex>
#include <list>
#include <unordered_map>
#include "ThreadPool.h"

class CHYTimer;

// 定时器到期回调函数
typedef std::function<void()> TimerCBFunc;
typedef std::function<void(CHYTimer*)> DefaultTimerCBFunc;


typedef std::mutex Mutex;
typedef std::lock_guard<Mutex> AutoLock;


class CHYTimer
{
public:
    bool Copy(CHYTimer& timer){
        nType = timer.nType;
        nOwner = timer.nOwner;
        tExpTime = timer.tExpTime;
        nDataLen = timer.nDataLen;
        if (nDataLen != 0)
        {
            memcpy(this+1, &timer+1, nDataLen);
        }
        return true;
    }

    void SetTimerMilli(int milli);
    void SetTimer(int sec);

    static void NewTimer(unsigned int millisecond, TimerCBFunc func);

    static CHYTimer* CopyTimer(CHYTimer* timer)
    {
        int nSize = sizeof(CHYTimer) + timer->nDataLen;
        char* t = new char[nSize];
        CHYTimer* pTimer = (CHYTimer*)t;
        pTimer->Copy(*timer);
        return pTimer;
    }
    static void FreeTimer(CHYTimer* pTimer)
    {
        char*t = (char*)pTimer;
        delete []t;
    }
public:
    int    nType   = 0;
    void*  nOwner = nullptr;
    time_t tExpTime = 0;
    int    nDataLen = 0;
    int    nReserve = 0;
};

class TimerMgr
{
private:
    TimerMgr();
    bool Init();

public:
    static TimerMgr* Instance();

public:
    void SetDefaultCallBackFunc(DefaultTimerCBFunc func){m_funcDefaultCB = func;}

    CHYTimer* SetTimer(CHYTimer& timer, TimerCBFunc func = nullptr);

    bool KillTimer(CHYTimer& timer);

private:
    unsigned long	WorkThreadFunc();

    bool DelTimer(CHYTimer* timer); // 从列表中删除Timer

    bool DoFunc(CHYTimer* timer);
    
private:
    std::list<CHYTimer*>         m_listTimer;
	Mutex                        m_lockListTimer;

    DefaultTimerCBFunc           m_funcDefaultCB = nullptr;

    std::unordered_map<CHYTimer*, TimerCBFunc> m_mapFunc;

    CThreadPool m_pool;
};