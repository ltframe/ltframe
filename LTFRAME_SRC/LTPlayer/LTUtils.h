// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LTUTILS_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// LTUTILS_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。


#ifndef _LTUtils_H
#define _LTUtils_H
#include "bass.h"
#include "tags.h"
#include "ConstValue.h"
#include <string.h>
#include <string>
#include <iostream>
#include <windows.h>
#include <vector>
using namespace std;



class  CLTCommon{
public:
	CLTCommon(void);
	static char* ConvertWCharToChar(wchar_t* s);
	static wchar_t* AnsiToWideChar(char* aaa);
	static char* GetFileInfo(char* filepath);
	static char* ReplaceFilePath(char* _str,char* str1,char* repstr1);
	static char* Seconds2Time(int sec);
	static void SetConfig(AppConfigInfo info);
	static void WriteFile(char* str,char* path,int isappend);
	static char* GetFile(char* path);
	static char* GetApplicationPath();
	static const char* CreateGUID();
	static void SplitSTLString(string strSrc ,vector<string>& vecDest ,char cSeparator);
	static bool IsStrInStrAarry(char * str,char* strstr);
	static void string_replace(std::string& strBig, const std::string & strsrc, const std::string &strdst);
	static char* GetFileExt(wchar_t* str);
	static bool FileExists(char * filepath);
	static bool CheckAllTemplateFile(char*);
	static bool GetContentMD5( BYTE *pszFilePath,BOOL bFile,BOOL bUpperCase,TCHAR *pszResult,DWORD &dwStatus);
	//static void FreeHeap(void* heap);
	//static AppConfigInfo GetConfig();
};

class  LTMusic
{
public:
	LTMusic(HWND m_hWnd);
	~LTMusic(void);
	 MusicBasicInfo GetMusicBasicInformation(char* filepath);
	 double  GetMusicSeconds(HSTREAM m_stream);
	 double GetCurrentMusicSeconds();
	 char* Seconds2Time(int sec);
	 bool MusicPlay(wchar_t* filename);
	 void MusicStop();
	 double CurrentMusicTime();
	 HSTREAM GetMusicHand();
	 int  GetMusicStatus();
	 void SetMusicPosition(double seconds);
	 void SetMusicVolume(float volume);
	 bool LTMusic::IsPlayMusic();
private:
	HSTREAM m_stream;
};




//// 此类是从 LTUtils.dll 导出的
//class LTUTILS_API CLTJson {
//public:
//	CLTJson(void);
//	void GetJsonRoot(wchar_t* str,Json::Value& root);
//	//Json::Value GetJsonRootP(wchar_t* s);
//	// TODO: 在此添加您的方法。
//};
#endif