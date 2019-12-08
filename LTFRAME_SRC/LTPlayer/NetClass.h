#pragma once
#ifndef NetClass___
#define NetClass___
#include "stdafx.h"
#include <windows.h>
#include <wininet.h>
#include <mmsystem.h>
#include <string>
using namespace std;

class NetClass
{
public:
	NetClass(void);
	~NetClass(void);
	UINT InternetGetFile(HINTERNET IN hOpen,CHAR *szUrl,CHAR *szFileName);
	int InternetDownloadFile(char *URL, char *FileDest);
	string ConvertUTF8toGB2312(const char *pData, size_t size);
	string HttpRequest(wstring szUrl); 
};
#endif 