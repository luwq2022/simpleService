#pragma once


class CServiceBase
{
public:

	CServiceBase();
	virtual ~CServiceBase();

public:

	virtual bool Run();

	virtual bool Start();

	virtual void Stop();

public:
	virtual void OnStart();

	virtual void OnTick();

	virtual void OnStop();
    
protected:
	bool m_bRunning;
};