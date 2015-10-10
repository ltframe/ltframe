#include "stdafx.h"
#include "InterFace.h"
#include <string>
using namespace std; 

class UIClass : public I_LTFRAME_IWebView
{
public:
	UIClass(wchar_t* filepath);
	~UIClass(void);
	virtual LRESULT HandleUserMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	void DragWindow();
	void QuitApp();
	void MinWindow();
	void SetLayerWindow(bool type);
	virtual void OnFinalMessage(){
		delete this;
	};
private:
	LTFrameView* ltskinview;
	wchar_t* applicationpath;
};