#include "NetService.h"
#include <thread>
#include <utility>
#include "AsioServer.h"

CNetService::CNetService()
	: m_nPort(20001)
{
}

CNetService::~CNetService()
{
}

void CNetService::ServerListenThread()
{
	asio::io_context io_context;
	CAsioServer svr(io_context, m_nPort);
	svr.SetCreateUserFunc([this](int nOrigine)->CLinkUser*{
					return CreateUser(nOrigine);
				});
	io_context.run();
}

void CNetService::OnStart()
{
	std::thread th(&CNetService::ServerListenThread, this);
	th.detach();
}

CLinkUser* CNetService::CreateUser(int nOrigin)
{
	return nullptr;
}
