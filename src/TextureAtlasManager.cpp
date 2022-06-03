/******************************************************************************
 *
 * File: TextureAtlasManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Manager Object
 * 
 *****************************************************************************/

#include "TextureAtlasManager.h"

TextureAtlasManager::TextureAtlasManager()
{
	_maxAtlases = 0;
	_numAtlases = 0;

	_atlases = NULL;

	_totalMemUsage = 0;
}

TextureAtlasManager::~TextureAtlasManager()
{
	deallocateTextureAtlasPool();
}

TextureAtlas* TextureAtlasManager::getFreeTextureAtlas(void)
{
	TextureAtlas* 	atlas = NULL;
	Uint32			i;

	if (_atlases)
	{
		for (i=0;i<_maxAtlases;i++)
		{
			if (_atlases[i].isUsed() == false)
			{
				atlas = &_atlases[i];
				atlas->setUsed(true);
				_numAtlases++;

				invalidate();
				break;
			}
		}
	}

	return atlas;
}

TextureAtlas* TextureAtlasManager::findTextureAtlasFromRefID(ObjectID refID)
{
	TextureAtlas* 	atlas = NULL;
	Uint32			i;

	if (_atlases)
	{
		for (i=0;i<_maxAtlases;i++)
		{
			if (_atlases[i].isUsed() == true)
			{
				if (_atlases[i].getRefID() == refID)
				{
					atlas = &_atlases[i];
					break;
				}
			}
		}
	}

	return atlas;
}

TextureAtlas* TextureAtlasManager::findTextureAtlasFromName(const Char* name)
{
	TextureAtlas* atlas = NULL;
	Uint32			i;

	if (_atlases)
	{
		for (i=0;i<_maxAtlases;i++)
		{
			if (_atlases[i].isUsed() == true)
			{
				if (!strcmp(_atlases[i].getName(), name))
				{
					atlas = &_atlases[i];
					break;
				}
			}
		}
	}

	return atlas;
}

void TextureAtlasManager::invalidate(void)
{
}

void TextureAtlasManager::destroyTextureAtlas(TextureAtlas& atlas)
{
	if (atlas.isUsed())
	{
		atlas.setUsed(false);
		atlas.destroy();
		_numAtlases--;
	}
}

EOSError TextureAtlasManager::allocateTextureAtlasPool(Uint32 max)
{
	EOSError error = EOSErrorNone;
	Uint32	i;

	AssertWDesc(max != 0, "TextureAtlasManager::allocateTextureAtlasPool(): max must be != 0");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateTextureAtlasPool();

	_atlases = new TextureAtlas[max];

	AssertWDesc(_atlases != NULL, "TextureAtlasManager::allocateTextureAtlasPool(): _atlases is NULL");

	if (_atlases)
	{
		for (i=0;i<max;i++)
		{
			_atlases[i].setTextureAtlasManager(this);
		}

		_maxAtlases = max;
		_numAtlases = 0;
	}
	else
	{
		error = EOSErrorNoMemory;
	}

	return error;
}

void TextureAtlasManager::deallocateTextureAtlasPool(void)
{
	Uint32	i;

	if (_atlases)
	{
		for (i=0;i<_maxAtlases;i++)
		{
			if (_atlases[i].isUsed())
			{
				_atlases[i].setUsed(false);
			}
		}

		delete [] _atlases;
		_atlases = NULL;
	}

	_maxAtlases = 0;
	_numAtlases = 0;
}

void TextureAtlasManager::updateUsage(void)
{
	Uint32	i;

	_numAtlases = 0;
	_totalMemUsage = 0;

	if (_atlases)
	{
		for (i=0;i<_maxAtlases;i++)
		{
			if (_atlases[i].isUsed())
			{
				_numAtlases++;
				_totalMemUsage += _atlases[i].getMemoryUsage();
			}
		}
	}
}

Uint32 TextureAtlasManager::getNumUsedTextureAtlases(void)
{
	return _numAtlases;
}

Uint32 TextureAtlasManager::getTotalMemoryUsage(void)
{
	return _totalMemUsage;
}

void TextureAtlasManager::dumpTextureAtlases(void)
{
	Uint32			i;
	Uint32			count = 0;
	const TextureState*	tstate;
	Texture*		tex;
#ifdef _PLATFORM_PC
	Char str[256];
#endif

	if (_atlases)
	{
#ifdef _PLATFORM_IPHONE
		NSLog(@"Texture Atlases (%d/%d)", _numAtlases, _maxAtlases);
#else
		sprintf(str, "Texture Atlases (%d/%d)\n", _numAtlases, _maxAtlases);
		OutputDebugStringA(str);
#endif

		for (i=0;i<_maxAtlases;i++)
		{
			if (_atlases[i].isUsed())
			{
				tstate = _atlases[i].getTextureState();
				tex = tstate->_textureObj;

#ifdef _PLATFORM_IPHONE
				NSLog(@"Texture Atlas (%d): %s (%d x %d)", i, _atlases[i].getName(), tex->getWidth(), tex->getHeight());
#else
				sprintf(str, "Texture Atlas (%d): %s (%d x %d)\n", i, _atlases[i].getName(), tex->getWidth(), tex->getHeight());
				OutputDebugStringA(str);
#endif
				count++;
			}
		}

#ifdef _PLATFORM_IPHONE
		NSLog(@"Texture Atlases Verified (%d/%d)", count, _maxAtlases);
#else
		sprintf(str, "Texture Atlases Verified (%d/%d)\n", count, _maxAtlases);
		OutputDebugStringA(str);
#endif
	}
}
