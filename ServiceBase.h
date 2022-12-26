#pragma once
#include "time.h"
#include <string>

class CHYTimer;
class CHYEvent;
class CServiceBase
{
public:

	CServiceBase();
	virtual ~CServiceBase();

public:
	virtual void SetLogName(std::string name);

	virtual bool Run();

	virtual bool Start();

	virtual void Stop();

public:
	virtual void OnStart();

	// 每隔50ms执行一次得tick函数
	virtual void OnTick();

	// 每隔30秒执行一次的超时函数
	virtual void OnTimeOut(time_t tNow);

	virtual void OnStop();

public:
	virtual void SetTimer(CHYTimer * pTimer);

	virtual void KillTimer(CHYTimer * pTimer);

	virtual void OnTimer(CHYTimer * pTimer);
    
public:
	virtual void PostEvent(CHYEvent* pEvent);

	virtual void OnEvent(CHYEvent* pEvent);

protected:
	bool m_bRunning;

	std::string m_logName;
};