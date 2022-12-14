#pragma once
#define ASIO_STANDALONE
//https://think-async.com/Asio/
#include "asio.hpp"
#include "TCPSession.h"
#include <iostream>
#include "HYLog.h"

class CAsioServer
{
public:
	CAsioServer(asio::io_context& io_context, int nPort)
		: m_acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), nPort))
	{
		std::cout << "Asio server construct, listen port " << nPort <<  std::endl;
		LogInfo("Service Listen Port %d", nPort);
		OnAccept();
	}

	void SetCreateUserFunc(CreateFunc func){m_CreateUserFunc = func;}

private:
	void OnAccept()
	{
		m_acceptor.async_accept([&](std::error_code ec, asio::ip::tcp::socket socket) {
			if (!m_acceptor.is_open())
			{
				LogWarn("Service Accept failed! acceptor is not open.");
				return;
			}

			if (!ec)
			{
				LogTrace("Service accept a new session");
				CTCPSession* pSession = new CTCPSession(socket);
				pSession->SetCreateUserFunc([this](int nOrigine)->CLinkUser*{
					if(m_CreateUserFunc){
						return m_CreateUserFunc(nOrigine);
					}
					return nullptr;
				});
				pSession->Start();
			}

			OnAccept();
		});
	}
private:
	asio::ip::tcp::acceptor m_acceptor;
	CreateFunc m_CreateUserFunc;
};

