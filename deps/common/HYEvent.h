#pragma once
#include <functional>
#include <unordered_map>
#include <list>
#include <mutex>
#include "ThreadPool.h"

class CHYEvent;
typedef std::function<void(CHYEvent*)> EventCBFunc;

typedef std::mutex Mutex;
typedef std::lock_guard<Mutex> AutoLock;

class CHYEvent
{
public:
 bool Copy(CHYEvent& event){
        nType = event.nType;
        nDataLen = event.nDataLen;
        if (nDataLen != 0)
        {
            memcpy(this+1, &event+1, nDataLen);
        }
        return true;
    }

public:
    int nType = 0;
    int nDataLen = 0; 
};

class EventMgr
{
private:
    EventMgr();

public:
    static EventMgr* Instance();

public:
    bool RegisterEvent(int nEventType, void* owner, EventCBFunc func);

    bool UnregisterEvent(int nEventType, void* owner);

    bool PostEvent(CHYEvent* pEvent);

    bool OnEvent(CHYEvent* pEvent);

private:
    unsigned long	WorkThreadFunc();

    std::unordered_map<int, std::unordered_map<void*, EventCBFunc>> m_mapEvent;
    Mutex                        m_lockEventMap;

    std::list<CHYEvent*>         m_listEvent;
    Mutex                        m_lockEvent;
    std::condition_variable      m_cond;

    CThreadPool m_pool;
};