#pragma once
#include <functional>
#include "stdint.h"

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

	void SetSessionID(int64_t id){m_sesID = id;};

protected:
	SendMsgFunc m_funcSendMsg;
	int64_t m_sesID;
};

