/******************************************************************************
 *
 * File: FontSetFileLoadTask.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Font Set File Load Task Base Class
 * 
 *****************************************************************************/

#include "Platform.h"
#include "App.h"
#include "TaskManager.h"
#include "FontSetFileLoadTask.h"

FontSetFileLoadTask::FontSetFileLoadTask() : _fontMgr(NULL)
{
}

FontSetFileLoadTask::~FontSetFileLoadTask()
{
}

void FontSetFileLoadTask::setFontManager(FontManager* mgr)
{
	_fontMgr = mgr;
}

EOSError FontSetFileLoadTask::prologue(void)
{
	return FileLoadTask::prologue();
}

EOSError FontSetFileLoadTask::epilogue(void)
{
	EOSError			error = EOSErrorNone;

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			if (_fontMgr)
			{
				error = _fontMgr->createFontSet((Uint8*) _buffer, getFileSize());
			}
		}
		else
			error = EOSErrorNULL;
	}
	else
		error = EOSErrorNULL;

	return error;
}


