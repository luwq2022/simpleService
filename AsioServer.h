#pragma once
#define ASIO_STANDALONE
//https://think-async.com/Asio/
#include "asio.hpp"
#include "TCPSession.h"
#include <iostream>
class CAsioServer
{
public:
	CAsioServer(asio::io_context& io_context, int nPort)
		: m_acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), nPort))
	{
		std::cout << "Asio server construct" << std::endl;
		OnAccept();
	}

	void SetCreateUserFunc(CreateFunc func){m_CreateUserFunc = func;}

private:
	void OnAccept()
	{
		m_acceptor.async_accept([&](std::error_code ec, asio::ip::tcp::socket socket) {
			if (!m_acceptor.is_open())
			{
				return;
			}

			if (!ec)
			{
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

