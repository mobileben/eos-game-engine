/******************************************************************************
 *
 * File: SpriteSetFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Set File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __SPRITE_SET_FILE_LOAD_TASK_H__
#define __SPRITE_SET_FILE_LOAD_TASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "SpriteSet.h"

class SpriteSetFileLoadTask : public FileLoadTask
{
private:
	ObjectID		_spriteSetID;
	Char*			_name;
	TextureAtlas*	_textureAtlas;

public:
	SpriteSetFileLoadTask();
	~SpriteSetFileLoadTask();

	void					setSpriteSetID(ObjectID objid);
	void					setName(const Char* name);
	void					setTextureAtlas(TextureAtlas* atlas);

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
};

#endif /* __SPRITE_SET_FILE_LOAD_TASK_H__ */
