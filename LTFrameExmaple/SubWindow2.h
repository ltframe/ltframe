#include "stdafx.h"
#include "InterFace.h"
#include <string>
using namespace std; 

namespace SubWindowSpace{

class SubWindow2 : public I_LTFRAME_IWebView
{
public:
	SubWindow2(HWND,int,int);
	~SubWindow2(void);
	virtual LRESULT HandleMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
	virtual LPCWSTR GetWindowClassName();
	void DragWindow();
	void CloseWindow();
	void MinWindow();
	virtual void ReleaseWindow()
	{
		ltskinview->ReleaseWindow();
	};
	virtual void OnFinalMessage(){
		ltskinview->ReleaseApplaction();
		delete this;
	};
private:
	I_LTFRAME_IWebView* ltskinview;
	wchar_t* applicationpath;

};

}
