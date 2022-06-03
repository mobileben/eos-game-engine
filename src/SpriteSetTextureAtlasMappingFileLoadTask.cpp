/******************************************************************************
 *
 * File: SpriteSetTextureAtlasMappingFileLoadTask.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas File Load Task Base Class
 * 
 *****************************************************************************/

#include "SpriteSetTextureAtlasMappingFileLoadTask.h"
#include "App.h"
#include "TaskManager.h"
#include "SpriteSetManager.h"

SpriteSetTextureAtlasMappingFileLoadTask::SpriteSetTextureAtlasMappingFileLoadTask() : _mappingID(0xFFFFFFFF), _name(NULL)
{
}

SpriteSetTextureAtlasMappingFileLoadTask::~SpriteSetTextureAtlasMappingFileLoadTask()
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}
}

void SpriteSetTextureAtlasMappingFileLoadTask::setSpriteSetTextureAtlasMappingID(ObjectID objid)
{
	_mappingID = objid;
}

void SpriteSetTextureAtlasMappingFileLoadTask::setName(const Char* name)
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

EOSError SpriteSetTextureAtlasMappingFileLoadTask::prologue(void)
{
	return FileLoadTask::prologue();
}

EOSError SpriteSetTextureAtlasMappingFileLoadTask::epilogue(void)
{
	EOSError			error = EOSErrorNone;
	SpriteSetManager*	spriteSetMgr;
	SpriteSetTextureAtlasMapping*			mapping;

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			spriteSetMgr = getTaskManager()->getApp()->getSpriteSetManager();

			mapping = spriteSetMgr->getFreeSpriteSetTextureAtlasMapping();

			AssertWDesc(mapping != NULL, "Could not allocate SpriteSetTextureAtlasMapping.");

			if (mapping)
			{
				error = mapping->create(_mappingID, _name, (Uint8*) _buffer, getFileSize());
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
