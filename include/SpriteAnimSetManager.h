/******************************************************************************
 *
 * File: SpriteAnimSetManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * SpriteAnim Set Manager Object
 * 
 *****************************************************************************/

#ifndef __SPRITE_ANIM_SET_MANAGER_H__
#define __SPRITE_ANIM_SET_MANAGER_H__

#include "Platform.h"
#include "Graphics.h"
#include "SpriteAnimSet.h"

class SpriteAnimSetManager : public EOSFrameworkComponent
{
private:
	Uint32				_maxSets;
	Uint32				_numSets;

	SpriteAnimSet*		_sets;

	Uint32				_totalMemUsage;

public:
	SpriteAnimSetManager();
	~SpriteAnimSetManager();

	SpriteAnimSet*	getFreeSpriteAnimSet(void);
	SpriteAnimSet*	findSpriteAnimSetFromRefID(ObjectID refID);
	SpriteAnimSet*	findSpriteAnimSetFromName(const Char* name);

	void			invalidate(void);

	void			destroySpriteAnimSet(SpriteAnimSet& set);

	void			completelyDestroySpriteAnimSet(const Char* name);
	void			completelyDestroySpriteAnimSet(SpriteAnimSet& set);

	EOSError		allocateSpriteAnimSetPool(Uint32 max);
	void			deallocateSpriteAnimSetPool(void);

	void			updateUsage(void);
	
	Uint32			getNumUsedSpriteAnimSets(void);	
	Uint32			getTotalMemoryUsage(void);
};

#endif /* __SPRITE_ANIM_SET_MANAGER_H__ */
