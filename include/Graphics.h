/******************************************************************************
 *
 * File: Graphics.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Graphics data structures
 * 
 *****************************************************************************/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "Platform.h"
#include "Texture.h"

#define CONTROL_TYPE_SPRITETEXTUREREF   0x00000000
#define CONTROL_TYPE_SPRITEMASTER       0x80000000
#define CONTROL_TYPE_SPRITENORMAL       0x40000000

#define CONTROL_FLIP_H                  0x00000001
#define CONTROL_FLIP_V                  0x00000002

#define CONTROL_TRANSFORM_TRANSLATE    	 	0x00000010
#define CONTROL_TRANSFORM_SCALE         	0x00000020
#define CONTROL_TRANSFORM_ROTATE        	0x00000040
#define CONTROL_TRANSFORM_IDENTITY      	0x00000080
#define CONTROL_TRANSFORM_COLOR_MULTIPLIER	0x00000100
#define CONTROL_TRANSFORM_COLOR_OFFSET		0x00000200
#define CONTROL_TRANSFORM_MATRIX	      	0x00000400

#define MaxSupportedHotSpotAttach		31
#define MaxSupportedHotSpotXformAttach	31

typedef struct
{
	Float32	x;
	Float32	y;
} Point2D;

typedef struct
{
	Sint32	x;
	Sint32	y;
} PointInt2D;

typedef struct
{
	Float32	r;
	Float32	g;
	Float32	b;
} ColorRGB;

typedef struct
{
	Float32	r;
	Float32	g;
	Float32	b;
	Float32	a;
} ColorRGBA;

typedef struct
{
	Uint32 	mapControl;
	Uint32	offset;
} SpriteMapping;

typedef SpriteMapping SpriteMappingRAW;

typedef struct
{
	Uint32			control;

	Uint32			spriteTranslate;
	Uint32			spriteTransform;
	Uint32			spriteColorOp;

	Float32			width;
	Float32			height;

	Uint32			numChildren;
	AddressOffset	firstChildIndex;
} MasterSpriteRAW;

#define SPRITE_MATRIX_A	0
#define SPRITE_MATRIX_B	1
#define SPRITE_MATRIX_C	2
#define SPRITE_MATRIX_D	3

#define SPRITE_TRANSFORM_SCALE_X	0
#define SPRITE_TRANSFORM_SCALE_Y	1
#define SPRITE_TRANSFORM_ROTATE_Z	2

typedef struct
{
	Float32	a;
	Float32	b;
	Float32	c;
	Float32	d;
} SpriteMatrix;

typedef struct
{
	Float32	scaleX;
	Float32	scaleY;
	Float32	rotateZ;
} SpriteTransform;

typedef struct
{
	Float32	redMultiplier;
	Float32	greenMultiplier;
	Float32	blueMultiplier;
	Float32	alphaMultiplier;
	Float32	redOffset;
	Float32	greenOffset;
	Float32	blueOffset;
	Float32	alphaOffset;
} SpriteColorOp;

typedef struct
{
	Uint32			control;

	Point2D*			xy;

	Float32*			transform;  

	SpriteColorOp*		colorOp;

	Float32	width;
	Float32	height;

	Uint32	numChildren;
	Uint32*	childrenList;
} MasterSprite;

typedef struct
{
	Uint32			control;

	Uint32			spriteTranslate;
	Uint32			spriteTransform;
	Uint32			spriteColorOp;

	Uint32			numChildren;
	AddressOffset	firstChildIndex;
} SpriteRAW;

typedef struct
{
	ObjectID		id;
	Uint32			width;
	Uint32			height;
	AddressOffset	vertices[4];
	AddressOffset	uvs[4];
} SpriteTextureRefRAW;

typedef struct
{
	ObjectID	id;
	Uint32		width;
	Uint32		height;
	Point2D*	vertices;
	Point2D*	uvs;
} SpriteTextureRef;

class SpriteSet;

typedef struct
{
	SpriteSet*			spriteSet;

	Uint32				control;

	Point2D*			xy;

	Float32*			transform;

	SpriteColorOp*		colorOp;

	Float32			width;
	Float32			height;

	Uint32			numChildren;
	SpriteMapping*	children;
} Sprite;

typedef struct
{
	SpriteSet*		spriteSet;

	Uint32			control;

	Point2D*			xy;

	Float32*			transform;

	SpriteColorOp*		colorOp;

	Uint32			numChildren;
	SpriteMapping*	children;
} SubSprite;

class TextureAtlas;

typedef struct
{
	Float32			x, y;
	Float32			u, v;
} Vertex_PT2F_UV2F;

typedef struct
{
	Float32			x, y;
	Float32			u, v;
	Float32			r, g, b, a;
} Vertex_PT2F_UV2F_CL4F;

typedef struct
{
	TextureAtlas*	atlas;
	Uint32			width;
	Uint32			height;
	union
	{
		struct
		{
			Point2D					vertices[4];
			Point2D					uvs[4];
		} PT2D_DIRECT;

		struct
		{
			Point2D*				vertices[4];
			Point2D*				uvs[4];
		} PT2D_PTR;

		struct
		{
			Vertex_PT2F_UV2F*		vertices[4];
		} VTX_PT2F_UV2F_PTR;

		struct
		{
			Vertex_PT2F_UV2F_CL4F*	vertices[4];
		} VTX_PT2F_UV2F_CL4F_PTR;

		struct
		{
			Uint16					indices[4];
		} VTX_IDX;
	} data;
} TextureAtlasSubTexture;

typedef struct
{
	Uint32	numFrames;
	Uint32	numKeyFrames;
	Uint32 	firstKeyFrameIndex;
} SpriteAnimSequenceRAW;

typedef struct
{
	Float32	x;
	Float32	y;
	Uint32	control;
	Float32	scaleX;	
	Float32	scaleY;
	Float32	rotateZ;
} HotSpotDataRAW;

typedef struct
{
	Float32	x;
	Float32	y;
	Float32	rotateZ;
} HotSpotData;

typedef struct
{
	Uint32			duration;	//	Duration in seconds for now
	Uint32			spriteNumber;
	Point2D			hotspotRef;
	AddressOffset	hotspotAttach;
	AddressOffset	hotspotXformAttach;
} SpriteAnimKeyFrameRAW;

class SpriteAnimSet;

typedef struct
{
	Uint32			duration;
	Uint32			spriteNumber;
	Point2D			hotspotRef;
	Point2D*		hotspotAttach;
	HotSpotData*	hotspotXformAttach;
} SpriteAnimKeyFrame;

typedef struct
{
	SpriteAnimSet*		animSet;

	Uint32				numFrames;
	Uint32				numKeyFrames;
	SpriteAnimKeyFrame*	keyFrames;
} SpriteAnimSequence;

typedef struct
{
	ObjectID	textureID;
	ObjectID	atlasID;
} TextureAtlasToTextureLink;

typedef struct
{
	ObjectID	atlasID;
	ObjectID	spriteSetID;
} SpriteSetToTextureAtlasLink;

typedef struct
{
	ObjectID	atlasMappingID;
	ObjectID	spriteSetID;
} SpriteSetToTextureAtlasMappingLink;

typedef struct
{
	ObjectID	spriteSetID;
	ObjectID	spriteAnimSetID;
} SpriteAnimSetToSpriteSetLink;

typedef struct
{
	Char*	textureName;
	Char*	atlasName;
} NamedTextureAtlasToNamedTextureLink;

typedef struct
{
	Char*		atlasName;
	Char*		spriteSetName;
} NamedSpriteSetToNamedTextureAtlasLink;

typedef struct
{
	Char*		atlasMappingName;
	Char*		spriteSetName;
} NamedSpriteSetToNamedTextureAtlasMappingLink;

typedef struct
{
	Char*	spriteSetName;
	Char*	spriteAnimSetName;
} NamedSpriteAnimSetToNamedSpriteSetLink;

typedef Uint32	SpriteID;

typedef Uint32	SpriteAnimSequenceID;

typedef Uint32	DrawControl;

#endif /* __GRAPHICS_H__ */

