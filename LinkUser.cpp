#include "LinkUser.h"
#include "HYLog.h"

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
	LogError("CLinkUser::SendMsg failed! can not find send func");
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