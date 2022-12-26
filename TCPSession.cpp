#include "TCPSession.h"
#include "LinkUser.h"
#include "Protocol.h"
#include "HYLog.h"
#include <atomic>

const int MAX_MSG_DATA_LEN = 64 * 1024;
std::atomic<int64_t> s_sessionID = 0;

CTCPSession::CTCPSession(asio::ip::tcp::socket& sock)
		: m_socket(std::move(sock))
		, m_pUser(nullptr)
		, m_CreateUserFunc(nullptr)
{
	m_sesID = (++s_sessionID);
}

void CTCPSession::Start()
{
	Read();
}

void CTCPSession::CloseSession()
{
	m_socket.close();
	if (m_pUser)
	{
		m_pUser->OnBreak();
		delete m_pUser;
		m_pUser = nullptr;
	}
}

bool CTCPSession::CreateUser()
{
	PHYHEADER pHeader = (PHYHEADER)m_szData;
	unsigned int nOrigine = 0;
	if (pHeader != nullptr)
	{
		nOrigine = pHeader->wOrigine;
	}
	if (nOrigine != 0)
	{
		CLinkUser* pUser = nullptr;
		if(m_CreateUserFunc)
		{
			pUser = m_CreateUserFunc(nOrigine);
		}
		if (pUser)
		{
			BindUser(pUser);
			pUser->BindSendMsgFunc([&](char* msg, int nLen)->bool{
				Send(msg, nLen);
			});
			pUser->SetSessionID(m_sesID);
		}
	}

	return m_pUser ? true : false;
}

bool CTCPSession::HandleMsg(std::error_code ec, std::size_t length)
{
	if (!ec)
	{
		//LogTrace("Session %lld Recv msg data %d", m_sesID, length);
		if(m_pUser == nullptr)
		{
			if (length < HYHEADERSIZE)
			{
				LogWarn("Session %lld Recv msg data %d, too short", m_sesID, length);
				return false;
			}
			
			CreateUser();
		}

		if (m_pUser == nullptr) 
		{
			LogWarn("Session %lld Recv msg data %d, create user failed", m_sesID, length);
			return false;
		}
		
		int nOffset = 0;
		while (nOffset < length)
		{
			if (m_buf.GetBufLen() == 0)
			{
				if (nOffset + HYHEADERSIZE > length) // 无法取出一个Header
				{
					if (length < max_length) // 本次缓冲区未用完
					{
						CloseSession();
					}
					else // 缓冲区已经用完，剩余Header需要下次获取
					{
						m_buf.Append(m_szData + nOffset, length - nOffset);
					}
					return false;
				}
				else // 取出一个Header
				{
					m_buf.Append(m_szData + nOffset, HYHEADERSIZE);
					nOffset += HYHEADERSIZE;
				}				
			}
			else if (m_buf.GetBufLen() < HYHEADERSIZE) // 缓存内容小于一个Header，可能是上次没有读完，先读完一个Header
			{
				int nNeedLen = HYHEADERSIZE - m_buf.GetBufLen();
				if(nOffset + nNeedLen <= length && nNeedLen > 0)
				{
					m_buf.Append(m_szData + nOffset, nNeedLen);
					nOffset += nNeedLen;
				}
			}

			if (m_buf.GetBufLen() < HYHEADERSIZE) // 此时仍不足一个Header,异常返回
			{
				return false;
			}
						
			PHYHEADER pHeader = (PHYHEADER)m_buf.GetBufPtr();
			DWORD dwLength = pHeader->dwLength;
			if (dwLength > MAX_MSG_DATA_LEN) // 消息太长，认为非法，或者已经处理错误
			{
				CloseSession();
				LogWarn("Session %lld Recv msg data %d, msg len %d, drop", m_sesID, length, dwLength);
				return false;
			}
			
			int nBufDataSize = length - nOffset; //剩余数据长度
			int nNeedDataLen = dwLength + HYHEADERSIZE - m_buf.GetBufLen(); // 未缓存的消息长度
			int nAppendLen = std::min(nNeedDataLen, nBufDataSize);
			if (nAppendLen >= 0)
			{
				m_buf.Append(m_szData + nOffset, nAppendLen);
				nOffset += nAppendLen;
			}

			if (m_buf.GetBufLen() == dwLength + HYHEADERSIZE) // 取好一条消息，交给业务层处理
			{
				m_pUser->OnMsg(m_buf.GetBufPtr(), HYHEADERSIZE + dwLength);
				m_buf.Clear();
			}			
		}
	}
	else
	{
		if (ec.value() == 10054) //远端断开连接
		{
			CloseSession();
		}
		else
		{
			LogWarn("Session recv msg err %d.", ec.value());
		}
	}
	return true;
}

void CTCPSession::Read()
{
	m_socket.async_read_some(asio::buffer(m_szData, max_length),
		[&](std::error_code ec, std::size_t length)
	{
		HandleMsg(ec, length);
		if(m_socket.is_open())
		{
			Read();
		}
	});
}

bool CTCPSession::Send(char* msg, int nLength)
{
	//LogTrace("Session %lld Send %d", m_sesID, nLength);
	asio::async_write(m_socket, asio::buffer(msg, nLength),
		[&](std::error_code ec, std::size_t sz)
	{
		if (ec)
		{
			LogWarn("Session %lld send msg result %d, isopen:%d", m_sesID, ec.value(), m_socket.is_open());
			CloseSession();
			return false;
		}
		else
		{
			//LogTrace("Session %lld Send size:%d", m_sesID, sz);
		}
		
	});

	return true;
}

bool CTCPSession::BindUser(CLinkUser* user)
{
	m_pUser = user;
	return true;
}
