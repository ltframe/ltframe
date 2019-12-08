#include "stdafx.h"

#include "KK.h"
#include <string>
using namespace std; 




class UIClass : public I_LTFRAME_IWebView
{
public:
	UIClass();
	~UIClass(void);
	virtual LRESULT HandleUserMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	void DragWindow();
	void QuitApp();
	void MinWindow();
	virtual void ReleaseWindow(){}
	DEFINEFUNCTION DragWindowFun(LTExecState* es);
private:
	WebView* ltskinview;
	wchar_t* applicationpath;
};
