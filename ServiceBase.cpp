#include "ServiceBase.h"
#include <iostream>
#include <thread>
#include "HYLog.h"
#include "HYTimer.h"
#include "HYEvent.h"
#include "HYFunc.h"

const int ServiceTickInterval = 50;

const int ServiceTimeOutInterval = 30 * 1000;

const int ServiceEventID = 2;

CServiceBase::CServiceBase()
	: m_bRunning(false)
{
	m_logName = "./ServiceBase.log";
}

CServiceBase::~CServiceBase()
{
}

void CServiceBase::SetLogName(std::string name)
{
	m_logName = name;
}

bool CServiceBase::Run()
{
	LogInfo("ServiceBase Run %s", "");
	static time_t lastTime = GetTimeMilli();
	while (m_bRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ServiceTickInterval));
		OnTick();
		auto tNow = GetTimeMilli();
		if (tNow - lastTime >= ServiceTimeOutInterval)
		{
			lastTime += ServiceTimeOutInterval;
			OnTimeOut(tNow);
		}
	}
	return true;
}

bool CServiceBase::Start()
{
	LOGINIT(m_logName);
	LogInfo("");
	LogInfo("ServiceBase Start ...");
	m_bRunning = true;
	TimerMgr::Instance()->SetDefaultCallBackFunc([&](CHYTimer * pTimer){
		OnTimer(pTimer);
	});
	
	EventMgr::Instance()->RegisterEvent(ServiceEventID, this, [&](CHYEvent* event){
		OnEvent(event);
	});
	
	OnStart();
	Run();
	return false;
}

void CServiceBase::Stop()
{
	LogInfo("ServiceBase Stop");
	m_bRunning = false;
}

void CServiceBase::OnStart()
{
	LogInfo("ServiceBase OnStart");
}

void CServiceBase::OnTick()
{
	//LogInfo("ServiceBase Tick");
}

void CServiceBase::OnTimeOut(time_t tNow)
{

}

void CServiceBase::OnStop()
{
	LogInfo("ServiceBase OnStop");
}

void CServiceBase::SetTimer(CHYTimer * pTimer)
{
	TimerMgr::Instance()->SetTimer(*pTimer);
}

void CServiceBase::KillTimer(CHYTimer * pTimer)
{
	TimerMgr::Instance()->KillTimer(*pTimer);
}

void CServiceBase::OnTimer(CHYTimer * pTimer)
{
	switch (pTimer->nType)
	{
	case 1:
		LogInfo("this is Service Timer " );
		break;
	
	default:
		break;
	}
}

void CServiceBase::PostEvent(CHYEvent* pEvent)
{
	EventMgr::Instance()->PostEvent(pEvent);
}

void CServiceBase::OnEvent(CHYEvent* pEvent)
{
	switch (pEvent->nType)
	{
	case ServiceEventID:
		
		break;
	
	default:
		break;
	}
}