#include "ServiceBase.h"
#include <iostream>
#include <thread>

const int ServiceTickInterval = 50;

int LogInfo(std::string info)
{
    std::cout << info.c_str() << std::endl;
	return 0;
}

CServiceBase::CServiceBase()
	: m_bRunning(false)
{
}

CServiceBase::~CServiceBase()
{
}

bool CServiceBase::Run()
{
	LogInfo("ServiceBase Run");
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

