#pragma once
#include <functional>
#include "asio.hpp"
#include "HYBuffer.h"
class CLinkUser;

typedef std::function<CLinkUser*(int nOrigin)> CreateFunc ;

class CTCPSession
{
public:
	CTCPSession(asio::ip::tcp::socket& sock);
	
public:
	asio::ip::tcp::socket& Socket() { return m_socket; }

	void Start();

	void Read();

	bool Send(char* msg, int nLength);
	
private:
	bool BindUser(CLinkUser* user);

	bool CreateUser();
	
	bool HandleMsg(std::error_code ec, std::size_t length);

	void CloseSession();
	
public:
	void SetCreateUserFunc(CreateFunc func){m_CreateUserFunc = func;}

private:
	asio::ip::tcp::socket m_socket;
	enum { max_length = 1024 };
	char m_szData[max_length];
	CLinkUser* m_pUser;
	CreateFunc m_CreateUserFunc;
	CHYBuffer m_buf;
	int64_t m_sesID = 0;
};

