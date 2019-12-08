#include "stdafx.h"
#include "DropFile.h"
#include "UIClass.h"

DropFile::DropFile(HWND hwnd,UIClass* inst)
{
	m_hWnd       = hwnd;
	m_fAllowDrop = false;
	instanceclass = inst;
}

DropFile::~DropFile(void)
{
}

void PositionCursor(HWND hwndEdit, POINTL pt)
{
	DWORD curpos; 
		
	// get the character position of mouse
	ScreenToClient(hwndEdit, (POINT *)&pt);
	curpos = SendMessage(hwndEdit, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y));

	// set cursor position
	SendMessage(hwndEdit, EM_SETSEL, LOWORD(curpos), LOWORD(curpos));
}

//
//	IUnknown::AddRef
//


//
//	QueryDataObject private helper routine
//
bool DropFile::QueryDataObject(IDataObject *pDataObject)
{
	FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

	// does the data object support CF_TEXT using a HGLOBAL?
	return pDataObject->QueryGetData(&fmtetc) == S_OK ? true : false;
}

//
//	DropEffect private helper routine
//
DWORD DropFile::DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed)
{
	DWORD dwEffect = 0;

	// 1. check "pt" -> do we allow a drop at the specified coordinates?
	
	// 2. work out that the drop-effect should be based on grfKeyState
	if(grfKeyState & MK_CONTROL)
	{
		dwEffect = dwAllowed & DROPEFFECT_COPY;
	}
	else if(grfKeyState & MK_SHIFT)
	{
		dwEffect = dwAllowed & DROPEFFECT_MOVE;
	}
	
	// 3. no key-modifiers were specified (or drop effect not allowed), so
	//    base the effect on those allowed by the dropsource
	if(dwEffect == 0)
	{
		if(dwAllowed & DROPEFFECT_COPY) dwEffect = DROPEFFECT_COPY;
		if(dwAllowed & DROPEFFECT_MOVE) dwEffect = DROPEFFECT_MOVE;
	}
	
	return dwEffect;
}


//
//	IDropTarget::DragEnter
//
//
//
bool DropFile::DragEnter(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{

	m_fAllowDrop = QueryDataObject(pDataObject);
	
	if(m_fAllowDrop)
	{
		// get the dropeffect based on keyboard state
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);

		SetFocus(m_hWnd);

		PositionCursor(m_hWnd, pt);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;

	// See if the dataobject contains any TEXT stored as a HGLOBAL
	if(pDataObject->QueryGetData(&fmtetc) == S_OK)
	{
		// Yippie! the data is there, so go get it!
		if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
		{
			// we asked for the data as a HGLOBAL, so access it appropriately
			PVOID data = GlobalLock(stgmed.hGlobal);


			//instanceclass->DropFiles(data);
			//SetWindowText(hwnd, (wchar_t *)data);
			instanceclass->DropEnter(data);
			GlobalUnlock(stgmed.hGlobal);

			// release the data using the COM API
			ReleaseStgMedium(&stgmed);
			return true;
		}
	}
	instanceclass->DropEnter(0);
	return true;



		
}

//
//	IDropTarget::DragOver
//
//
//
bool DropFile::DragOver(DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	if(m_fAllowDrop)
	{
		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
		PositionCursor(m_hWnd, pt);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	return true;
}

//
//	IDropTarget::DragLeave
//
bool  DropFile::DragLeave(void)
{
	instanceclass->DropLeave();
	return true;
}

//
//	IDropTarget::Drop
//
//
bool DropFile::Drop(IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect)
{
	PositionCursor(m_hWnd, pt);

	if(m_fAllowDrop)
	{
		DropData(m_hWnd, pDataObject);

		*pdwEffect = DropEffect(grfKeyState, pt, *pdwEffect);
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}

	return true;
}

void DropFile::DropData(HWND hwnd, IDataObject *pDataObject)
{
	// construct a FORMATETC object
	FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
	STGMEDIUM stgmed;

	// See if the dataobject contains any TEXT stored as a HGLOBAL
	if(pDataObject->QueryGetData(&fmtetc) == S_OK)
	{
		// Yippie! the data is there, so go get it!
		if(pDataObject->GetData(&fmtetc, &stgmed) == S_OK)
		{
			// we asked for the data as a HGLOBAL, so access it appropriately
			PVOID data = GlobalLock(stgmed.hGlobal);


			instanceclass->DropFiles(data);
			//SetWindowText(hwnd, (wchar_t *)data);

			GlobalUnlock(stgmed.hGlobal);

			// release the data using the COM API
			ReleaseStgMedium(&stgmed);
		}
	}
}
