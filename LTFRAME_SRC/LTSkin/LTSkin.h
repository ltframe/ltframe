// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LTSKIN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LTSKIN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.


//#include "config.h"
//#include "WebView.h"
//#include "InterFace.h"
//
//
//
//// This class is exported from the LTSkin.dll
//class LTSKIN_API CLTSkin : I_LTSKIN_IWebView{
//public:
//	CLTSkin(HWND DlgHWND);
//	static HINSTANCE GetWebCoreInstanceHandle();
//	static void initialize(); 
//	static I_LTSKIN_IWebView CreateIWebView(HWND hwnd);
//	wchar_t *  RunUserDefineJavaScript(const WTF::String& message);
//	JSC::JSValue RunJavaScript(const wchar_t* script);
//	void PaintView(HDC hDC,RECT updateRect,HWND hwnd,bool islayered);
//
//	// TODO: add your methods here.
////private:
//	
//};
//
//extern LTSKIN_API int nLTSkin;
//
//LTSKIN_API int fnLTSkin(void);
