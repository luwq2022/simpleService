#pragma once
#include <stdarg.h>
using ull = unsigned long long;
class CHYBuffer  
{
public:
	CHYBuffer();
	virtual ~CHYBuffer();

public:
	bool	SetExpandLen(int lenExpand);
	bool	SetHeaderLen(int lenHeader);
	bool	AddDataLen(int lenData);
	void	Clear(bool bFree=false);

public:
	char	*GetBufPtr(){return m_pBuf;}
	char	*GetDataPtr(){ if (m_pBuf ==  nullptr) { return nullptr; } else { return m_pBuf + m_lenHeader; } }

	bool	Append(const void *pBuf,int len);
	bool	AppendText(const char *pszTest);
	bool	AppendString(const char *pszString);
	bool	AppendFormatText(const char* pszFmt,...);
	bool	AppendFormatTextV(const char* pszFmt,va_list args);
	bool	AppendFormatString(const char* pszFmt,...);
	bool	AppendFormatStringV(const char* pszFmt,va_list args);

	bool	AppendByte(unsigned char byValue)		{return Append(&byValue, sizeof(unsigned char));};
	bool	AppendWord(unsigned short wValue)			{return Append(&wValue, sizeof(unsigned short));};
	bool	AppendDWord(unsigned long dwValue)		{return Append(&dwValue, sizeof(unsigned long));};
	bool	AppendInt(int nValue)			{return Append(&nValue, sizeof(int));};
	bool	AppendULL(ull ullValue)	{return Append(&ullValue, sizeof(ull));}
	bool	Append(CHYBuffer &buffer)		{return Append(buffer.GetBufPtr(), buffer.GetBufLen());};

	char	*Detach();
	bool	Expand(int len);
	bool	ExpandTo(int len);
	void	Exchange(CHYBuffer &buffer);

	int		GetCapability()	{return m_lenCapability;};
	int		GetBufLen(){return m_lenData+m_lenHeader;}
	int		GetDataLen()	{return m_lenData;};
	int		GetHeaderLen()	{return m_lenHeader;};
	int		GetExpandLen()	{return m_lenExpand;};
private:
	char	*m_pBuf;
	int		m_lenData;
	int		m_lenCapability;

	int		m_lenHeader;
	int		m_lenExpand;
};

