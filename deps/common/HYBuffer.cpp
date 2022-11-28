#include "HYBuffer.h"
#include "stdlib.h"
#include <string>
#include <math.h>

const int BUFFER_SIZE = 2048;

CHYBuffer::CHYBuffer()
	: m_pBuf(nullptr)
	, m_lenData(0)
	, m_lenHeader(0)
	, m_lenExpand(256)
{
}

CHYBuffer::~CHYBuffer()
{
	if (m_pBuf) free(m_pBuf);
	m_pBuf = nullptr;
}

bool CHYBuffer::SetExpandLen(int lenExpand)
{	
	if (lenExpand <= 0) return false;
	m_lenExpand = lenExpand;
	return true;
}

bool CHYBuffer::SetHeaderLen(int lenHeader)
{
	if (lenHeader < 0) return false;
	m_lenHeader = lenHeader;
	return true;
}

bool CHYBuffer::AddDataLen(int lenData)
{
	if (m_lenData <= 0) return false;
	if (m_lenData + lenData + m_lenHeader >= m_lenCapability) return false;

	m_lenData += lenData;
	return true;
}

void CHYBuffer::Clear(bool bFree)
{
	if (bFree)
	{
		if (m_pBuf) free(m_pBuf);
		m_pBuf = nullptr;
		m_lenCapability = 0;
	}

	m_lenData = 0;
}

bool CHYBuffer::Append(const void * pBuf, int len)
{
	if (nullptr == m_pBuf)
	{
		int nLength = m_lenHeader + (int)ceil(len / (float)m_lenExpand) * m_lenExpand;
		m_pBuf = (char*)malloc(nLength);
		if (m_pBuf == nullptr)
		{
			return false;
		}
		m_lenCapability = nLength;
	}
	else
	{
		int need = m_lenHeader + m_lenData + len - m_lenCapability;
		if (need > 0)
		{
			if (false == Expand(need))
			{
				return false;
			}
		}
	}

	memcpy(m_pBuf + m_lenHeader + m_lenData, pBuf, len);
	m_lenData += len;
	return true;
}


int GetStrLen(const char * pszText)
{
	const char *p = pszText;
	while (*p++);
	return (int)(p - pszText);
}

bool CHYBuffer::AppendText(const char * pszTest)
{
	int nLen = GetStrLen(pszTest);
	bool ret = Append(pszTest, nLen);
	if (ret == true)
	{
		m_lenData--;
	}
	return ret;
}

bool CHYBuffer::AppendString(const char * pszString)
{
	int nLen = GetStrLen(pszString);
	return Append(pszString, nLen);
}

bool CHYBuffer::AppendFormatText(const char* pszFmt, ...)
{
	va_list arglist;
	va_start(arglist, pszFmt);
	bool ret = AppendFormatTextV(pszFmt, arglist);
	va_end(arglist);

	return ret;
}

bool CHYBuffer::AppendFormatTextV(const char* pszFmt, va_list arglist)
{
	char pData[BUFFER_SIZE] = { 0 };
	
	vsprintf(pData, pszFmt, arglist);
	int nLen = GetStrLen(pData);
	
	bool ret = Append(pData, nLen);
	if (ret == true)
	{
		m_lenData--;
	}
	return ret;
}

bool CHYBuffer::AppendFormatString(const char* pszFmt, ...)
{
	va_list arglist;
	va_start(arglist, pszFmt);
	bool ret = AppendFormatStringV(pszFmt, arglist);
	va_end(arglist);
	return ret;
}

#include <iostream>
bool CHYBuffer::AppendFormatStringV(const char* pszFmt, va_list arglist)
{
	char pData[BUFFER_SIZE] = { 0 };
	vsprintf(pData, pszFmt, arglist);
	int nLen = GetStrLen(pData);
	return Append(pData, nLen);
}

char * CHYBuffer::Detach()
{
	char* p = m_pBuf;
	m_pBuf = nullptr;
	m_lenData = 0;
	m_lenHeader = 0;
	m_lenCapability = 0;
	return p;
}

bool CHYBuffer::Expand(int len)
{
	if (len <= 0)return false;

	int nMoreLen = ((len / m_lenExpand) + 1) * m_lenExpand;
	void* newAddr = realloc(m_pBuf, m_lenCapability + nMoreLen);
	if (newAddr == nullptr)
	{
		return false;
	}
	m_pBuf = (char*)newAddr;
	m_lenCapability += nMoreLen;

	return true;
}

bool CHYBuffer::ExpandTo(int len)
{
	if (len < m_lenCapability) return false;
	
	int need = len - m_lenCapability;
	
	if (false == Expand(need))
	{
		return false;
	}

	return true;
}

template<class T>
void ExchangeData(T& t1, T& t2)
{
	T tmp = t1;
	t1 = t2;
	t2 = tmp;
}

void CHYBuffer::Exchange(CHYBuffer & buffer)
{
	ExchangeData(this->m_pBuf, buffer.m_pBuf);
	ExchangeData(this->m_lenData, buffer.m_lenData);
	ExchangeData(this->m_lenCapability, buffer.m_lenCapability);
	ExchangeData(this->m_lenHeader, buffer.m_lenHeader);
	ExchangeData(this->m_lenExpand, buffer.m_lenExpand);
}
