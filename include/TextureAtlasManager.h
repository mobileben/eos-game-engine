/******************************************************************************
 *
 * File: TextureAtlasManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Manager Object
 * 
 *****************************************************************************/

#ifndef __TEXTURE_ATLAS_MANAGER_H__
#define __TEXTURE_ATLAS_MANAGER_H__

#include "Platform.h"
#include "TextureAtlas.h"

class TextureAtlasManager : public EOSFrameworkComponent
{
private:
	Uint32			_maxAtlases;
	Uint32			_numAtlases;

	TextureAtlas*	_atlases;

	Uint32			_totalMemUsage;	

public:
	TextureAtlasManager();
	~TextureAtlasManager();

	TextureAtlas*	getFreeTextureAtlas(void);
	TextureAtlas*	findTextureAtlasFromRefID(ObjectID refID);
	TextureAtlas*	findTextureAtlasFromName(const Char* name);

	void			invalidate(void);

	void			destroyTextureAtlas(TextureAtlas& atlas);

	EOSError		allocateTextureAtlasPool(Uint32 max);
	void			deallocateTextureAtlasPool(void);

	void			updateUsage(void);
	Uint32			getNumUsedTextureAtlases(void);
	Uint32			getTotalMemoryUsage(void);

	void			dumpTextureAtlases(void);
};

#endif /* __TEXTURE_ATLAS_MANAGER_H__ */
