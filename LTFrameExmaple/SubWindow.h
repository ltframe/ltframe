#include "stdafx.h"
#include "InterFace.h"
#include <string>
#include <map>
using namespace std; 

namespace SubWindowSpace{

class SubWindow : public I_LTFRAME_IWebView
{
public:
	SubWindow(HWND);
	~SubWindow(void);
	virtual LRESULT HandleUserMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	void DragWindow();
	void CloseWindow();
	void MinWindow();
	virtual void OnFinalMessage(){
		delete this;
	};
private:
	LTFrameView* ltskinview;
	wchar_t* applicationpath;
	map<HWND,SubWindow*> instancelist;
};

}
