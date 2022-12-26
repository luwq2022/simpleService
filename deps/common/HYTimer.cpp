#include "HYTimer.h"
#include <thread>
#include <math.h>
#include <iostream>
#include "HYFunc.h"

const int TIMER_INTERVAL_CHECK = 50;


void CHYTimer::NewTimer(unsigned int millisecond, TimerCBFunc func)
{
    CHYTimer timer;
    timer.tExpTime = GetTimeStamp() + millisecond*1000;
    TimerMgr::Instance()->SetTimer(timer, func);
}

void CHYTimer::SetTimerMilli(int milli)
{
    tExpTime = GetTimeStamp() + milli*1000;
}
void CHYTimer::SetTimer(int sec)
{
    tExpTime = GetTimeStamp() + sec * 1000 *1000;
}

TimerMgr::TimerMgr()
{
    Init();
}

TimerMgr* TimerMgr::Instance()
{
    static TimerMgr instance;
    return &instance;
}

bool TimerMgr::Init()
{
    m_pool.init(4);

    std::thread th(&TimerMgr::WorkThreadFunc, this);
	th.detach();
    return true;
}

bool compareTimer(CHYTimer* timer1, CHYTimer* timer2)
{
    return timer1->tExpTime < timer2->tExpTime;
}

CHYTimer* TimerMgr::SetTimer(CHYTimer& timer, TimerCBFunc func)
{
    //std::cout << "set timer " << timer.tExpTime  <<std::endl;
    
    CHYTimer* pTimer = CHYTimer::CopyTimer(&timer);
    {
        AutoLock lock(m_lockListTimer);
        m_listTimer.push_front(pTimer);
        m_mapFunc[pTimer] = func;
        m_listTimer.sort(compareTimer);
    }
    
    return pTimer;
}

bool TimerMgr::KillTimer(CHYTimer& timer)
{
    return DelTimer(&timer);
}

bool TimerMgr::DelTimer(CHYTimer* timer)
{
    //std::cout << "del timer " << timer->tExpTime << std::endl;
    AutoLock lock(m_lockListTimer);
    auto itr = std::find_if(m_listTimer.begin(), m_listTimer.end(), [&](CHYTimer* pTimer){
        return (timer->nOwner == pTimer->nOwner && timer->nType == pTimer->nType);
    });

    if (itr != m_listTimer.end())
    {
        CHYTimer* pTimer = (CHYTimer*)*itr;
        m_listTimer.erase(itr);
        m_mapFunc.erase(pTimer);
        CHYTimer::FreeTimer(pTimer);
        return true;
    }
    return false;
}

unsigned long TimerMgr::WorkThreadFunc()
{
    while (true)
    {
        int nListSize = 0;
        {
            AutoLock lock(m_lockListTimer);
            nListSize = m_listTimer.size();
        }
        
        if (nListSize == 0)
        {
            // 检测没有定时器，直接sleep 50ms再检测
            std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_INTERVAL_CHECK));
            continue;
        }
        time_t tNow = GetTimeStamp();
        //std::cout << "now " << tNow << " size " << nListSize << std::endl;
        // 取第一个定时器查看，是否到期
        CHYTimer* pTimer = nullptr;
        {
            AutoLock lock(m_lockListTimer);
            pTimer = m_listTimer.front();
        }
        if (pTimer)
        {
           if (tNow >= pTimer->tExpTime) //定时器过期,执行定时器回调，删除定时器
            {
                DoFunc(pTimer);
                DelTimer(pTimer); // 删除这个timer  
            }
            else // 定时器还没到，看到期时间和50ms哪个更小，sleep较小的时间
            {
                int nSpleep = std::min((int)(pTimer->tExpTime - tNow) / 1000, TIMER_INTERVAL_CHECK) ;
                std::this_thread::sleep_for(std::chrono::milliseconds(nSpleep));
            }
        }        
    }
    return 0;
}

// 后续优化使用线程池执行
bool TimerMgr::DoFunc(CHYTimer* timer)
{
    TimerCBFunc func = nullptr;
    {
        AutoLock lock(m_lockListTimer);
        func = m_mapFunc[timer];
    }

    if (func)
    {
        m_pool.AddTask(func);
    }
    else if(m_funcDefaultCB)
    {
        CHYTimer *pTimer = CHYTimer::CopyTimer(timer);
         m_pool.AddTask([=](){
            m_funcDefaultCB(pTimer);
            CHYTimer::FreeTimer(pTimer);
         });
    }
    
    return true;
}