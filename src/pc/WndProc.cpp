/******************************************************************************
 *
 * File: WndProc.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * This is the WndProc routine used for PC platforms using EOS
 * 
 *****************************************************************************/

#include "pc/WndProc.h"
#include "App.h"
#include "EOSEventManager.h"

#ifdef _PLATFORM_PC

LRESULT CALLBACK EOSWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	EOSEventManager*	evtMgr = _appRefPtr->getEOSEventManager();

	switch (message)
	{
		case WM_CREATE:
			return 0;
		
		case WM_CLOSE:
			PostQuitMessage( 0 );
			return 0;
		
		case WM_DESTROY:
			return 0;
		
		case WM_KEYUP:
			evtMgr->interceptKeyboardEventUp((Uint32) wParam, (Uint32) lParam);
			return 0;

		case WM_KEYDOWN:
			switch ( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					return 0;

				default:
					evtMgr->interceptKeyboardEventDown((Uint32) wParam, (Uint32) lParam);
					return 0;
			}
			return 0;

		case WM_LBUTTONDOWN:
			evtMgr->interceptMouseEventLeftButtonDown((Uint32) wParam, (Uint32) lParam);
			return 0;

		case WM_LBUTTONUP:
			evtMgr->interceptMouseEventLeftButtonUp((Uint32) wParam, (Uint32) lParam);
			return 0;

		case WM_RBUTTONDOWN:
			evtMgr->interceptMouseEventRightButtonDown((Uint32) wParam, (Uint32) lParam);
			return 0;

		case WM_RBUTTONUP:
			evtMgr->interceptMouseEventRightButtonUp((Uint32) wParam, (Uint32) lParam);
			return 0;

		case WM_MBUTTONDOWN:
			evtMgr->interceptMouseEventMiddleButtonDown((Uint32) wParam, (Uint32) lParam);
			return 0;

		case WM_MBUTTONUP:
			evtMgr->interceptMouseEventMiddleButtonUp((Uint32) wParam, (Uint32) lParam);
			return 0;

		case WM_MOUSEMOVE:
			evtMgr->interceptMouseEventMove((Uint32) wParam, (Uint32) lParam);
			return 0;

		default:
			return DefWindowProc( hWnd, message, (Uint32) wParam, (Uint32) lParam );
	}
}

#else

	#error	THIS MODULE IS ONLY COMPATIBLE WITH _PLATFORM_PC TARGETS!

#endif /* _PLATFORM_PC */
