/******************************************************************************
 *
 * File: TextureAtlas.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Object
 * 
 *****************************************************************************/

#ifndef __TEXTURE_ATLAS_H__
#define __TEXTURE_ATLAS_H__

#include "Texture.h"
#include "TextureState.h"
#include "EOSError.h"

typedef struct
{
	Uint32			endian;
	Uint32			version;
	TextureID		textureID;
	Uint32			nameOffset;
	Uint32			textureFormat;
	Uint32			width;
	Uint32			height;
	Uint32			textureSize;
	AddressOffset	texture;
	Uint32			numSubTextures;
	AddressOffset	subTextures;
	Uint32			numVertices;
	AddressOffset	vertices;
	Uint32			numUVs;
	AddressOffset	uvs;
	Uint32			numNameOffsets;
	AddressOffset	nameOffsets;
	AddressOffset	nameData;
} TextureAtlasHeader;

class TextureAtlasManager;

class TextureAtlas : EOSObject
{
private:
	Boolean					_used;

	ObjectID				_id;
	Char*					_name;

	TextureAtlasManager*	_textureAtlasMgr;

	TextureState			_textureState;

	Uint32					_numSubTextures;	
	TextureAtlasSubTexture*	_subTextures; 

	Point2D*				_verticeList;
	Point2D*				_uvList;

	Char*					_nameData;
	Char**					_nameList;

	Uint32					_memUsage;

public:
	TextureAtlas();
	~TextureAtlas();

	EOSError					create(ObjectID objid, const Char* name, Texture* tex, Uint32 numSubTex, SpriteTextureRefRAW* subTex, Uint32 numVerts, Point2D* verts, Uint32 numUVs, Point2D* uvs);
	EOSError					create(ObjectID objid, const Char* name, Texture* tex, Uint8* data, Uint32 datasize);

	EOSError					createFromFile(const Char* filename, ObjectID objid, const Char* name, Texture* tex, Uint8* buffer, Uint32 maxBufferSize);
	
	void						destroy(void);

	inline ObjectID				getRefID(void) const { return _id; }
	inline Char*				getName(void) const { return _name; }

	inline void					setTextureAtlasManager(TextureAtlasManager* mgr) { _textureAtlasMgr = mgr; }

	ObjectID					findSubTextureIDFromName(const Char* name);
	TextureAtlasSubTexture*		findSubTextureFromName(const Char* name);

	inline const TextureState* 	getTextureState(void) const { return &_textureState; }
	inline Boolean				isUsed(void) const { return _used; }

	TextureAtlasSubTexture* 	getSubTexture(ObjectID objid);

	void						setTexture(Texture* tex);
	void						setUsed(Boolean used);
	void						setRefID(ObjectID objid);
	void						setName(const Char* name);

	inline Uint32				getMemoryUsage(void) { return _memUsage; }
};

#endif /* __TEXTURE_ATLAS_H__ */
