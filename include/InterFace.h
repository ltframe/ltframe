
#ifndef LTFRAME_INTERFACE_H
#define LTFRAME_INTERFACE_H

#include <windows.h>
#include <Shobjidl.h>


/*JS BIND*/

typedef void* LTExecState;
typedef __int64 LTEncodedJSValue;

typedef LTEncodedJSValue (__fastcall *LTNativeFunction)(LTExecState*);
#define  DEFINEFUNCTION  LTEncodedJSValue __fastcall

/*JS BIND END*/
LTEncodedJSValue Int2JsValue(int n);
LTEncodedJSValue Float2JsValue(float f);
LTEncodedJSValue Double2JsValue(double d);
LTEncodedJSValue Boolean2JsValue(bool b);
LTEncodedJSValue JsUndefined();
LTEncodedJSValue JsNull();
LTEncodedJSValue JsTrue();
LTEncodedJSValue JsFalse();
LTEncodedJSValue String2JsValue(LTExecState es, const wchar_t* str);
int JsValue2Int(LTExecState es, LTEncodedJSValue v);
float JsValue2Float(LTExecState es, LTEncodedJSValue v);
double JsValue2Double(LTExecState es, LTEncodedJSValue v);
bool JsValue2Boolean(LTExecState es, LTEncodedJSValue v);
int JsValue2StringW(LTExecState es, LTEncodedJSValue v,wchar_t* result);
int JsValue2String(LTExecState es, LTEncodedJSValue v,char* result);
LTEncodedJSValue GetJsParameter(LTExecState es, int argIdx);
LTEncodedJSValue GetJsObjectParameter(LTExecState es,int argIdx,char* key);
LTEncodedJSValue JsCall(LTExecState es,LTEncodedJSValue func,LTEncodedJSValue* args, int argCount);
LTEncodedJSValue CreateJsObject(LTExecState es,LTEncodedJSValue o,char* key,LTEncodedJSValue value);
LTEncodedJSValue CreateObject(LTExecState es);
LTEncodedJSValue GetJsObjectAttribute(LTExecState es,LTEncodedJSValue v,char* key);
bool IsJsNumber(LTEncodedJSValue v);
bool IsJsString(LTEncodedJSValue v);
bool IsJsBoolean(LTEncodedJSValue v);
bool IsJsObject(LTEncodedJSValue v);
bool IsJsFunction(LTEncodedJSValue v);
bool IsJsUndefined(LTEncodedJSValue v);
bool IsJsNull(LTEncodedJSValue v);
bool IsJsArray(LTEncodedJSValue v);
bool IsJsTrue(LTEncodedJSValue v);
bool IsJsFalse(LTEncodedJSValue v);

class WebView;
class I_LTFRAME_IWebView
{
public:
   virtual LRESULT HandleUserMessage(HWND, UINT, WPARAM, LPARAM)=0;
   virtual LPCWSTR GetWindowClassName()=0;
   virtual bool  OnDrop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
   virtual bool  OnDragLeave();
   virtual bool  OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
   virtual bool  OnDragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
   virtual void OnFinalMessage();
};
class LTFrameView
{
public:
	LTFrameView(I_LTFRAME_IWebView * iwebview,LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
												   int x,int y,int nWidth,int nHeight,HICON icon);
	~LTFrameView();
	LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);
	void MessageLoop();
	void loadHTML(const wchar_t* html);
	void load(const wchar_t* html);
	void loadFile(const wchar_t* html);
	void SetViewTransparent(bool);
	void IsAutoGC(bool,int maxmemorysize=50);
	void CleanMemory();
	void SetWebSqlPath(const wchar_t* path=NULL);
	void SetLocalStoragePath(const wchar_t* path=NULL);
	LTEncodedJSValue RunJavaScript(const char* script);
	LTEncodedJSValue RunJavaScript(const wchar_t* script);
	LTExecState GetGlobalExecState();
	HWND windowHandle() const;
	void CloseWindow();
	void QuitApp();
	void BindUserFunction(const char* name, LTNativeFunction fn, unsigned int argCount);
	void GaussianBlurImage(HBITMAP hBitmap,int radius);
	void SetAllowKeyDownAutoScroll(bool);
	void EnableDragFrameChangeSize(bool);
private:
	WebView* _itframeview;
};
#endif;


