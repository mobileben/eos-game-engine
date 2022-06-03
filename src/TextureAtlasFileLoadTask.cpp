/******************************************************************************
 *
 * File: TextureAtlasFileLoadTask.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas File Load Task Base Class
 * 
 *****************************************************************************/

#include "TextureAtlasFileLoadTask.h"
#include "App.h"
#include "TaskManager.h"
#include "TextureAtlasManager.h"

TextureAtlasFileLoadTask::TextureAtlasFileLoadTask() : _texture(NULL), _atlasID(0xFFFFFFFF), _name(NULL)
{
}

TextureAtlasFileLoadTask::~TextureAtlasFileLoadTask()
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}
}

void TextureAtlasFileLoadTask::setAtlasID(ObjectID objid)
{
	_atlasID = objid;
}

void TextureAtlasFileLoadTask::setTexture(Texture* tex)
{
	_texture = tex;
}

void TextureAtlasFileLoadTask::setName(const Char* name)
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

EOSError TextureAtlasFileLoadTask::prologue(void)
{
	return FileLoadTask::prologue();
}

EOSError TextureAtlasFileLoadTask::epilogue(void)
{
	EOSError				error = EOSErrorNone;
	TextureAtlasManager*	atlasMgr;
	TextureAtlas*			atlas;

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			atlasMgr = getTaskManager()->getApp()->getTextureAtlasManager();

			atlas = atlasMgr->getFreeTextureAtlas();

			AssertWDesc(atlas != NULL, "Cloud not allocate TextureAtlas.");

			if (atlas)
			{
				error = atlas->create(_atlasID, _name, _texture, (Uint8*) _buffer, getFileSize());
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
