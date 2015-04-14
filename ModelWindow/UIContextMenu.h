#pragma once
#include "InterFace.h"
class UIContextMenu : public I_LTFRAME_IWebView
{
public:
    virtual LRESULT HandleUserMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	virtual void OnFinalMessage(){
		delete this;
	};
	UIContextMenu(void);
	~UIContextMenu(void);
	WebView* ltskinview; 
	HWND GetWindowHandle();
	void CreateContextMenu(HWND hwnd,int x,int y,int cx,int cy);	
	int applactionclose;
private:
	HWND parentHwnd;
	wchar_t* applicationpath;
};
