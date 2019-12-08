#include "StdAfx.h"
#include "PlayClass.h"
#include "Shlobj.h"
#include <algorithm>
PlayClass::PlayClass(HWND m_hWnd)
{
	ltmusic =new LTMusic(m_hWnd);
}

PlayClass::~PlayClass(void)
{

}


MusicBasicInfo PlayClass::GetMusicBasicInformation(char * filepath)
{
	return ltmusic->GetMusicBasicInformation(filepath);
}

void PlayClass::MusicPlay(const char* filepath){
 
	MusicStop();
	ltmusic->MusicPlay(CLTCommon::AnsiToWideChar((char*)filepath));
}

void PlayClass::MusicStop()
{
	ltmusic->MusicStop();
}

double PlayClass::CurrentMusicTime()
{
	return ltmusic->CurrentMusicTime();
}

int PlayClass::GetMusicStatus()
{
	return ltmusic->GetMusicStatus();
}

bool PlayClass::IsPlayMusic()
{
	return ltmusic->IsPlayMusic();
}

int PlayClass::GetSliderPre()
{
	double a = ltmusic->GetCurrentMusicSeconds();
	double b = ltmusic->CurrentMusicTime();
	return	(b/a)*100;
}

void PlayClass::SetMusicPosition(double prc)
{
	double d = ltmusic->GetCurrentMusicSeconds();
	double c = d*prc;
	ltmusic->SetMusicPosition(c);
}

void PlayClass::SetMusicVolume(float volume)
{
	ltmusic->SetMusicVolume(volume);
}
void PlayClass::SetMusicEffect(int effectid)
{
	//ltmusic->SetMusicEffect(effectid);
}

void PlayClass::BakRegisterValue(wchar_t* strExt)
{
	LONG lRet;
	HKEY hKey;
	TCHAR szProductType[MAX_PATH];
	DWORD dwBufLen=MAX_PATH*sizeof(TCHAR);


	TCHAR szProductTypeT[MAX_PATH];
	ZeroMemory(szProductTypeT,MAX_PATH);
	DWORD dwBufLenT=MAX_PATH*sizeof(TCHAR);


	lRet = RegOpenKeyEx(HKEY_CLASSES_ROOT,
		strExt, 0, KEY_QUERY_VALUE, &hKey );
	if( lRet != ERROR_SUCCESS)
		return;

	lRet = RegQueryValueEx( hKey, TEXT(""),
		NULL, NULL, (LPBYTE) szProductType, &dwBufLen);

	string regifile("LTplayer");
	regifile.append("strExt");
	//CString regfile("LTplayer");
	//regfile.Append(strExt);
	wstring sz = szProductType;
	if (wcscmp(_wcslwr(szProductType),L"ltplayerstrext")==0)
	{
		RegCloseKey( hKey );
		return;
	}
	if (RegQueryValueEx(hKey, TEXT("ltplayerbak"),NULL, NULL, (LPBYTE)szProductTypeT,&dwBufLenT)!=ERROR_SUCCESS)
	{
		if (wcscmp(szProductTypeT,szProductType))
		{
			RegCreateKey(HKEY_CLASSES_ROOT,strExt,&hKey);  
			//RegSetValue(hKey,null,REG_SZ,szProductType,wcslen(szProductType)+1);  
			RegSetValueEx(hKey,TEXT("ltplayerbak"),0,REG_SZ,(LPBYTE)szProductType,dwBufLen+1);
		}
	}


	RegCloseKey( hKey );
}
void  PlayClass::RegisterFileRelation(wchar_t* strExt, wchar_t* strAppName, wchar_t* strAppKey, wchar_t* strDefaultIcon, wchar_t* strDescribe)
{

	BakRegisterValue(strExt);
	wchar_t strTemp[3000]={};
	HKEY hKey;  
	RegCreateKey(HKEY_CLASSES_ROOT,strExt,&hKey);  
	RegSetValue(hKey,_T(""),REG_SZ,strAppKey,wcslen(strAppKey)+1);  
	RegCloseKey(hKey);  


	swprintf(strTemp,L"%s\\DefaultIcon",strAppKey);

	//strTemp.Format(_T("%s\\DefaultIcon"),strAppKey); 
	RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);  
	RegSetValue(hKey,_T(""),REG_SZ,strDefaultIcon,wcslen(strDefaultIcon)+1);  
	RegCloseKey(hKey);  

	ZeroMemory(strTemp,3000);
	//strTemp.Format(_T("%s\\Shell"),strAppKey);  
	swprintf(strTemp,L"%s\\Shell",strAppKey);
	RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);  
	RegSetValue(hKey,_T(""),REG_SZ,_T("Open"),5);  
	RegCloseKey(hKey);  

	ZeroMemory(strTemp,3000);
	//strTemp.Format(_T("%s\\Shell\\Open\\Command"),strAppKey);
	swprintf(strTemp,3000,L"%s\\Shell\\Open\\Command",strAppKey);
	RegCreateKey(HKEY_CLASSES_ROOT,strTemp,&hKey);  
  
	ZeroMemory(strTemp,3000);
	swprintf(strTemp,L"%s \"%%1\"",strAppName);
	//strTemp.Format(_T("%s \"%%1\""),strAppName);
	RegSetValue(hKey,_T(""),REG_SZ,strTemp,wcslen(strTemp)+1);  
	RegCloseKey(hKey);  
	
	SHChangeNotify(SHCNE_ASSOCCHANGED,SHCNF_FLUSHNOWAIT,0,0);
}