/******************************************************************************
 *
 * File: SpriteAnimSetFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * SpriteAnim Set File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __SPRITE_ANIM_SET_FILE_LOAD_TASK_H__
#define __SPRITE_ANIM_SET_FILE_LOAD_TASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "SpriteAnimSet.h"

class SpriteAnimSetFileLoadTask : public FileLoadTask
{
private:
	ObjectID		_spriteAnimSetID;
	Char*			_name;
	SpriteSet*		_spriteSet;

public:
	SpriteAnimSetFileLoadTask();
	~SpriteAnimSetFileLoadTask();

	void					setSpriteAnimSetID(ObjectID objid);
	void					setName(const Char* name);
	void					setSpriteSet(SpriteSet* set);

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
};

#endif /* __SPRITE_ANIM_SET_FILE_LOAD_TASK_H__ */
