/******************************************************************************
 *
 * File: TextureFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Texture File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __TEXTUREFILELOADTASK_H__
#define __TEXTUREFILELOADTASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "Texture.h"

class TextureFileLoadTask : public FileLoadTask
{
private:
	TextureInfo		_textureInfo;
	Texture*		_texture;

public:
	TextureFileLoadTask();
	~TextureFileLoadTask();

	void 					setTextureInfo(TextureInfo& textureInfo);

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
};

#endif /* __TEXTUREFILELOADTASK_H__ */

