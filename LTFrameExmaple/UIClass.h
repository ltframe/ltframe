#include "stdafx.h"
#include "InterFace.h"
#include <string>
using namespace std; 

namespace UIClassSpace{

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
	void OpenNewApplaction(wchar_t*,wchar_t* params=0);
	void OpenSubWindow();
	virtual void OnFinalMessage(){
		delete this;
	};
private:
	LTFrameView* ltskinview;
	wchar_t* applicationpath;
	RECT CenterWindow(int w,int h);

};
}