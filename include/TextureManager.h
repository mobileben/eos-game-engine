/******************************************************************************
 *
 * File: TextureManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Texture Manager
 * 
 *****************************************************************************/

#ifndef __TEXTUREMANAGER_H__
#define __TEXTUREMANAGER_H__

#include "Platform.h"
#include "EOSError.h"
#include "Texture.h"

class TextureManager : public EOSFrameworkComponent
{
public:

private:
	Uint32		_maxTextures;
	Uint32		_maxRefIDs;
	Uint32		_numUsedTextures;

	Texture*	_textures;
	Uint32*		_refIDToTextureID;
	Uint32*		_refIDToHWTextureID;

	Boolean			_textureInfoNeedsFree;
	Uint32			_numTextureInfo;
	TextureInfo*	_textureInfo;

	Texture*	_colorBlendTexture;

	Uint32		_totalMemUsage;

	void		initTexture(Texture& texture);

public:
	TextureManager();
	~TextureManager();

	EOSError		allocateTexturePool(Uint32 texmax, Uint32 refmax);

	Texture*		getFreeTexture(void);

	Texture*		findTextureFromRefID(ObjectID refID);
	Texture*		findTextureFromName(const Char* name);

	EOSError		createTextureInfoDatabase(Uint32 num, TextureInfo* info, Boolean persistent);
	EOSError		createTextureInfoDatabase(const Char* filename, Uint8* buffer, Uint32 buffersize);

	void			destroyTextureInfoDatabase(void);
	TextureInfo*	findTextureInfoByRefID(ObjectID objid);
	TextureInfo*	findTextureInfoByRefName(const Char* name);

	void			invalidate(void);

	inline Texture* getColorBlendTexture(void) { return _colorBlendTexture; }

	Uint32			getNumUsedTextures(void);
	void			updateNumUsedTextures(void);

	Uint32			getTotalMemoryUsage(void);

	void			destroyTexture(Texture& texture);

	void			deallocateTexturePool(void);
};

#endif /* __TEXTUREMANAGER_H__ */
