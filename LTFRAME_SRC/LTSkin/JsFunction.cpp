#include "config.h"
#include "InterFace.h"
#include <ScriptController.h>
#include <ScriptValue.h>
#include <BString.h>
#include <JavaScriptCore/CallData.h>
//wchar_t* WtfToWChar(){
//	int strlen = message.length();
//	wchar_t* str =new wchar_t[message.length()];
//	wcscpy(str,(wchar_t*)message.characters());
//	str[strlen-1] = '\0';
//	MessageBox(NULL,str,NULL,NULL);
//	delete[] str;
//}

typedef enum
{	
	JSTYPE_OBJECT,
	JSTYPE_FUNCTION,
	JSTYPE_UNDEFINED,
	JSTYPE_NUMBER,
	JSTYPE_STRING,
	JSTYPE_BOOLEAN,
}JsVlaueType;



LTFRAME_API LTEncodedJSValue Int2JsValue(int n)
{
	return JSC::JSValue::encode(JSC::jsNumber(n));
}

LTFRAME_API LTEncodedJSValue Float2JsValue(float f)
{
	return JSC::JSValue::encode(JSC::jsDoubleNumber(f));
}

LTFRAME_API LTEncodedJSValue Double2JsValue(double d)
{
	return JSC::JSValue::encode(JSC::jsDoubleNumber(d));
}

LTFRAME_API LTEncodedJSValue Boolean2JsValue(bool b)
{
	return JSC::JSValue::encode(JSC::jsBoolean(b));
}


LTFRAME_API LTEncodedJSValue JsUndefined()
{
	return JSC::JSValue::encode(JSC::jsUndefined());
}

LTFRAME_API LTEncodedJSValue JsNull()
{
	return JSC::JSValue::encode(JSC::jsNull());
}

LTFRAME_API LTEncodedJSValue JsTrue()
{
	return JSC::JSValue::encode(JSC::jsBoolean(true));
}

LTFRAME_API LTEncodedJSValue JsFalse()
{
	return JSC::JSValue::encode(JSC::jsBoolean(false));
}


LTFRAME_API LTEncodedJSValue String2JsValue(LTExecState es, const wchar_t* str)
{
	JSC::JSValue value = JSC::jsString((JSC::ExecState*)es, JSC::UString(str));
	return JSC::JSValue::encode(value);
}


LTFRAME_API int JsValue2Int(LTExecState es, LTEncodedJSValue v)
{
	JSC::JSValue value = JSC::JSValue::decode(v);
	return value.toInt32((JSC::ExecState*)es);
}

LTFRAME_API float JsValue2Float(LTExecState es, LTEncodedJSValue v)
{
	JSC::JSValue value = JSC::JSValue::decode(v);
	return value.toFloat((JSC::ExecState*)es);
}

LTFRAME_API double JsValue2Double(LTExecState es, LTEncodedJSValue v)
{
	JSC::JSValue value = JSC::JSValue::decode(v);
	return value.toNumber((JSC::ExecState*)es);
}

LTFRAME_API bool JsValue2Boolean(LTExecState es, LTEncodedJSValue v)
{
	JSC::JSValue value = JSC::JSValue::decode(v);
	return value.toBoolean((JSC::ExecState*)es);
}

LTFRAME_API int JsValue2StringW(LTExecState es, LTEncodedJSValue v,wchar_t* result)
{
	JSC::JSValue value = JSC::JSValue::decode(v);
	JSC::UString str = value.toString((JSC::ExecState*)es)->value((JSC::ExecState*)es);
	//return StringTableW::addString(str.characters(), str.length());
	//String message(str.impl());
	//return message.characters();
	if(result==0)
	{
		return str.length();
	}
	wcscpy(result,WebCore::BString(str));
	return str.length();
		//    JSC::JSValue value = JSC::jsString((JSC::ExecState*)es, JSC::UString(L"的防守打法是的发生的"));
	//    return JSC::JSValue::encode(value);
}

LTFRAME_API int JsValue2String(LTExecState es, LTEncodedJSValue v,char* returnpszMultiByte)
{

	JSC::JSValue value = JSC::JSValue::decode(v);
	JSC::UString str = value.toString((JSC::ExecState*)es)->value((JSC::ExecState*)es);
    wchar_t *result = (wchar_t*)malloc((str.length()+1)*sizeof(wchar_t)); 
	ZeroMemory(result,(str.length()+1)*sizeof(wchar_t));
	wcscpy(result,WebCore::BString(str));
	int iSize = WideCharToMultiByte(CP_ACP, 0,result, -1, NULL, 0, NULL, NULL); 
	if(returnpszMultiByte==0){
		free(result);
		return iSize;
	}
	WideCharToMultiByte(CP_ACP, 0, result, -1, returnpszMultiByte, iSize, NULL, NULL); 
	free(result);
	return iSize;

}


LTFRAME_API LTEncodedJSValue GetJsParameter(LTExecState es, int argIdx)
{
	JSC::ExecState* exec = (JSC::ExecState*)es;
	JSC::JSValue value = exec->argument(argIdx);
	int x = (int)exec->argumentCount(); 


	//LTEncodedJSValue v  = JSC::JSValue::encode(Int2JsValue(8));
//JsCall(es,exec->argument(0),0,0);


	return JSC::JSValue::encode(value);
}

LTFRAME_API LTEncodedJSValue GetJsObjectParameter(LTExecState es,int argIdx,char* key)
{
	JSC::ExecState* exec = (JSC::ExecState*)es;
	JSC::JSObject* jsAttrs = exec->argument(argIdx).getObject();
	JSC::Identifier alpha(exec, key);
	JSC::JSValue value  = jsAttrs->get(exec, alpha);
	return JSC::JSValue::encode(value);
}

LTFRAME_API LTEncodedJSValue GetJsObjectAttribute(LTExecState es,LTEncodedJSValue v,char* key)
{
	JSC::ExecState* exec = (JSC::ExecState*)es;
	//JSC::JSObject* jsAttrs = exec->argument(argIdx).getObject();
	
	JSC::JSObject* object = JSC::JSValue::decode(v).toObject(exec);
	//JSC::JSObject* object = JSC::JSValue::decode(v).getObject();

	JSC::Identifier alpha(exec, key);
	JSC::JSValue value  = object->get(exec, alpha);
	return JSC::JSValue::encode(value);
}



LTFRAME_API LTEncodedJSValue CreateJsObject(LTExecState es,LTEncodedJSValue o,char* key,LTEncodedJSValue value){
	JSC::ExecState* exec = (JSC::ExecState*)es;
	JSC::JSObject* object = JSC::JSValue::decode(o).toObject(exec);
	object->putDirect(exec->globalData(), JSC::Identifier(exec, key), JSC::JSValue::decode(value));
	return JSC::JSValue::encode(object);
    
}

LTFRAME_API LTEncodedJSValue CreateObject(LTExecState es){
	JSC::ExecState* exec = (JSC::ExecState*)es;
	JSC::JSObject* object = constructEmptyObject(exec);
	return JSC::JSValue::encode(object);
}

LTFRAME_API LTEncodedJSValue JsCall(LTExecState es,LTEncodedJSValue func,LTEncodedJSValue* args, int argCount){

	JSC::ExecState* exec = (JSC::ExecState*)es;
	JSC::JSValue jsThisValue = exec->globalThisValue();
	JSC::MarkedArgumentBuffer argList;
	for(int i = 0;i<argCount;i++){
		argList.append(JSC::JSValue::decode(args[i]));
	}

	JSC::CallData callData;
	JSC::JSObject* object = JSC::JSValue::decode(func).toObject(exec);
	JSC::CallType callType = object->methodTable()->getCallData(object, callData);
	if(callType==JSC::CallTypeNone){
		return JsUndefined();
	}
	JSC::JSValue value = JSC::call(exec, object, JSC::CallTypeJS, callData, jsThisValue, argList);
	return JSC::JSValue::encode(value);
}

JsVlaueType GetJsVlaueType(LTEncodedJSValue v)
{
    JSC::JSValue value = JSC::JSValue::decode(v);

    if (value.isUndefined())
        return JSTYPE_UNDEFINED;

    if (value.isBoolean())
        return JSTYPE_BOOLEAN;

    if (value.isNumber())
        return JSTYPE_NUMBER;

    if (value.isString())
        return JSTYPE_STRING;

    if (value.isObject()) {
        // Return "undefined" for objects that should be treated
        // as null when doing comparisons.
        if (JSC::asObject(value)->structure()->typeInfo().masqueradesAsUndefined())
            return JSTYPE_UNDEFINED;

        JSC::CallData callData;
        JSC::JSObject* object = JSC::asObject(value);
        if (object->methodTable()->getCallData(object, callData) != JSC::CallTypeNone)
            return JSTYPE_FUNCTION;
    }

    return JSTYPE_OBJECT;
}

LTFRAME_API bool IsJsNumber(LTEncodedJSValue v)
{
    return GetJsVlaueType(v) == JSTYPE_NUMBER ? true : false;
}

LTFRAME_API bool IsJsString(LTEncodedJSValue v)
{
    return GetJsVlaueType(v) == JSTYPE_STRING ? true : false;
}

LTFRAME_API bool IsJsBoolean(LTEncodedJSValue v)
{
    return GetJsVlaueType(v) == JSTYPE_BOOLEAN ? true : false;
}

LTFRAME_API bool IsJsObject(LTEncodedJSValue v)
{
    return GetJsVlaueType(v) == JSTYPE_OBJECT ? true : false;
}

LTFRAME_API bool IsJsFunction(LTEncodedJSValue v)
{
    return GetJsVlaueType(v) == JSTYPE_FUNCTION ? true : false;
}

LTFRAME_API bool IsJsUndefined(LTEncodedJSValue v)
{
    return GetJsVlaueType(v) == JSTYPE_UNDEFINED ? true : false;
}

LTFRAME_API bool IsJsNull(LTEncodedJSValue v)
{
    JSC::JSValue value = JSC::JSValue::decode(v);
    return value.isNull();
}

LTFRAME_API bool IsJsArray(LTEncodedJSValue v)
{
    JSC::JSValue value = JSC::JSValue::decode(v);
    if (!value.isObject())
        return false;

    return value.inherits(&JSC::JSArray::s_info);
}

LTFRAME_API bool IsJsTrue(LTEncodedJSValue v)
{
    JSC::JSValue value = JSC::JSValue::decode(v);
    return value.isTrue();
}

LTFRAME_API bool IsJsFalse(LTEncodedJSValue v)
{
    JSC::JSValue value = JSC::JSValue::decode(v);
    return value.isFalse();
}

