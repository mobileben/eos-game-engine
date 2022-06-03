/******************************************************************************
 *
 * File: CoreSys.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * CoreSys supports HW support of the platform 
 * 
 *****************************************************************************/

#include "CoreSys.h"

CoreSys::CoreSys()
{
}

CoreSys::~CoreSys()
{
}

EOSError CoreSys::createVideoWindow(VideoWindow& videowin, VideoWindowInfo& videowininfo)
{
	EOSError	error = EOSErrorNone;

	error = videowin.create(videowininfo);

	return error;
}

EOSError CoreSys::destroyVideoWindow(VideoWindow& videowin)
{
	EOSError	error = EOSErrorNone;

	error = videowin.destroy();

	return error;
}

