#include "ServiceBase.h"
#include <iostream>
#include <thread>
#include "HYLog.h"

const int ServiceTickInterval = 50;

CServiceBase::CServiceBase()
	: m_bRunning(false)
{
	LOGINIT("./ServiceBase.log");
}

CServiceBase::~CServiceBase()
{
}

bool CServiceBase::Run()
{
	LogInfo("ServiceBase Run %s", "...");
	while (m_bRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ServiceTickInterval));
		OnTick();
	}
	return true;
}

bool CServiceBase::Start()
{
	LogInfo("ServiceBase Start");
	m_bRunning = true;
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

void CServiceBase::OnStop()
{
	LogInfo("ServiceBase OnStop");
}

