/******************************************************************************
 *
 * File: SpriteAnimSetFileLoadTask.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas File Load Task Base Class
 * 
 *****************************************************************************/

#include "SpriteAnimSetFileLoadTask.h"
#include "App.h"
#include "TaskManager.h"
#include "SpriteAnimSetManager.h"

SpriteAnimSetFileLoadTask::SpriteAnimSetFileLoadTask() : _spriteSet(NULL), _spriteAnimSetID(0xFFFFFFFF), _name(NULL)
{
}

SpriteAnimSetFileLoadTask::~SpriteAnimSetFileLoadTask()
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}
}

void SpriteAnimSetFileLoadTask::setSpriteAnimSetID(ObjectID objid)
{
	_spriteAnimSetID = objid;
}

void SpriteAnimSetFileLoadTask::setName(const Char* name)
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	if (name)
	{
		_name = new Char[strlen(name) + 1];

		if (_name)
			strcpy(_name, name);
	}
}

void SpriteAnimSetFileLoadTask::setSpriteSet(SpriteSet* set)
{
	_spriteSet = set;
}

EOSError SpriteAnimSetFileLoadTask::prologue(void)
{
	return FileLoadTask::prologue();
}

EOSError SpriteAnimSetFileLoadTask::epilogue(void)
{
	EOSError				error = EOSErrorNone;
	SpriteAnimSetManager*	spriteAnimSetMgr;
	SpriteAnimSet*			set;

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			spriteAnimSetMgr = getTaskManager()->getApp()->getSpriteAnimSetManager();

			set = spriteAnimSetMgr->getFreeSpriteAnimSet();

			AssertWDesc(set != NULL, "Could not allocate SpriteAnimSet.");

			if (set)
			{
				error = set->create(_spriteAnimSetID, _name, _spriteSet, (Uint8*) _buffer, getFileSize());
			}
			else
				error = EOSErrorResourceNotAvailable;
		}
		else
			error = EOSErrorNULL;
	}
	else
		error = EOSErrorNULL;

	return error;
}
