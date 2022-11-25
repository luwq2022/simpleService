#pragma once
#include "ServiceBase.h"

class CLinkUser;

class CNetService :
    public CServiceBase
{
public:
    CNetService();
    virtual ~CNetService();

public:
	virtual void ServerListenThread();
    virtual void OnStart();

public:
    virtual CLinkUser* CreateUser(int nOrigin);

protected:
    int m_nPort;
};
