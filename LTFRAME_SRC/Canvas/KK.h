
#ifndef LTFRAME_INTERFACE_H_F
#define LTFRAME_INTERFACE_H_F

#include <windows.h>
#include <Shobjidl.h>
#ifdef LTFRAME_EXPORTS
#define LTFRAME_API __declspec(dllexport)
#else
#define LTFRAME_API __declspec(dllimport)
#endif

/*JS BIND*/
typedef void* LTExecState;
typedef __int64 LTEncodedJSValue;
class UIClass;

#define  DEFINEFUNCTION  LTEncodedJSValue __fastcall
typedef LTEncodedJSValue (__fastcall *LTNativeFunction)(LTExecState*);
/*JS BIND END*/
LTFRAME_API LTEncodedJSValue Int2JsValue(int n);
LTFRAME_API LTEncodedJSValue Float2JsValue(float f);
LTFRAME_API LTEncodedJSValue Double2JsValue(double d);
LTFRAME_API LTEncodedJSValue Boolean2JsValue(bool b);
LTFRAME_API LTEncodedJSValue JsUndefined();
LTFRAME_API LTEncodedJSValue JsNull();
LTFRAME_API LTEncodedJSValue JsTrue();
LTFRAME_API LTEncodedJSValue JsFalse();
LTFRAME_API LTEncodedJSValue String2JsValue(LTExecState es, const wchar_t* str);
LTFRAME_API int JsValue2Int(LTExecState es, LTEncodedJSValue v);
LTFRAME_API float JsValue2Float(LTExecState es, LTEncodedJSValue v);
LTFRAME_API double JsValue2Double(LTExecState es, LTEncodedJSValue v);
LTFRAME_API bool JsValue2Boolean(LTExecState es, LTEncodedJSValue v);
LTFRAME_API int JsValue2StringW(LTExecState es, LTEncodedJSValue v,wchar_t* result);
LTFRAME_API int JsValue2String(LTExecState es, LTEncodedJSValue v,char* result);
LTFRAME_API LTEncodedJSValue GetJsParameter(LTExecState es, int argIdx);
LTFRAME_API LTEncodedJSValue GetJsObjectParameter(LTExecState es,int argIdx,char* key);
LTFRAME_API LTEncodedJSValue JsCall(LTExecState es,LTEncodedJSValue func,LTEncodedJSValue* args, int argCount);
LTFRAME_API LTEncodedJSValue CreateJsObject(LTExecState es,LTEncodedJSValue o,char* key,LTEncodedJSValue value);
LTFRAME_API LTEncodedJSValue CreateObject(LTExecState es);
LTFRAME_API LTEncodedJSValue GetJsObjectAttribute(LTExecState es,LTEncodedJSValue v,char* key);

class I_LTFRAME_IWebView
{
public:
   virtual LRESULT HandleUserMessage(HWND, UINT, WPARAM, LPARAM)=0;
   virtual LPCWSTR GetWindowClassName()=0;
   virtual void ReleaseWindow()=0;
   virtual void OnFinalMessage(){};
   virtual	   bool  OnDrop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect){return true;};
	virtual   bool  OnDragLeave(){return true;};
	virtual   bool  OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect){return true;};
	virtual   bool  OnDragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect){return true;};
};
class LTFRAME_API WebView :public IDropTarget,public I_LTFRAME_IWebView{
public:
   LRESULT HandleMessage(HWND, UINT, WPARAM, LPARAM);
   void MessageLoop();
   void loadHTML(const wchar_t* html);
   void load(const wchar_t* html);
   void loadFile(const wchar_t* html);
   void SetViewTransparent(bool);
   void IsAutoGC(bool,int maxmemorysize=50);
   void CleanMemory(){};
   void SetWebSqlPath(const wchar_t* path=NULL);
   void SetLocalStoragePath(const wchar_t* path=NULL);
   LTEncodedJSValue RunJavaScript(const char* script);
   LTEncodedJSValue RunJavaScript(const wchar_t* script);
   LTExecState GetGlobalExecState();

   HWND windowHandle() const ;
   void CloseWindow();
   void QuitApp();
   
   void BindUserFunction(const char* name, LTNativeFunction fn, unsigned int argCount);
   template<class T> void my(const char* name, T* a,LTNativeFunction fn, unsigned int argCount);
   void GaussianBlurImage(HBITMAP hBitmap,int radius);
   void SetAllowKeyDownAutoScroll(bool);
   void EnableDragFrameChangeSize(bool);
   void ReleaseApplaction();
};



LTFRAME_API WebView* CreateLTFrameInstance(I_LTFRAME_IWebView * iwebview,LPCTSTR lpWindowName,HWND hWndParent,DWORD dwStyle,
               int x,int y,int nWidth,int nHeight,HICON icon);
 



#endif;


