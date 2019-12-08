#pragma once
#include "InterFace.h"
#include "LTUtils.h"
#include "json/json.h"
class UIClass;
#define BINDJAVASCRIPTTOAPP(str,index)  JavaScriptMap.insert(make_pair(string(str),index));
class UISetting :  public I_LTFRAME_IWebView
{
public:
	UISetting(UIClass* p);
	~UISetting(void);
	void CreateSettingBox(HWND hwnd,int x,int y,int cx,int cy);
    virtual LRESULT HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	I_LTFRAME_IWebView* ltskinview; 
private:
	UIClass * ParentInstance;
};
