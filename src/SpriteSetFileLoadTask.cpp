/******************************************************************************
 *
 * File: SpriteSetFileLoadTask.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas File Load Task Base Class
 * 
 *****************************************************************************/

#include "SpriteSetFileLoadTask.h"
#include "App.h"
#include "TaskManager.h"
#include "SpriteSetManager.h"

SpriteSetFileLoadTask::SpriteSetFileLoadTask() : _textureAtlas(NULL), _spriteSetID(0xFFFFFFFF), _name(NULL)
{
}

SpriteSetFileLoadTask::~SpriteSetFileLoadTask()
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}
}

void SpriteSetFileLoadTask::setSpriteSetID(ObjectID objid)
{
	_spriteSetID = objid;
}

void SpriteSetFileLoadTask::setName(const Char* name)
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

void SpriteSetFileLoadTask::setTextureAtlas(TextureAtlas* atlas)
{
	_textureAtlas = atlas;
}

EOSError SpriteSetFileLoadTask::prologue(void)
{
	return FileLoadTask::prologue();
}

EOSError SpriteSetFileLoadTask::epilogue(void)
{
	EOSError			error = EOSErrorNone;
	SpriteSetManager*	spriteSetMgr;
	SpriteSet*			set;

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			spriteSetMgr = getTaskManager()->getApp()->getSpriteSetManager();

			set = spriteSetMgr->getFreeSpriteSet();

			AssertWDesc(set != NULL, "Could not allocate SpriteSet.");

			if (set)
			{
				error = set->create(_spriteSetID, _name, _textureAtlas, (Uint8*) _buffer, getFileSize());
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
