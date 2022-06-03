/******************************************************************************
 *
 * File: CoreSys.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * CoreSys supports HW support of the platform 
 * 
 *****************************************************************************/

#ifndef __CORESYS_H__
#define __CORESYS_H__

#include "Platform.h"
#include "EOSError.h"
#include "VideoWindow.h"

class CoreSys : public EOSFrameworkComponent
{
private:
#ifdef _PLATFORM_PC

#elif defined(_PLATFORM_MAC)
	
#elif defined(_PLATFORM_IPHONE)
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

public:
	CoreSys();
	~CoreSys();

	EOSError	createVideoWindow(VideoWindow& videowin, VideoWindowInfo& videowininfo);
	EOSError	destroyVideoWindow(VideoWindow& videowin);
};

#endif /* __CORESYS_H__ */

