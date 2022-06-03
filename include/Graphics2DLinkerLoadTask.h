/******************************************************************************
 *
 * File: Graphics2DLinkerTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Task to resolve sprite anim set data
 * 
 *****************************************************************************/

#ifndef __GRAPHICS2DLINKERLOADTASK_H__
#define __GRAPHICS2DLINKERLOADTASK_H__

#include "Platform.h"
#include "Task.h"
#include "Graphics.h"
#include "SpriteAnimSet.h"

class Graphics2DLinkerLoadTask : public Task
{
private:
	Boolean									_named;
	Boolean									_needsFree;

	Uint32									_numAtlasLinks;
	Uint32									_numSetLinks;
	Uint32									_numSetAtlasMappingLinks;
	Uint32									_numAnimLinks;

	TextureAtlasToTextureLink*				_atlasLinks;
	SpriteSetToTextureAtlasLink*			_setLinks;
	SpriteSetToTextureAtlasMappingLink*		_setAtlasMappingLinks;
	SpriteAnimSetToSpriteSetLink*			_animLinks;

	NamedTextureAtlasToNamedTextureLink*	_namedAtlasLinks;
	NamedSpriteSetToNamedTextureAtlasLink*	_namedSetLinks;
	NamedSpriteSetToNamedTextureAtlasMappingLink*	_namedSetAtlasMappingLinks;
	NamedSpriteAnimSetToNamedSpriteSetLink*	_namedAnimLinks;

	void			destroy(void);

public:
	Graphics2DLinkerLoadTask();
	~Graphics2DLinkerLoadTask();

	void			setLinks(Uint32 numAtlas, TextureAtlasToTextureLink* atlasLink, Uint32 numSet, SpriteSetToTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, SpriteSetToTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, SpriteAnimSetToSpriteSetLink* animLink, Boolean persistent);
	void			setLinks(Uint32 numAtlas, NamedTextureAtlasToNamedTextureLink* atlasLink, Uint32 numSet, NamedSpriteSetToNamedTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, NamedSpriteSetToNamedTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, NamedSpriteAnimSetToNamedSpriteSetLink* animLink, Boolean persistent);

	EOSError		epilogue(void);
};

#endif /* __GRAPHICS2DLINKERLOADTASK_H__ */

