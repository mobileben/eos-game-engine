/******************************************************************************
 *
 * File: SpriteSetManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Set Manager Object
 * 
 *****************************************************************************/

#ifndef __SPRITE_SET_MANAGER_H__
#define __SPRITE_SET_MANAGER_H__

#include "Platform.h"
#include "Graphics.h"
#include "SpriteSet.h"

class SpriteSetManager : public EOSFrameworkComponent
{
private:
	Uint32			_maxSets;
	Uint32			_numSets;

	SpriteSet*		_sets;

	Uint32							_maxSetAtlasMappings;
	Uint32							_numSetAtlasMappings;
	SpriteSetTextureAtlasMapping*	_setAtlasMappings;

	Uint32			_spriteSetTotalMemUsage;
	Uint32			_setAtlasMappingTotalMemUsage;

public:
	SpriteSetManager();
	~SpriteSetManager();

	SpriteSet*		getFreeSpriteSet(void);
	SpriteSet*		findSpriteSetFromRefID(ObjectID refID);
	SpriteSet*		findSpriteSetFromName(const Char* name);

	void			invalidate(void);

	void			destroySpriteSet(SpriteSet& set);

	EOSError		allocateSpriteSetPool(Uint32 max);
	void			deallocateSpriteSetPool(void);

	SpriteSetTextureAtlasMapping*		getFreeSpriteSetTextureAtlasMapping(void);
	SpriteSetTextureAtlasMapping*		findSpriteSetTextureAtlasMappingFromRefID(ObjectID refID);
	SpriteSetTextureAtlasMapping*		findSpriteSetTextureAtlasMappingFromName(const Char* name);

	EOSError		allocateSpriteSetTextureAtlasMappingPool(Uint32 max);
	void			deallocateSpriteSetTextureAtlasMappingPool(void);

	void			updateUsage(void);

	Uint32			getNumUsedSpriteSetTextureAtlasMappings(void);
	Uint32			getSpriteSetTextureAtlasMappingTotalMemoryUsage(void);

	Uint32			getNumUsedSpriteSets(void);
	Uint32			getSpriteSetTotalMemoryUsage(void);
};

#endif /* __SPRITE_SET_MANAGER_H__ */
