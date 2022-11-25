#include "LinkUser.h"

CLinkUser::CLinkUser()
	: m_funcSendMsg(nullptr)
{
}

CLinkUser::~CLinkUser()
{
}

bool CLinkUser::SendMsg(char* msg, int nLen)
{
	if (m_funcSendMsg)
	{
		return m_funcSendMsg(msg, nLen);
	}
	return false;
}

bool CLinkUser::BindSendMsgFunc(SendMsgFunc func)
{
	m_funcSendMsg = func;
	return true;
}

bool CLinkUser::OnMsg(char * msg, int nLen)
{
	return false;
}

bool CLinkUser::OnBreak()
{
	return false;
}