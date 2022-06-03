/******************************************************************************
 *
 * File: SpriteAnimSetManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Anim Manager Set Object
 * 
 *****************************************************************************/

#include "SpriteAnimSetManager.h"
#include "App.h"

SpriteAnimSetManager::SpriteAnimSetManager()
{
	_maxSets = 0;
	_numSets = 0;

	_sets = NULL;

	_totalMemUsage = 0;
}

SpriteAnimSetManager::~SpriteAnimSetManager()
{
	deallocateSpriteAnimSetPool();
}

SpriteAnimSet* SpriteAnimSetManager::getFreeSpriteAnimSet(void)
{
	SpriteAnimSet* 	set = NULL;
	Uint32			i;

	if (_sets)
	{
		for (i=0;i<_maxSets;i++)
		{
			if (_sets[i].isUsed() == false)
			{
				set = &_sets[i];
				set->setUsed(true);

				invalidate();
				break;
			}
		}
	}

	return set;
}

SpriteAnimSet* SpriteAnimSetManager::findSpriteAnimSetFromRefID(ObjectID refID)
{
	SpriteAnimSet* 	set = NULL;
	Uint32			i;

	if (_sets)
	{
		for (i=0;i<_maxSets;i++)
		{
			if (_sets[i].isUsed() == true)
			{
				if (_sets[i].getRefID() == refID)
				{
					set = &_sets[i];
					break;
				}
			}
		}
	}

	return set;
}

SpriteAnimSet* SpriteAnimSetManager::findSpriteAnimSetFromName(const Char* name)
{
	SpriteAnimSet* set = NULL;
	Uint32			i;

	if (_sets)
	{
		for (i=0;i<_maxSets;i++)
		{
			if (_sets[i].isUsed() == true)
			{
				if (!strcmp(_sets[i].getName(), name))
				{
					set = &_sets[i];
					break;
				}
			}
		}
	}

	return set;
}

void SpriteAnimSetManager::invalidate(void)
{
}

void SpriteAnimSetManager::completelyDestroySpriteAnimSet(const Char* name)
{
	SpriteAnimSet*	aset = findSpriteAnimSetFromName(name);

	if (aset)
	{
		completelyDestroySpriteAnimSet(*aset);
	}
}

void SpriteAnimSetManager::completelyDestroySpriteAnimSet(SpriteAnimSet& set)
{
	if (set.isUsed())
	{
		SpriteSet*			spriteSet;
		TextureAtlas*		atlas;
		const TextureState*	tstate;
		Texture*			tex;
		Uint32				i;

		spriteSet = set.getSpriteSet();

		if (spriteSet)
		{
			for (i=0;i<spriteSet->getNumTextureAtlas();i++)
			{
				atlas = spriteSet->getTextureAtlas(i);

				if (atlas)
				{
					tstate = atlas->getTextureState();

					tex = tstate->_textureObj;

					if (tex)
						_appRefPtr->getTextureManager()->destroyTexture(*tex);

					_appRefPtr->getTextureAtlasManager()->destroyTextureAtlas(*atlas);
				}
			}

			_appRefPtr->getSpriteSetManager()->destroySpriteSet(*spriteSet);
		}

		destroySpriteAnimSet(set);
	}
}

void SpriteAnimSetManager::destroySpriteAnimSet(SpriteAnimSet& set)
{
	if (set.isUsed())
	{
		set.setUsed(false);
		set.destroy();
	}
}

EOSError SpriteAnimSetManager::allocateSpriteAnimSetPool(Uint32 max)
{
	EOSError 	error = EOSErrorNone;
	Uint32		i;

	AssertWDesc(max != 0, "SpriteAnimSetManager::allocateSpriteAnimSetPool(): max must be != 0");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateSpriteAnimSetPool();

	_sets = new SpriteAnimSet[max];

	AssertWDesc(_sets != NULL, "SpriteAnimSetManager::allocateSpriteAnimSetPool(): _sets is NULL");

	if (_sets)
	{
		for (i=0;i<max;i++)
		{
			_sets[i].setSpriteAnimSetManager(this);
		}

		_maxSets = max;
		_numSets = 0;
	}
	else
	{
		error = EOSErrorNoMemory;
	}

	return error;
}

void SpriteAnimSetManager::deallocateSpriteAnimSetPool(void)
{
	Uint32	i;

	if (_sets)
	{
		for (i=0;i<_maxSets;i++)
		{
			if (_sets[i].isUsed())
			{
				_sets[i].setUsed(false);
			}
		}

		delete [] _sets;
		_sets = NULL;
	}

	_maxSets = 0;
	_numSets = 0;
}

void SpriteAnimSetManager::updateUsage(void)
{
	Uint32	i;

	_numSets = 0;
	_totalMemUsage = 0;

	if (_sets)
	{
		for (i=0;i<_maxSets;i++)
		{
			if (_sets[i].isUsed())
			{
				_numSets++;
				_totalMemUsage += _sets[i].getMemoryUsage();
			}
		}
	}
}

Uint32 SpriteAnimSetManager::getNumUsedSpriteAnimSets(void)
{
	return _numSets;
}

Uint32 SpriteAnimSetManager::getTotalMemoryUsage(void)
{
	return _totalMemUsage;
}

