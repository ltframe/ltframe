#include "stdafx.h"
#include "InterFace.h"
#include <string>
using namespace std; 

namespace CanvasWindowSpace{

class CanvasWindow : public I_LTFRAME_IWebView
{
public:
	CanvasWindow();
	~CanvasWindow(void);
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
