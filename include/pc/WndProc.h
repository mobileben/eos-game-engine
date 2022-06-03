/******************************************************************************
 *
 * File: WndProc.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * This is the WndProc routine used for PC platforms using EOS
 * 
 *****************************************************************************/

#ifndef __WNDPROC_H__
#define __WNDPROC_H__

#include "Platform.h"

#ifdef _PLATFORM_PC

extern LRESULT CALLBACK EOSWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#else

	#error	THIS MODULE IS ONLY COMPATIBLE WITH _PLATFORM_PC TARGETS!

#endif /* _PLATFORM_PC */

#endif /* __WNDPROC_H__ */

