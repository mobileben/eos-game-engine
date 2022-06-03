/******************************************************************************
 *
 * File: SpriteSetTextureAtlasMappingFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Set File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __SPRITE_SET_TEXTURE_ATLAS_MAPPING_FILE_LOAD_TASK_H__
#define __SPRITE_SET_TEXTURE_ATLAS_MAPPING_FILE_LOAD_TASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "SpriteSet.h"

class SpriteSetTextureAtlasMappingFileLoadTask : public FileLoadTask
{
private:
	ObjectID		_mappingID;
	Char*			_name;

public:
	SpriteSetTextureAtlasMappingFileLoadTask();
	~SpriteSetTextureAtlasMappingFileLoadTask();

	void					setSpriteSetTextureAtlasMappingID(ObjectID objid);
	void					setName(const Char* name);

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
};

#endif /* __SPRITE_SET_TEXTURE_ATLAS_MAPPING_FILE_LOAD_TASK_H__ */
