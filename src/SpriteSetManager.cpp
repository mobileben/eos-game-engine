/******************************************************************************
 *
 * File: SpriteSetManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Set Manager Object
 * 
 *****************************************************************************/

#include "SpriteSetManager.h"

SpriteSetManager::SpriteSetManager()
{
	_maxSets = 0;
	_numSets = 0;

	_sets = NULL;

	_maxSetAtlasMappings = 0;
	_numSetAtlasMappings = 0;

	_spriteSetTotalMemUsage = 0;
	_setAtlasMappingTotalMemUsage = 0;

	_setAtlasMappings = NULL;
}

SpriteSetManager::~SpriteSetManager()
{
	deallocateSpriteSetPool();
	deallocateSpriteSetTextureAtlasMappingPool();
}

SpriteSet* SpriteSetManager::getFreeSpriteSet(void)
{
	SpriteSet* 		set = NULL;
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

SpriteSet* SpriteSetManager::findSpriteSetFromRefID(ObjectID refID)
{
	SpriteSet*	 	set = NULL;
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

SpriteSet* SpriteSetManager::findSpriteSetFromName(const Char* name)
{
	SpriteSet* 	set = NULL;
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

void SpriteSetManager::invalidate(void)
{
}

void SpriteSetManager::destroySpriteSet(SpriteSet& set)
{
	if (set.isUsed())
	{
		set.setUsed(false);
		set.destroy();
	}
}

EOSError SpriteSetManager::allocateSpriteSetPool(Uint32 max)
{
	EOSError error = EOSErrorNone;
	Uint32		i;

	AssertWDesc(max != 0, "SpriteSetManager::allocateSpriteSetPool(): max must be != 0");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateSpriteSetPool();

	_sets = new SpriteSet[max];

	AssertWDesc(_sets != NULL, "SpriteSetManager::allocateSpriteSetPool(): _sets is NULL");

	if (_sets)
	{
		for (i=0;i<max;i++)
		{
			_sets[i].setSpriteSetManager(this);
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

void SpriteSetManager::deallocateSpriteSetPool(void)
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

SpriteSetTextureAtlasMapping* SpriteSetManager::getFreeSpriteSetTextureAtlasMapping(void)
{
	SpriteSetTextureAtlasMapping* 		mapping = NULL;
	Uint32			i;

	if (_setAtlasMappings)
	{
		for (i=0;i<_maxSetAtlasMappings;i++)
		{
			if (_setAtlasMappings[i].isUsed() == false)
			{
				mapping = &_setAtlasMappings[i];
				mapping->setUsed(true);

				invalidate();
				break;
			}
		}
	}

	return mapping;
}

SpriteSetTextureAtlasMapping* SpriteSetManager::findSpriteSetTextureAtlasMappingFromRefID(ObjectID refID)
{
	SpriteSetTextureAtlasMapping*	 	mapping = NULL;
	Uint32			i;

	if (_setAtlasMappings)
	{
		for (i=0;i<_maxSetAtlasMappings;i++)
		{
			if (_setAtlasMappings[i].isUsed() == true)
			{
				if (_setAtlasMappings[i].getRefID() == refID)
				{
					mapping = &_setAtlasMappings[i];
					break;
				}
			}
		}
	}

	return mapping;
}

SpriteSetTextureAtlasMapping* SpriteSetManager::findSpriteSetTextureAtlasMappingFromName(const Char* name)
{
	SpriteSetTextureAtlasMapping* 	set = NULL;
	Uint32			i;

	if (_setAtlasMappings)
	{
		for (i=0;i<_maxSetAtlasMappings;i++)
		{
			if (_setAtlasMappings[i].isUsed() == true)
			{
				if (!strcmp(_setAtlasMappings[i].getName(), name))
				{
					set = &_setAtlasMappings[i];
					break;
				}
			}
		}
	}

	return set;
}

EOSError SpriteSetManager::allocateSpriteSetTextureAtlasMappingPool(Uint32 max)
{
	EOSError error = EOSErrorNone;
	Uint32	i;

	AssertWDesc(max != 0, "SpriteSetManager::allocateSpriteSetTextureAtlasMappingPool(): max must be != 0");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateSpriteSetTextureAtlasMappingPool();

	_setAtlasMappings = new SpriteSetTextureAtlasMapping[max];

	AssertWDesc(_setAtlasMappings != NULL, "SpriteSetManager::allocateSpriteSetTextureAtlasMappingPool(): _setAtlasMappings is NULL");

	if (_setAtlasMappings)
	{
		for (i=0;i<max;i++)
		{
			_setAtlasMappings[i].setSpriteSetManager(this);
		}

		_maxSetAtlasMappings = max;
		_numSetAtlasMappings = 0;
	}
	else
	{
		error = EOSErrorNoMemory;
	}

	return error;
}

void SpriteSetManager::deallocateSpriteSetTextureAtlasMappingPool(void)
{
	Uint32	i;

	if (_setAtlasMappings)
	{
		for (i=0;i<_maxSetAtlasMappings;i++)
		{
			if (_setAtlasMappings[i].isUsed())
			{
				_setAtlasMappings[i].setUsed(false);
			}
		}

		delete [] _setAtlasMappings;
		_setAtlasMappings = NULL;
	}

	_maxSetAtlasMappings = 0;
	_numSetAtlasMappings = 0;
}

void SpriteSetManager::updateUsage(void)
{
	Uint32	i;

	_numSets = 0;
	_numSetAtlasMappings = 0;

	_spriteSetTotalMemUsage = 0;
	_setAtlasMappingTotalMemUsage = 0;

	if (_setAtlasMappings)
	{
		for (i=0;i<_maxSetAtlasMappings;i++)
		{
			if (_setAtlasMappings[i].isUsed())
			{
				_numSetAtlasMappings++;
				_setAtlasMappingTotalMemUsage += _setAtlasMappings[i].getMemoryUsage();
			}
		}
	}

	if (_sets)
	{
		for (i=0;i<_maxSets;i++)
		{
			for (i=0;i<_maxSets;i++)
			{
				if (_sets[i].isUsed())
				{
					_numSets++;
					_spriteSetTotalMemUsage += _sets[i].getMemoryUsage();
				}
			}
		}
	}
}

Uint32 SpriteSetManager::getNumUsedSpriteSetTextureAtlasMappings(void)
{
	return _numSetAtlasMappings;
}

Uint32 SpriteSetManager::getSpriteSetTextureAtlasMappingTotalMemoryUsage(void)
{
	return _setAtlasMappingTotalMemUsage;
}

Uint32 SpriteSetManager::getNumUsedSpriteSets(void)
{
	return _numSets;
}

Uint32 SpriteSetManager::getSpriteSetTotalMemoryUsage(void)
{
	return _spriteSetTotalMemUsage;
}

