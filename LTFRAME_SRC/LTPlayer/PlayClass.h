#pragma once
#ifndef PlayClass___
#define PlayClass___
#include "LTUtils.h"
class PlayClass
{
public:
	PlayClass(HWND m_hWnd);
	~PlayClass(void);
	LTMusic * ltmusic;
	MusicBasicInfo GetMusicBasicInformation(char * filepath);
	void MusicPlay(const char* filepath);
	void MusicStop();
	double CurrentMusicTime();
	int GetMusicStatus();
	int GetSliderPre();
	void SetMusicPosition(double prc);
	void SetMusicVolume(float volume);
	void BakRegisterValue(wchar_t* strExt);
	void SetMusicEffect(int effectid);
	void RegisterFileRelation(wchar_t* strExt, wchar_t* strAppName, wchar_t* strAppKey, wchar_t* strDefaultIcon, wchar_t* strDescribe);
	bool IsPlayMusic();
};
#endif 