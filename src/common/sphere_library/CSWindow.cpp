//
// CSWindow.cpp
//

#include "CSWindow.h"

#ifdef _WIN32

///////////////////////
// -CDialogBase

INT_PTR CALLBACK CDialogBase::DialogProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) // static
{
	CDialogBase * pDlg;
	if ( message == WM_INITDIALOG )
	{
		pDlg = reinterpret_cast<CDialogBase *>(lParam);
		ASSERT( pDlg );
		pDlg->m_hWnd = hWnd;	// OnCreate()
		pDlg->SetWindowLongPtr( GWLP_USERDATA, reinterpret_cast<INT_PTR>(pDlg) );
	}
	else
	{
		pDlg = static_cast<CDialogBase *>((LPVOID)::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}
	if ( pDlg )
	{
		return pDlg->DefDialogProc( message, wParam, lParam );
	}
	else
	{
		return FALSE;
	}
}

///////////////////////
// -CSWindowBase

ATOM CSWindowBase::RegisterClass( WNDCLASS & wc )	// static
{
	wc.lpfnWndProc = WndProc;
	return ::RegisterClass( &wc );
}

LRESULT WINAPI CSWindowBase::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) // static
{
	// NOTE: It is important to call OnDestroy() for asserts to work.
	CSWindowBase * pWnd;
	if ( message == WM_NCCREATE || message == WM_CREATE )
	{
		LPCREATESTRUCT lpCreateStruct = (LPCREATESTRUCT)lParam;
		ASSERT(lpCreateStruct);
		pWnd = static_cast<CSWindowBase *>(lpCreateStruct->lpCreateParams);
		ASSERT( pWnd );
		pWnd->m_hWnd = hWnd;	// OnCreate()
		pWnd->SetWindowLongPtr(GWLP_USERDATA, reinterpret_cast<INT_PTR>(pWnd));
	}
	pWnd = static_cast<CSWindowBase *>((LPVOID)::GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return ( pWnd ? pWnd->DefWindowProc(message, wParam, lParam) : ::DefWindowProc(hWnd, message, wParam, lParam) );
}

#endif // _WIN32
