#pragma once
#include <functional>

typedef std::function<bool(char* msg, int nLen)> SendMsgFunc ;

class CLinkUser
{
public:
	CLinkUser();
	virtual ~CLinkUser();

public:
	virtual bool SendMsg(char* msg, int nLen);

	virtual bool OnMsg(char* msg, int nLen);

	virtual bool OnBreak();

	bool BindSendMsgFunc(SendMsgFunc func);

protected:
	SendMsgFunc m_funcSendMsg;
};

