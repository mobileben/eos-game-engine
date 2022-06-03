/******************************************************************************
 *
 * File: Graphics2DBinder.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Binder to hold all the information related to shared 2D asset pieces
 * 
 *****************************************************************************/

#ifndef __GRAPHICS2DBINDER_H__
#define __GRAPHICS2DBINDER_H__

#include "Platform.h"
#include "Graphics.h"
#include "TaskManager.h"

class Graphics2DBinder : public EOSFrameworkComponent
{
private:
	Char*									_name;
	Boolean									_named;

	Boolean									_needsFree;

	Uint32									_numAtlasLinks;
	Uint32									_maxAtlasLinks;
	Uint32									_numSetLinks;
	Uint32									_maxSetLinks;
	Uint32									_numSetAtlasMappingLinks;
	Uint32									_maxSetAtlasMappingLinks;
	Uint32									_numAnimLinks;
	Uint32									_maxAnimLinks;

	TextureAtlasToTextureLink*				_atlasLinks;
	SpriteSetToTextureAtlasLink*			_setLinks;
	SpriteSetToTextureAtlasMappingLink*		_setAtlasMappingLinks;
	SpriteAnimSetToSpriteSetLink*			_animLinks;

	NamedTextureAtlasToNamedTextureLink*	_namedAtlasLinks;
	NamedSpriteSetToNamedTextureAtlasLink*	_namedSetLinks;
	NamedSpriteSetToNamedTextureAtlasMappingLink*	_namedSetAtlasMappingLinks;
	NamedSpriteAnimSetToNamedSpriteSetLink*	_namedAnimLinks;

public:
	Graphics2DBinder();
	~Graphics2DBinder();

	EOSError								createLinks(Uint32 numAtlas, Uint32 numSetLinks, Uint32 numSetAtlasMappingLinks, Uint32 numAnimLinks, Boolean named);
	void									destroyLinks(void);

	EOSError								setLinks(Uint32 numAtlas, TextureAtlasToTextureLink* atlasLink, Uint32 numSet, SpriteSetToTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, SpriteSetToTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, SpriteAnimSetToSpriteSetLink* animLink, Boolean persistent);
	EOSError								setLinks(Uint32 numAtlas, NamedTextureAtlasToNamedTextureLink* atlasLink, Uint32 numSet, NamedSpriteSetToNamedTextureAtlasLink* setLink, Uint32 numSetAtlasMapping, NamedSpriteSetToNamedTextureAtlasMappingLink* setAtlasMappingLink, Uint32 numAnim, NamedSpriteAnimSetToNamedSpriteSetLink* animLink, Boolean persistent);

	inline Boolean							isNamed(void) { return _named; }

	inline Uint32									getNumTextureAtlasLinks(void) { return _numAtlasLinks; }
	inline TextureAtlasToTextureLink*				getTextureAtlasLinks(void) { return _atlasLinks; }
	inline NamedTextureAtlasToNamedTextureLink*		getNamedTextureAtlasLinks(void) { return _namedAtlasLinks; }

	inline Uint32									getNumSpriteSetLinks(void) { return _numSetLinks; }
	inline SpriteSetToTextureAtlasLink*				getSpriteSetLinks(void) { return _setLinks; }
	inline NamedSpriteSetToNamedTextureAtlasLink*	getNamedSpriteSetLinks(void) { return _namedSetLinks; }

	inline Uint32									getNumSpriteSetTextureAtlasMappingLinks(void) { return _numSetAtlasMappingLinks; }
	inline SpriteSetToTextureAtlasMappingLink*				getSpriteSetTextureAtlasMappingLinks(void) { return _setAtlasMappingLinks; }
	inline NamedSpriteSetToNamedTextureAtlasMappingLink*	getNamedSpriteSetTextureAtlasMappingLinks(void) { return _namedSetAtlasMappingLinks; }

	inline Uint32									getNumSpriteAnimSetLinks(void) { return _numAnimLinks; }
	inline SpriteAnimSetToSpriteSetLink*			getSpriteAnimSetLinks(void) { return _animLinks; }
	inline NamedSpriteAnimSetToNamedSpriteSetLink*	getNamedSpriteANimSetLinks(void) { return _namedAnimLinks; }

	EOSError								addAtlasLink(const Char* textureName, const Char* atlasName);  
	EOSError 								addSetLink(const Char* atlasName, const Char* spriteSetName);  
	EOSError 								addSetTextureAtlasMappingLink(const Char* atlasMappingName, const Char* spriteSetName);  
	EOSError 								addAnimLink(const Char* spriteSetName, const Char* spriteAnimSetName);  

	EOSError 								addAtlasLink(ObjectID textureID, ObjectID atlasID);  
	EOSError 								addSetLink(ObjectID atlasID, ObjectID spriteSetID);  
	EOSError 								addSetTextureAtlasMappingLink(ObjectID atlasMappingID, ObjectID spriteSetID);  
	EOSError 								addAnimLink(ObjectID spriteSetID, ObjectID spriteAnimSetID);  

	EOSError								resetLinkData(void);

	EOSError								queueGraphics2DLinkerLoadTasks(TaskManager* loader, Boolean persistent, Task** loadtask = NULL);
	EOSError								queueLoadTasks(TaskManager* loader, Boolean texture, Boolean atlas, Boolean spriteSet, Boolean animSet, Uint8* buffer, Uint32 bufferSize, const Char* name, ObjectID objid = UndefinedObjectID);
	EOSError								queueLoadTasks(TaskManager* loader, Uint32 numTextures, Boolean spriteSet, Boolean animSet, Uint8* buffer, Uint32 bufferSize, const Char* name, ObjectID objid = UndefinedObjectID);
};

#endif /* __GRAPHICS2DBINDER_H__ */


