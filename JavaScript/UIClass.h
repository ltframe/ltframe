#include "stdafx.h"
#include "InterFace.h"
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
	void f1();
	void f5(LTExecState*);
	virtual void OnFinalMessage(){
		delete this;
	};
private:
	LTFrameView* ltskinview;
	wchar_t* applicationpath;
};