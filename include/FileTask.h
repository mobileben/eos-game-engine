/******************************************************************************
 *
 * File: FileTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * FileTask is a basic file access task. This accomodates for async and sync
 * file access, allowing us to create a programming framework that can easily
 * handle both situations using a similar code base.
 * 
 *****************************************************************************/

#ifndef __FILETASK_H__
#define __FILETASK_H__

#include "Platform.h"
#include "File.h"

class FileTask : public Task
{
private:
#ifdef _PLATFORM_PC

#elif defined(_PLATFORM_MAC)
	
#else

	#error _PLATFORM must be defined.

#endif /* _PLATFORM_PC */

public:
		
};

#endif /* __FILETASK_H__ */
