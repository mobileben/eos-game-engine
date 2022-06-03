/******************************************************************************
 *
 * File: EOSRsrcBundle.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * EOS Rsrc Bundle
 * 
 *****************************************************************************/

#ifndef __EOS_RSRC_BUNDLE_H__
#define __EOS_RSRC_BUNDLE_H__

#include "Platform.h"

#define	EOS_RSRC_BUNDLE_TEXTURE							0
#define	EOS_RSRC_BUNDLE_TEXTURE_ATLAS					1
#define	EOS_RSRC_BUNDLE_SPRITE_SET						2
#define	EOS_RSRC_BUNDLE_SPRITE_ANIM_SET					3
#define	EOS_RSRC_BUNDLE_SPRITE_TEXTURE_ATLAS_MAPPING	4

typedef struct
{
	Uint32	nameOffset;
	Uint32	dataOffset;
	Uint32	dataSize;
	Uint32	width;
	Uint32	height;
	Uint32	colorFormat;
	Uint32	texState;
	Uint32	texBlend;
} EOSRsrcBundleTextureExport;

typedef struct
{
	Uint32	nameOffset;
	Uint32	dataOffset;
	Uint32	dataSize;
	Uint32	texNameOffset;
} EOSRsrcBundleTextureAtlasExport;

typedef struct
{
	Uint32	nameOffset;
	Uint32	dataOffset;
	Uint32	dataSize;
} EOSRsrcBundleSpriteTextureAtlasMappingExport;

typedef struct
{
	Uint32	nameOffset;
	Uint32	dataOffset;
	Uint32	dataSize;
	Uint32	numTexAtlasNames;
	Uint32	texAtlasNameListOffset;
	Uint32	sprTexAtlasNameOffset;
} EOSRsrcBundleSpriteExport;

typedef struct
{
	Uint32	nameOffset;
	Uint32	dataOffset;
	Uint32	dataSize;
	Uint32	sprNameOffset;
} EOSRsrcBundleSpriteAnimExport;

typedef struct
{
	Uint32			endian;
	Uint32			version;
	Uint32			numTextures;
	AddressOffset	textures;
	Uint32			numTextureAtlases;
	AddressOffset	textureAtlases;
	Uint32			numSpriteMappings;
	AddressOffset	spriteMappings;
	Uint32			numSprites;
	AddressOffset	sprites;
	Uint32			numSpriteAnims;
	AddressOffset	spriteAnims;
	Uint32			numNameOffsets;
	AddressOffset	nameOffsets;
	AddressOffset	names;
	AddressOffset	data;
} EOSRsrcBundleDatabaseHeader;

#endif /* __EOS_RSRC_BUNDLE_H__ */

