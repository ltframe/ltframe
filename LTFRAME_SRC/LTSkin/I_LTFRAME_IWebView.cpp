#include "StdAfx.h"
#include "InterFace.h"

bool  I_LTFRAME_IWebView::OnDrop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	return true;
}
bool  I_LTFRAME_IWebView::OnDragLeave()
{
	return true;
}
bool  I_LTFRAME_IWebView::OnDragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	return true;
}
bool  I_LTFRAME_IWebView::OnDragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
{
	return true;
}
void  I_LTFRAME_IWebView::OnFinalMessage()
{

}