#include "HYEvent.h"
#include <thread>
#include "HYLog.h"

EventMgr::EventMgr()
{
    m_pool.init(4);

    std::thread th(&EventMgr::WorkThreadFunc, this);
	th.detach();
}

EventMgr* EventMgr::Instance()
{
    static EventMgr instance;
    return &instance;
}

bool EventMgr::RegisterEvent(int nEventType, void* owner, EventCBFunc func)
{
    AutoLock lock(m_lockEventMap);
    if ( m_mapEvent.count(nEventType) == 0 )
    {
        std::unordered_map<void*, EventCBFunc> map;
        map[owner] = func;
        m_mapEvent[nEventType] = map;
    }
    else
    {
        m_mapEvent[nEventType][owner] = func;
    }
    
    return true;    
}

bool EventMgr::UnregisterEvent(int nEventType, void* owner)
{
    AutoLock lock(m_lockEventMap);
    if ( m_mapEvent.count(nEventType) == 0 ) return false;

    if (m_mapEvent[nEventType].count(owner) == 0) return false;
    
    m_mapEvent[nEventType].erase(owner);

    if (m_mapEvent[nEventType].size() == 0)
    {
        m_mapEvent.erase(nEventType);
    }
    
    return true;
}

bool EventMgr::PostEvent(CHYEvent* pEvent)
{
    int nSize = sizeof(CHYEvent) + pEvent->nDataLen;
    char* t = new char[nSize];
    CHYEvent* event = (CHYEvent*)t;
    event->Copy(*pEvent);

    std::unique_lock<std::mutex> locker(m_lockEvent);
    m_listEvent.push_back(event);
    locker.unlock();
    m_cond.notify_one();

    return true;
}

 unsigned long EventMgr::WorkThreadFunc()
 {
    while(true)
    {
        std::unique_lock<std::mutex> locker(m_lockEvent);
        while (m_listEvent.empty())
            m_cond.wait(locker);
        
        CHYEvent* pEvent = m_listEvent.front();
        m_listEvent.pop_front();
        locker.unlock();

        OnEvent(pEvent);
    }
    return 0;
 }

 bool EventMgr::OnEvent(CHYEvent* pEvent)
 {
    AutoLock lock(m_lockEventMap);
    if ( m_mapEvent.count(pEvent->nType) == 0 ) return false;

    auto & map = m_mapEvent[pEvent->nType];

    m_pool.AddTask([=](){
        for (auto itr : map)
        {
            itr.second(pEvent);
        }
        char*e = (char*)pEvent;
        delete []e;
        return true;
    });
    
    return true;
 }