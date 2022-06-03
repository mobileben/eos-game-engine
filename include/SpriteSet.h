/******************************************************************************
 *
 * File: SpriteSet.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Set
 * 
 *****************************************************************************/

#ifndef __SPRITE_SET_H__
#define __SPRITE_SET_H__

#include "Platform.h"
#include "Graphics.h"
#include "TextureAtlas.h"


typedef struct
{
	Uint32			endian;
	Uint32			version;
	AddressOffset	nameOffset;
	ObjectID		textureAtlasID;
	AddressOffset	textureAtlasNameOffset;
	Uint32			numSprites;
	AddressOffset	sprites;
	Uint32			numSubSprites;
	AddressOffset	subSprites;
	Uint32			numSpriteMappings;
	AddressOffset	spriteMappings;
	Uint32			numSpriteTranslates;
	AddressOffset	spriteTranslates;
	Uint32			numSpriteTransforms;
	AddressOffset	spriteTransforms;
	Uint32			numSpriteMatrices;
	AddressOffset	spriteMatrices;
	Uint32			numSpriteColorOps;
	AddressOffset	spriteColorOps;
	Uint32			numNameOffsets;
	AddressOffset	nameOffsets;
	AddressOffset	nameData;
} SpriteSetHeader;

typedef struct
{
	Uint32			endian;
	Uint32			version;
	AddressOffset	nameOffset;
	Uint32			numAtlasMappings;
	AddressOffset	atlasMappings;
	AddressOffset	subTexIDMappings;
} SpriteSetTextureAtlasMappingHeader;

class SpriteSetManager;

class SpriteSetTextureAtlasMapping : public EOSObject
{
private:
	Boolean		_used;

	ObjectID	_id;
	Char*		_name;

	SpriteSetManager*		_spriteSetMgr;

	Uint32		_numAtlasMappings;
	Uint8*		_atlasMappings;
	Uint32*		_subTexIDMappings;

	Uint32		_memUsage;

public:
	SpriteSetTextureAtlasMapping();
	~SpriteSetTextureAtlasMapping();

	EOSError	create(ObjectID objid, const Char* name, Uint8* data, Uint32 datasize);


	void 					destroy(void);

	inline ObjectID			getRefID(void) const { return _id; }
	inline Char*			getName(void) const { return _name; }

	inline void				setSpriteSetManager(SpriteSetManager* mgr) { _spriteSetMgr = mgr; }

	inline Uint32			getNumTextureAtlasMappings(void) { return _numAtlasMappings; }
	inline Uint8*			getTextureAtlasMappings(void) { return _atlasMappings; }
	inline Uint32*			getTextureAtlasSubTexIDMappings(void) { return _subTexIDMappings; }

	inline Boolean			isUsed(void) const { return _used; }

	void					setUsed(Boolean used);
	void					setRefID(ObjectID objid);
	void					setName(const Char* name);

	inline Uint32			getMemoryUsage(void) { return _memUsage; }
};

class SpriteSet : public EOSObject
{
private:
	Boolean					_used;

	ObjectID				_id;
	Char*					_name;

	SpriteSetManager*		_spriteSetMgr;

	Uint32					_numSprites;
	Sprite*					_sprites;
	Uint32					_numSubSprites;
	SubSprite*				_subSprites;
	Uint32					_numSpriteMappings;
	SpriteMapping*			_spriteMappings;

	Uint32					_numSpriteTranslates;
	Point2D*				_spriteTranslates;

	Uint32					_numSpriteTransforms;
	SpriteTransform*		_spriteTransforms;

	Uint32					_numSpriteMatrices;
	SpriteMatrix*			_spriteMatrices;

	Uint32					_numSpriteColorOps;
	SpriteColorOp*			_spriteColorOps;

	Uint32					_numTextureAtlas;
	TextureAtlas**			_textureAtlas; 

	SpriteSetTextureAtlasMapping*	_spriteSetTextureAtlasMapping;	//	Needed to destroy
	Uint8*							_textureAtlasMapping;
	Uint32*							_textureAtlasSubTexIDMapping;

	Uint32							_memUsage;

public:
	SpriteSet();
	~SpriteSet();

	EOSError				create(TextureAtlas* atlas, Uint32 numSprites, MasterSpriteRAW* sprites, Uint32 numSubSprites, SpriteRAW* subSprites, Uint32 numSpriteMappings, SpriteMapping* mappings);
	EOSError				create(ObjectID objid, const Char* name, TextureAtlas* atlas, Uint8* data, Uint32 datasize);

	EOSError				createFromFile(const Char* filename, ObjectID objid, const Char* name, TextureAtlas* atlas, Uint8* buffer, Uint32 maxBufferSize);

	void 					destroy(void);

	inline ObjectID			getRefID(void) const { return _id; }
	inline Char*			getName(void) const { return _name; }

	inline void				setSpriteSetManager(SpriteSetManager* mgr) { _spriteSetMgr = mgr; }

	void					addTextureAtlas(TextureAtlas* atlas);
	inline Uint32			getNumTextureAtlas(void) { return _numTextureAtlas; }
	inline TextureAtlas*	getTextureAtlas(ObjectID objid) { return _textureAtlas[objid]; }
	TextureAtlasSubTexture*	getTextureAtlasSubTexture(ObjectID objid);
	TextureAtlasSubTexture*	findTextureAtlasSubTexture(const Char* name);

	void					setTextureAtlasMapping(SpriteSetTextureAtlasMapping* mapping);

	inline Uint32 			getNumSprites(void) { return _numSprites; }

	inline Sprite*			getSprite(ObjectID objid) { return &_sprites[objid]; }
	inline SubSprite*		getSubSprite(ObjectID objid) { return &_subSprites[objid]; }
	inline SpriteMapping*	getSpriteMapping(ObjectID objid) { return &_spriteMappings[objid]; }

	inline Boolean			isUsed(void) const { return _used; }

	void					setUsed(Boolean used);
	void					setRefID(ObjectID objid);
	void					setName(const Char* name);

	inline Uint32			getMemoryUsage(void) { return _memUsage; }
};

#endif /* __SPRITE_SET_H__ */
