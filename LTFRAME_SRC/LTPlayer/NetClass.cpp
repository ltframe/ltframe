#include "StdAfx.h"
#include "NetClass.h"
enum
{
	INTERNET_ERROR_OPENURL=1,
	INTERNET_ERROR_FILEOPEN,
	INTERNET_ERROR_READFILE,
	INTERNET_ERROR_OPEN,
	INTERNET_ERROR_NOT_FOUND
};

NetClass::NetClass(void)
{
}

NetClass::~NetClass(void)
{

}
UINT NetClass::InternetGetFile(HINTERNET IN hOpen,CHAR *szUrl,CHAR *szFileName)
{
    DWORD dwSize;
    CHAR   szHead[] = "Accept: */*\r\n\r\n";
    VOID* szTemp[16384];
    HINTERNET  hConnect;
    FILE * pFile;

	if ( !(hConnect = InternetOpenUrlA ( hOpen, szUrl, szHead,
			lstrlenA (szHead), INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD, 0)))
	{
		return INTERNET_ERROR_OPENURL;
	}


	if( !(pFile = fopen (szFileName, "wb" ) ) )
	{
		return INTERNET_ERROR_FILEOPEN;
	}

	DWORD dwStatusCode;
	DWORD dwStatusSize = sizeof (dwStatusCode);
	HttpQueryInfo (hConnect, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, & dwStatusCode, & dwStatusSize, NULL);

	if(HTTP_STATUS_NOT_FOUND==dwStatusCode)
	{
		return INTERNET_ERROR_NOT_FOUND;
	}

	DWORD dwByteToRead = 0;
	DWORD dwSizeOfRq = 4;
	DWORD dwBytes = 0;

    if (!HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, 
                    (LPVOID)&dwByteToRead, &dwSizeOfRq, NULL))
	{
		dwByteToRead = 0;
	}
	DWORD start;
	DWORD end;
	DWORD time;
	time = 10;
	start = timeGetTime();
    do
    {
			  // Keep coping in 16 KB chunks, while file has any data left.
			  // Note: bigger buffer will greatly improve performance.
		if (!InternetReadFile (hConnect, szTemp, 16384,  &dwSize) )
		{
			fclose (pFile);
			return INTERNET_ERROR_READFILE;
		}
		if (!dwSize)
			break;  // Condition of dwSize=0 indicate EOF. Stop.
		else
			fwrite(szTemp, sizeof (char), dwSize , pFile);
		dwBytes+=dwSize;

		printf_s("%d", (dwBytes*100)/dwByteToRead);
		printf("\r\n");
		FLOAT fSpeed = 0;
		fSpeed = (float)dwBytes;
		fSpeed /= ((float)time)/1000.0f;
		fSpeed /= 1024.0f;

		char s[260];
		sprintf(s, "%d KB / %d KB @ %1.1f KB/s", dwBytes/1024, dwByteToRead/1024, fSpeed);
		printf(s);
		printf("\r\n");

		end = timeGetTime();
		time = end - start;
		if(time == 0)
		time = 10;
    }   // do
	while (TRUE);
		fflush (pFile);
		fclose (pFile);
		return 0;
}

int NetClass::InternetDownloadFile(char *URL, char *FileDest)
{
	DWORD dwFlags;
	DWORD dwResult = INTERNET_ERROR_OPEN;
	InternetGetConnectedState(&dwFlags, 0);
	if(dwFlags & INTERNET_CONNECTION_OFFLINE)//take appropriate steps
		return INTERNET_ERROR_OPEN;
	CHAR strAgent[64];
	sprintf(strAgent, "Agent%ld", timeGetTime());
	HINTERNET hOpen;
	if(!(dwFlags & INTERNET_CONNECTION_PROXY))
		hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, 0);
	else
		hOpen = InternetOpenA(strAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hOpen)
	{
		dwResult = InternetGetFile(hOpen, URL, FileDest);
		InternetCloseHandle(hOpen);
	}
	else 
	    return INTERNET_ERROR_OPEN;
	return dwResult;
}
string NetClass::ConvertUTF8toGB2312(const char *pData, size_t size)
{
	size_t n = MultiByteToWideChar(CP_UTF8, 0, pData, (int)size, NULL, 0);
	WCHAR   *   pChar   =   new   WCHAR[n+1];
	n = MultiByteToWideChar(CP_UTF8, 0, pData, (int)size, pChar, n);
	pChar[n]=0;
	n = WideCharToMultiByte(936, 0, pChar, -1, 0, 0, 0, 0);
	char *p = new char[n+1];
	n = WideCharToMultiByte(936, 0, pChar, -1, p, (int)n, 0, 0);
	string result(p);
	delete []pChar;
	delete []p;
	return result;
}
string NetClass::HttpRequest(wstring szUrl) 
{
	//WCHAR szUrl[] = L"http://api.douban.com/v2/music/search?q=%E6%97%A0%E5%B0%BD%E6%97%8B%E8%BD%AC";
	//WCHAR szUrl[] = L"http://www.baidu.com";
	HINTERNET hInternet1 =
		InternetOpen(NULL,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,NULL);
	if (NULL == hInternet1)
	{
		InternetCloseHandle(hInternet1);
		return "";
	}
	HINTERNET hInternet2 =
		InternetOpenUrl(hInternet1,szUrl.c_str(),NULL,NULL,INTERNET_FLAG_NO_CACHE_WRITE,NULL);
	if (NULL == hInternet2)
	{
		InternetCloseHandle(hInternet2);
		InternetCloseHandle(hInternet1);
		return "";
	}
	DWORD dwMaxDataLength = 16384;

	string htmlstr = "";
	BOOL bRet = TRUE;
	do 
	{
		char* pBuf = new char [16384];

		DWORD dwReadDataLength = NULL;

		ZeroMemory(pBuf,16384);
		bRet = InternetReadFile(hInternet2,pBuf,dwMaxDataLength,&dwReadDataLength);
		if (dwReadDataLength == 0)
		{
			break;
		}
		string str = ConvertUTF8toGB2312(pBuf,dwMaxDataLength);
		htmlstr+=str;
		delete[] pBuf;
	} while (bRet);
	InternetCloseHandle(hInternet2);
	InternetCloseHandle(hInternet1);
	
	return htmlstr;
}