/******************************************************************************
 *
 * File: GraphicsToolKit.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Graphics data structures for usage in tools chains
 * 
 *****************************************************************************/

#ifndef __GRAPHICS_TOOLKIT_H__
#define __GRAPHICS_TOOLKIT_H__

#include "Platform.h"
#include "Graphics.h"

#include <string>

using namespace std;

//	Note that toolkit items can use STL
typedef struct
{
	int		id;
	string	label;
	string	filename;
} TextureDef;

typedef struct
{
	TextureDef*	tex_def;
} TextureDefPtr;

typedef struct
{
	Sint32	x;
	Sint32	y;
	Sint32	w;
	Sint32	h;
	Sint32	area;
} TextureArea;

typedef struct
{
	bool			placed;
	bool			vertical_hint;
	Sint32			x;
	Sint32			y;
	Sint32			area;
	Sint32			w;
	Sint32			h;
	TextureDefPtr	def;
	Uint32			vertices[4];
	Uint32			uvs[4];
	unsigned char*	image;
}  TextureEntry;

typedef struct
{
	TextureEntry*	texture;
} TexturePtr;

typedef struct
{
	Uint32			frameNumStart;
	Uint32			frameNumEnd;
	Uint32			spriteNumber;
	Point2D			hotspotRef;

	Point2D			hotspotAttach[MaxSupportedHotSpotAttach];

	HotSpotDataRAW	hotspotXformAttach[MaxSupportedHotSpotXformAttach];

	//	Other attachments spots go here
} SpriteAnimKeyFrameFlashRAW;

typedef struct
{
	Uint32			control;

	Float32			x;
	Float32			y;
	Float32			scaleX;
	Float32			scaleY;
	Float32			rotateZ;

	Float32			mat_a;
	Float32			mat_b;
	Float32			mat_c;
	Float32			mat_d;
	Float32			mat_tx;
	Float32			mat_ty;

	Float32			redMultiplier;
	Float32			greenMultiplier;
	Float32			blueMultiplier;
	Float32			alphaMultiplier;
	Uint32			redOffset;
	Uint32			greenOffset;
	Uint32			blueOffset;
	Uint32			alphaOffset;

	Float32			width;
	Float32			height;

	Uint32			numChildren;
	AddressOffset	firstChildIndex;
} MasterSpriteFlashRAW;

typedef struct
{
	Uint32			control;

	Float32			x;
	Float32			y;
	Float32			scaleX;
	Float32			scaleY;
	Float32			rotateZ;

	Float32			mat_a;
	Float32			mat_b;
	Float32			mat_c;
	Float32			mat_d;
	Float32			mat_tx;
	Float32			mat_ty;

	Float32			redMultiplier;
	Float32			greenMultiplier;
	Float32			blueMultiplier;
	Float32			alphaMultiplier;
	Uint32			redOffset;
	Uint32			greenOffset;
	Uint32			blueOffset;
	Uint32			alphaOffset;

	Uint32			numChildren;
	AddressOffset	firstChildIndex;
} SpriteFlashRAW;

typedef struct
{
	Uint32			endian;
	Uint32 			version;
	Uint32			numMasterSprites;
	AddressOffset	masterSprites;
	Uint32			numNormalSprites;
	AddressOffset	normalSprites;
	Uint32 			numSpriteMappings;
	AddressOffset	spriteMappings;
	Uint32			numNameOffsets;
	AddressOffset	nameOffsets;
	AddressOffset	nameData;
} FlashSpriteDataHeader;

typedef struct
{
	Uint32			endian;
	Uint32			version;
	Uint32			sampledFrameRate;
	Uint32			targetFrameRate;
	Uint32			numHotspotsInFrame;
	Uint32			numHotspotsXformInFrame;
	Uint32			numAnimSequences;
	Uint32			animSequences;
	Uint32 			numKeyFrames;
	Uint32			keyFrames;
	Uint32			numNameOffsets;
	AddressOffset	nameOffsets;
	AddressOffset	nameData;
} FlashSpriteAnimDataHeader;

#endif /* __GRAPHICS_TOOLKIT_H__ */

