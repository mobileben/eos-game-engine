/******************************************************************************
 *
 * File: SpriteDataSet.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Parts for encapsulating and loading Sprite Data sets
 * 
 *****************************************************************************/

#ifndef __SPRITEDATASET_H__
#define __SPRITEDATASET_H__

#include "Platform.h"
#include "toolkit/GraphicsToolKit.h"

#include <iterator>
#include <vector>

class SpriteDataSet
{
private:
	vector<MasterSpriteFlashRAW>	_masterSprites;
	vector<SpriteFlashRAW>			_normalSprites;
	vector<SpriteMapping>			_spriteMappings;
	vector<string>					_textureRefNames;
	vector<Point2D>					_spriteTranslates;
	vector<SpriteTransform>			_spriteTransforms;
	vector<SpriteMatrix>			_spriteMatrices;
	vector<SpriteColorOp>			_spriteColorOps;

	Endian							_endian;

	Uint32							_textureAtlasID;

	Char*							_exportNames;
	Uint32*							_exportNamesOffsets;
	Uint32							_exportNamesSize;

	Uint32							_version;
	Boolean							_comments;

	void swapHeaderIfNecessary(FlashSpriteDataHeader* header);
	void swapMasterSpriteIfNecessary(MasterSpriteFlashRAW* sprite);
	void swapNormalSpriteIfNecessary(SpriteFlashRAW* sprite);
	void swapSpriteMappingIfNecessary(SpriteMapping* mapping);
	void swapNameOffsetIfNecessary(Uint32* offset);

	EOSError loadMasterSprites(Uint32 num, Uint8* buffer,Uint32 size);
	EOSError loadNormalSprites(Uint32 num, Uint8* buffer,Uint32 size);
	EOSError loadSpriteMappings(Uint32 num, Uint8* buffer,Uint32 size);
	EOSError loadNames(Uint32 num, Uint8* buffer,Uint32 size, Uint8* databuffer,Uint32 datasize);
	EOSError parseSpriteData(Uint8* data, Uint32 size);

	EOSError 	addExportName(Char* name, Char** nameList, Uint32& size, Uint32& offset);

	Boolean		isSkewedMatrix(SpriteTransform& xform, SpriteMatrix& matrix);
	Uint32		addSpriteTranslate(Point2D& xlate, Uint32& control);
	Uint32		addSpriteTransform(SpriteTransform& xform, Uint32& control);
	Uint32		addSpriteMatrix(SpriteTransform& xform, SpriteMatrix& matrix, Uint32& control);
	Uint32		addSpriteColorOp(SpriteColorOp& color, Uint32& control);

public:
	SpriteDataSet();
	~SpriteDataSet();

	Uint32 	getNumMasterSprites(void);
	vector<MasterSpriteFlashRAW>::iterator	getMasterSpriteAt(Uint32 index);

	Uint32	getNumNormalSprites(void);
	vector<SpriteFlashRAW>::iterator		getNormalSpriteAt(Uint32 index);

	Uint32	getNumSpriteMappings(void);
	vector<SpriteMapping>::iterator		getSpriteMappingAt(Uint32 index);

	Uint32 	getNumTextureRefNames(void);
	vector<string>::iterator			getTextureRefNameAt(Uint32 index);

	inline Boolean	getComments(void) { return _comments; }
	inline void		setComments(Boolean comments) { _comments = comments; }

	EOSError	loadDataSet(Char* filename);
	EOSError	exportDataSetAsText(Char* filename, Char* outprefix);
	EOSError	exportDataSetAsBIN(Char* filename);
};

#endif /* __SPRITEDATASET_H__ */
