#pragma once
#include "InterFace.h"
#include "LTUtils.h"
#include "json/json.h"
class UIClass;
class UITrayMenu :public I_LTFRAME_IWebView
{
public:
	UITrayMenu(void);
	~UITrayMenu(void);
	map<std::string,int> JavaScriptMap;
	void CreateTrayBox(HWND hwnd,char* apppath,int x,int y,int cx,int cy);
	virtual LRESULT HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	virtual void OnFinalMessage(){
		ltskinview->ReleaseApplaction();
		delete this;
	};
	virtual void ReleaseWindow()
	{
		ltskinview->ReleaseWindow();
	};
	I_LTFRAME_IWebView* ltskinview; 
	HWND GetWindowHandle();
	Json::Value GetJsonRoot(string s);
	int GetJavaScriptIntValue(string s);
	int SendMessageToParentWindow(UINT uMsg,WPARAM wParam,LPARAM lParam);
	int applactionclose;
private:
//	UIClass * ParentInstance;
	HWND parentHwnd;
	void SetVolume(WPARAM wParam);
	void SetPlayStatus(WPARAM wParam,LPARAM lParam);
};
