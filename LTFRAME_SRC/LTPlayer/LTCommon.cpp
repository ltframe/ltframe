#include "stdafx.h"
#include "LTUtils.h"
#include "Shlwapi.h"
#include "objbase.h"
#include<algorithm>
#include<map>
#include <windows.h>
#include <Wincrypt.h>



#define _CONVERPAHT  "UserData\\cover\\src"
#define _CONVERBLURPATH "UserData\\cover\\blur"
#define CHECK_NULL_RET(bCondition) if (!bCondition) goto Exit0
#define BUFSIZE 1024
#define MD5LEN  16

CLTCommon::CLTCommon()
{
	return;
}
char* CLTCommon::ConvertWCharToChar(wchar_t* pwszUnicode)
{
	int iSize; 
	char* pszMultiByte; 

	iSize = WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, NULL, 0, NULL, NULL); 
	pszMultiByte = (char*)malloc((iSize+1)/**sizeof(char)*/); 
	WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, pszMultiByte, iSize, NULL, NULL); 
	return pszMultiByte;
}

wchar_t* CLTCommon::AnsiToWideChar(char* szStr )
{
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
	wchar_t* pResult; 
	if (nLen == 0)
	{
		return NULL;
	}
	pResult = (wchar_t*)malloc((nLen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
	return pResult;
}
//
char* CLTCommon::GetFileInfo(char* filepath)
{

	wstring string(PathFindFileName(CLTCommon::AnsiToWideChar(filepath)));
	 basic_string <char>::size_type  F = string.find_last_of(L".");
	 wstring s = string.substr(0,F);
	 return CLTCommon::ConvertWCharToChar((wchar_t*)(s.c_str()));
}

//wchar_t* CLTCommon::GetFileExtName(wchar_t* filepath)
//{
//
//	wstring string(PathFindFileName(CLTCommon::AnsiToWideChar(filepath)));
//	 basic_string <char>::size_type  F = string.find_last_of(L".");
//	 wstring s = string.substr(0,F);
//	 return CLTCommon::ConvertWCharToChar((wchar_t*)(s.c_str()));
//}



char* CLTCommon::ReplaceFilePath(char* _str,char* str1,char* repstr1)
{
	char *d =new char[MAX_PATH];
	string str(_str);  
	int find=0, where=0;  
	while((where=str.find(str1, find))!=string::npos) //从find处寻找，where为找到字符串返回的位置  
	{  
		str.replace(where, strlen(str1),repstr1);   //注意这里的长度是被替换的字符串的长度，而不是partern的长度  
		find = strlen(str1)+where;   //将下一次寻找字符串的位置加上partern的长度  
	}  
	//_str=(char*)str.c_str();  

	strcpy(d,str.c_str());
	return d;  

}


char* CLTCommon::Seconds2Time(int sec)
{
	char cs[50]={};
	int mm=sec/60;
	int se=sec%60;
	char* _pmm="";
	char* _pse="";
	if(mm<10)
	{
		_pmm = "0";
	}else{_pmm="";}
	if(se<10)
	{
		_pse = "0";
	}else{_pse="";}
	sprintf(cs,"%s%d:%s%d",_pmm,mm,_pse,se);
	return cs;
}

char* CLTCommon::GetFileExt(wchar_t* str)
{
	wstring s1 (str);
	basic_string <wchar_t>::size_type  s = s1.find_last_of(L".");
	basic_string <wchar_t> str2  = s1.substr(s+1,s1.length()-s);
    transform(str2.begin(), str2.end(), str2.begin(),tolower);   
    char * d = ConvertWCharToChar((wchar_t*)str2.c_str());

	return d;
}

void CLTCommon::WriteFile(char* str,char* path,int isappend)
{
	//FILE *fp;
	//int i;
	//if((fp=fopen(path,"w+"))==NULL)
	//{
	//	printf("cant open the file");
	//	exit(0);
	//}
	//
	//	if(fwrite(str,strlen(str),1,fp)!=1)
	//		printf("file write error\n");

	//fclose(fp);
	DWORD createfiletype = CREATE_ALWAYS;
	if(isappend){
		if(FileExists(path))
			createfiletype = OPEN_ALWAYS;
	}


	HANDLE pfile;
	pfile = ::CreateFileA(path,GENERIC_WRITE,0,NULL,createfiletype,
		FILE_ATTRIBUTE_NORMAL,NULL); // 用这个函数比OpenFile好
	if ( pfile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(pfile); // 一定注意在函数退出之前对句柄进行释放。
		return ;
	}
	if(isappend){
		SetFilePointer(pfile,0, NULL, FILE_END); 
	}

	DWORD outBytes = 0;  
	::WriteFile(pfile, str, strlen(str), &outBytes, NULL );
	int x = GetLastError();
	CloseHandle(pfile); // 关闭句柄。

}

char* CLTCommon::GetFile(char* path)
{
	//FILE *fp;
	//if((fp=fopen(path,"rb"))==NULL)
	//{
	//	printf("cant open the file");
	//	exit(0);
	//}

	//fseek (fp , 0 , SEEK_END);  
	//long lSize = ftell (fp);  
	//fseek(fp,0,SEEK_SET);
	////fflush (fp);  
	//char *output = (char*) malloc (lSize);
	//ZeroMemory(output,lSize);
	//fread(output,1,lSize,fp); 
	//fclose(fp);
	//output[lSize] = 0;
	//return output;
		HANDLE pfile;
		pfile = ::CreateFileA(path,GENERIC_READ,0,NULL,OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,NULL); // 用这个函数比OpenFile好
		if ( pfile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(pfile); // 一定注意在函数退出之前对句柄进行释放。
			return 0;
		}
		DWORD filesize=GetFileSize(pfile,NULL);
		char* buffer=new char[filesize+1]; // 最后一位为 '/0',C-Style 字符串的结束符。
		ZeroMemory(buffer,filesize+1);
		DWORD readsize;
		ReadFile(pfile,buffer,filesize,&readsize,NULL);
		buffer[filesize]=0;
		CloseHandle(pfile); // 关闭句柄。
		return buffer;
}

char* CLTCommon::GetApplicationPath()
{
	char szFilePath[MAX_PATH + 1];
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
	(strrchr(szFilePath, '\\'))[1] = 0;
	return szFilePath;
}

//bool CLTCommon::IsPlayerSupportMusicType(CString mname)
//{
//	CString ext = GetMusicExt(mname).MakeLower();
//	return IsStrInStrAarry(ext,L"mp1,mp2,mp3,wma,flac,aac,ogg,ape");
//} 
void CLTCommon::SplitSTLString(string strSrc ,vector<string>& vecDest ,char cSeparator)
{
    if( strSrc.empty() )
        return ;
 
      string::size_type size_pos = 0;
      string::size_type size_prev_pos = 0;
        
      while( (size_pos = strSrc.find_first_of( cSeparator ,size_pos)) != string::npos)
      {
           string strTemp=  strSrc.substr(size_prev_pos , size_pos-size_prev_pos );
           
           vecDest.push_back(strTemp);
           size_prev_pos =++ size_pos;
      }          
      vecDest.push_back(&strSrc[size_prev_pos]);
}

bool CLTCommon::IsStrInStrAarry(char * str,char* strstr)
{
	vector<string> vecDest;
	CLTCommon::SplitSTLString(string(strstr),vecDest,',');
	for(int i=0;i<vecDest.size();i++)
	{
		//string sss = "aaaa";
		if(vecDest.at(i)==string(str)){
		 return true;
		}
	}
	return false;
}

bool CLTCommon::FileExists(char * filepath)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind,hbgFind;
	hFind = FindFirstFileA(filepath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return false;
	} 
	return true;
}


void CLTCommon::string_replace(std::string& strBig, const std::string & strsrc, const std::string &strdst)
{
     std::string::size_type pos = 0;
     while( (pos = strBig.find(strsrc, pos)) != string::npos)
     {
         strBig.replace(pos, strsrc.length(), strdst);
         pos += strdst.length();
     }
}


const char* CLTCommon::CreateGUID()
{
	static char buf[64] = {0};
	  GUID guid;
	  if (S_OK == ::CoCreateGuid(&guid))
		{
			 _snprintf(buf, sizeof(buf)
				, "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"
				, guid.Data1
				, guid.Data2
				, guid.Data3
				, guid.Data4[0], guid.Data4[1]
			    , guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			    , guid.Data4[6], guid.Data4[7]
			   );
			}
	return (const char*)buf;
}

bool CLTCommon::CheckAllTemplateFile(char* basepath)
{

	map <char*, wchar_t*> m1;
	map <char*, wchar_t*>::const_iterator m1_AcIter;
	typedef pair <char*, wchar_t*> string_Pair;

	m1.insert(string_Pair(MUSICLIST_DATA,L""));
	m1.insert(string_Pair(PLAYCONFIG_DATA,L""));
	m1.insert(string_Pair(_CONVERPAHT,L""));
	m1.insert(string_Pair(_CONVERBLURPATH,L""));



	char* files[] ={"UserData\\template\\contextmenu.html","UserData\\template\\css\\css.css","UserData\\template\\css\\cssmenu.css","UserData\\template\\css\\csstray.css","UserData\\template\\css\\jquery.minicolors.css","UserData\\template\\index.html","UserData\\template\\js\\jquery.min.js","UserData\\template\\js\\jquery.minicolors.js","UserData\\template\\js\\jquery.nicescroll.js","UserData\\template\\js\\js.js","UserData\\template\\js\\jstray.js","UserData\\template\\js\\onerror.js","UserData\\template\\tray.html"};
	wchar_t* md5s[] = {L"F461B2D043B2F9541A0CE2458A978D6F",L"FFCE513B0540449B26C7465ADEE8CF5C",L"425C636AF01427041AD00335015FD1E4",L"9E0F98323FC8C37B19A0D73E45DBA904",L"30EB79AF58D3ABBEF84A6C4CA37A274E",L"6BB88FC4074761D09A0053F60C3E3752",L"B8D64D0BC142B3F670CC0611B0AEBCAE",L"D08113D748395209464E9D8CB61DB96E",L"0DD51F1C900998B06F5AE3F6BC6AC5ED",L"1449F2A97653C07C0A1AED7B249B84A9",L"7E4FF4B1E99A3DF6233D936281BC0E56",L"6705F099868660565FEAC69E4B78DE2F",L"01F2FA8B7F059F1B09D23E1D51375639"};
	for(int i=0;i<sizeof(files)/sizeof(char*);i++)
	{
		m1.insert(string_Pair(files[i],md5s[i]));
	}

	

	for (map<char*, wchar_t*>::iterator it = m1.begin(); it != m1.end(); it++)
    {
		char _path[MAX_PATH] ={}; 

		strcat(_path,basepath);
		strcat(_path,it->first);
		if(!wcscmp(L"",it->second))
		{
			if(!FileExists(_path))
			{
				return false;
			}
			continue;
		}
		else
		{
			if(!FileExists(_path))
			{
				return false;
			}
		}
		DWORD dwStatus = 0;
		TCHAR szResult[MD5LEN*2+1] = {0};
		wchar_t * fp =AnsiToWideChar(_path);
		TCHAR* szFilePath = fp;
	 
		GetContentMD5((BYTE *)szFilePath,TRUE, TRUE,szResult,dwStatus);
		if(wcscmp(szResult,(it->second)))
		{
			free(fp);
			return false;
		}
		free(fp);
    }
	return true;
}


bool CLTCommon::GetContentMD5(
    BYTE *pszFilePath, 
    BOOL bFile, 
    BOOL bUpperCase, 
    TCHAR *pszResult,
    DWORD &dwStatus)
{
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile = NULL;
    BYTE rgbFile[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[MD5LEN];
    DWORD cbHash = 0;
    CHAR rgbDigitsL[] = "0123456789abcdef";
    CHAR rgbDigitsU[] = "0123456789ABCDEF";
    CHAR *rgbDigits = bUpperCase ? rgbDigitsU : rgbDigitsL;
    TCHAR szResult[MD5LEN*2+1] = {0};
 
    dwStatus = 0;
    bResult = CryptAcquireContext(&hProv,
        NULL,
        NULL,
        PROV_RSA_FULL,
        CRYPT_VERIFYCONTEXT);
    CHECK_NULL_RET(bResult);
 
    bResult = CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);
    CHECK_NULL_RET(bResult
		);
 
    if (bFile)
    {
        hFile = CreateFile((TCHAR *)pszFilePath,
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_SEQUENTIAL_SCAN,
            NULL);
        CHECK_NULL_RET(!(INVALID_HANDLE_VALUE == hFile));
 
        while (bResult = ReadFile(hFile, rgbFile, BUFSIZE, 
            &cbRead, NULL))
        {
            if (0 == cbRead)
            {
                break;
            }
 
            bResult = CryptHashData(hHash, rgbFile, cbRead, 0);
            CHECK_NULL_RET(bResult);
        }
    }
    else
    {
        bResult = CryptHashData(hHash, pszFilePath, strlen((CHAR *)pszFilePath), 0);
        CHECK_NULL_RET(bResult);
    }
 
    cbHash = MD5LEN;
    if (bResult = CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0))
    {
        TCHAR szTmpBuff[3] = {0};
        for (DWORD i = 0; i < cbHash; i++)
        {
            swprintf(szTmpBuff, TEXT("%c%c"), rgbDigits[rgbHash[i] >> 4],
                rgbDigits[rgbHash[i] & 0xf]);
            lstrcat(szResult, szTmpBuff);
        }
        bResult = TRUE;
    }
 
Exit0:
    dwStatus = GetLastError();
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);
 
    lstrcpy(pszResult, szResult);
 
    return bResult; 
}