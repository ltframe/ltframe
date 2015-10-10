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
	virtual bool  OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	virtual bool  OnDrop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	virtual bool  OnDragLeave();	
	virtual bool  OnDragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	virtual void OnFinalMessage(){
		delete this;
	};
private:
	LTFrameView* ltskinview;
	wchar_t* applicationpath;
};