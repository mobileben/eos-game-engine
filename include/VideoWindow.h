/******************************************************************************
 *
 * File: VideoWindow.h
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

#ifndef __VIDEOWINDOW_H__
#define __VIDEOWINDOW_H__

#include "Platform.h"
#include "EOSError.h"

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

#ifndef _EOS_CONSOLE_TOOL
#include "OpenGLView.h"
#endif /* _EOS_CONSOLE_TOOL */

#endif /* _PLATFORM_MAC */

typedef struct
{
	Uint32	x;
	Uint32	y;
	Uint32 	width;
	Uint32 	height;

#ifdef _PLATFORM_PC

	HINSTANCE 	hInstance;
	UTF16*		titleName;
	UTF16*		className;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
#ifndef _EOS_CONSOLE_TOOL
	OpenGLView*	view;
#endif /* _EOS_CONSOLE_TOOL */
	
#elif defined(_PLATFORM_IPHONE)
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

} VideoWindowInfo;

class VideoWindow : public EOSObject
{
private:
	Uint32	_x;
	Uint32	_y;
	Uint32	_width;
	Uint32	_height;

#ifdef _PLATFORM_PC

	HINSTANCE	_hInstance;

	HWND		_hwnd;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
#ifndef _EOS_CONSOLE_TOOL
	OpenGLView*			_view;
#endif /* _EOS_CONSOLE_TOOL */
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

public:
	VideoWindow();
	~VideoWindow();

	EOSError	create(VideoWindowInfo& info);
	EOSError	destroy(void);

	inline Uint32	getWidth(void) const { return _width; }
	inline Uint32	getHeight(void) const { return _height; }

#ifdef _PLATFORM_PC

	inline HWND		getHWND(void) const { return _hwnd; }

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
#ifndef _EOS_CONSOLE_TOOL
	inline OpenGLView*	getOpenGLView(void) const { return _view; }
#endif /* _EOS_CONSOLE_TOOL */
	
#elif defined(_PLATFORM_IPHONE)
	
#else
	
#error _PLATFORM not defined.	
	
#endif /* _PLATFORM_PC*/

};


#endif /* __VIDEOWINDOW_H__ */

