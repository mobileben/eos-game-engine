/******************************************************************************
 *
 * File: VideoWindow.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * VideoWindow is the immediate interface to the platform's visual system.
 * For OS such as Windows or MAC OS, this is the actual window instance. For
 * systems such as consoles, this is an abstraction for the video HW.
 * 
 *****************************************************************************/

#include "VideoWindow.h"

#ifdef _PLATFORM_PC
#include "pc/WndProc.h"
#endif

VideoWindow::VideoWindow()
{
	_x = _y = _width = _height = 0;


#ifdef _PLATFORM_PC

	_hwnd = NULL;
	
#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	_view =  nil;

#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

VideoWindow::~VideoWindow()
{
	//	Just in case
	destroy();
}

EOSError VideoWindow::create(VideoWindowInfo& info)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC

	if (_hwnd == NULL)
	{
		WNDCLASS	wndclass;
		RECT		rect;
		Uint32 		style;

		wndclass.style = CS_OWNDC;
		wndclass.lpfnWndProc = EOSWndProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = info.hInstance;
		wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = info.className;
	
		RegisterClass(&wndclass);

		style = WS_CAPTION | WS_POPUPWINDOW | WS_VISIBLE;

		rect.top = 0;
		rect.bottom = info.height;

		rect.left = 0;
		rect.right = info.width;

		AdjustWindowRect(&rect, style, false);

		if (rect.left < 0)
			rect.right -= rect.left;
		else
			rect.right += rect.left;

		if (rect.top < 0)
			rect.bottom -= rect.top;
		else
			rect.bottom += rect.top;

		_hwnd = CreateWindow(info.className, info.titleName,
							style,
							info.x, info.y, rect.right, rect.bottom,
							NULL, NULL, info.hInstance, NULL);
	
		AssertWDesc(_hwnd != NULL, "CreateWindow failed.");
	
		if (_hwnd != NULL)
			_hInstance = info.hInstance;
		else
			error = EOSErrorResourceCreationFailure;
	}
	else
		error = EOSErrorAlreadyInitialized;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	if (_view == nil)
	{
		_view = info.view;
	}
	else
		error = EOSErrorAlreadyInitialized;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

	if (error == EOSErrorNone)
	{
		_x = info.x;
		_y = info.y;
		_width = info.width;
		_height = info.height;
	}

	return error;
}

EOSError VideoWindow::destroy(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC

	if (_hwnd)
	{
		DestroyWindow(_hwnd);

		_hwnd = NULL;
	}
	
#elif defined(_PLATFORM_MAC)
	
#elif defined(_PLATFORM_IPHONE)
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */


	return error;
}

