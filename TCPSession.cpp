#include "TCPSession.h"
#include "LinkUser.h"
#include "Protocol.h"

const int MAX_MSG_DATA_LEN = 64 * 1024;

void CTCPSession::Start()
{
	Read();
}

void CTCPSession::CloseSession()
{
	// TODO
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
		}
	}

	return m_pUser ? true : false;
}

bool CTCPSession::HandleMsg(std::error_code ec, std::size_t length)
{
	if (!ec)
	{
		if(m_pUser == nullptr) CreateUser();

		if (m_pUser == nullptr) return false;
		
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
		if (!m_socket.is_open())
		{
			if (m_pUser)
			{
				m_pUser->OnBreak();
			}
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
		Read();
	});
}

bool CTCPSession::Send(char* msg, int nLength)
{
	/*
	asio::async_write(m_socket, asio::buffer(msg, nLength),
		[&](std::error_code ec, std::size_t )
	{
		if (ec)
		{
			if (!m_socket.is_open())
			{
				if (m_pUser)
				{
					m_pUser->OnBreak();
				}
			}
		}
		
	});
	*/

	return true;
}

bool CTCPSession::BindUser(CLinkUser* user)
{
	m_pUser = user;
	return true;
}
