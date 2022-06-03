/******************************************************************************
 *
 * File: TextureAtlasFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __TEXTURE_ATLAS_FILE_LOAD_TASK_H__
#define __TEXTURE_ATLAS_FILE_LOAD_TASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "Texture.h"

class TextureAtlasFileLoadTask : public FileLoadTask
{
private:
	ObjectID	_atlasID;
	Texture*	_texture;
	Char*		_name;

public:
	TextureAtlasFileLoadTask();
	~TextureAtlasFileLoadTask();

	void					setAtlasID(ObjectID objid);
	void					setTexture(Texture* tex);
	void					setName(const Char* name);

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
};

#endif /* __TEXTURE_ATLAS_FILE_LOAD_TASK_H__ */
