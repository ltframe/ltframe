#pragma once
#include "oleidl.h"
class UIClass;
class DropFile
{
public:
	bool  DragEnter (IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	bool  DragOver (DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
	bool  DragLeave (void);
	bool  Drop (IDataObject * pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);

	// Constructor
	DropFile(HWND hwnd,UIClass* inst);
	~DropFile(void);

private:
	void DropData(HWND hwnd, IDataObject *pDataObject);
	// internal helper function
	DWORD DropEffect(DWORD grfKeyState, POINTL pt, DWORD dwAllowed);
	bool  QueryDataObject(IDataObject *pDataObject);


	// Private member variables
	LONG	m_lRefCount;
	HWND	m_hWnd;
	bool    m_fAllowDrop;

	IDataObject *m_pDataObject;
    UIClass* instanceclass;

};
