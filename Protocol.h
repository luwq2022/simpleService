#pragma once

using DWORD = unsigned long;
using WORD = unsigned short;

typedef struct tagHEADER
{
    WORD	wOrigine;			//消息来源
    WORD	wType;				//消息类型
    DWORD	dwLength;			//消息体长度
}HYHEADER,*PHYHEADER;

const int HYHEADERSIZE = 8;