#pragma once
#include "InterFace.h"
#include "LTUtils.h"
#include "json/json.h"
class UIContextMenu : public I_LTFRAME_IWebView
{
public:
    virtual LRESULT HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	virtual void OnFinalMessage(){
		ltskinview->ReleaseApplaction();
		delete this;
	};
	UIContextMenu(void);
	~UIContextMenu(void);
	I_LTFRAME_IWebView* ltskinview; 
	virtual void ReleaseWindow()
	{
		ltskinview->ReleaseWindow();
	};
	HWND GetWindowHandle();
	Json::Value GetJsonRoot(string);
	map<std::string,int> JavaScriptMap;
	int GetJavaScriptIntValue(string s);
	void CreateContextMenu(HWND hwnd,char* apppath,int x,int y,int cx,int cy);	
	int SendMessageToParentWindow(UINT uMsg,WPARAM wParam,LPARAM lParam);
	int applactionclose;
private:
	HWND parentHwnd;
};
