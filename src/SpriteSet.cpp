/******************************************************************************
 *
 * File: SpriteSet.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * SpriteSet
 * 
 *****************************************************************************/

#include "SpriteSet.h"
#include "File.h"
#include "App.h"

SpriteSetTextureAtlasMapping::SpriteSetTextureAtlasMapping() : _used(false), _id(0xFFFFFFFF), _name(NULL), _numAtlasMappings(0), _atlasMappings(NULL), _subTexIDMappings(NULL), _memUsage(0), _spriteSetMgr(NULL)
{
}

SpriteSetTextureAtlasMapping::~SpriteSetTextureAtlasMapping()
{
	if (_name)
		delete _name;

	_spriteSetMgr = NULL;

	destroy();
}

void SpriteSetTextureAtlasMapping::destroy(void)
{
	if (_atlasMappings)
		delete _atlasMappings;

	if (_subTexIDMappings)
		delete _subTexIDMappings;

	_numAtlasMappings = 0;
	_atlasMappings = NULL;
	_subTexIDMappings = NULL;

	_memUsage = 0;

	if (_spriteSetMgr)
		_spriteSetMgr->updateUsage();
}

EOSError SpriteSetTextureAtlasMapping::create(ObjectID objid, const Char* name, Uint8* data, Uint32 datasize)
{
	EOSError 							error = EOSErrorNone;
	SpriteSetTextureAtlasMappingHeader*	header = (SpriteSetTextureAtlasMappingHeader*) data;
	Endian								endian;
	Uint8*								mappings; 
	Uint32*								subtex;

	AssertWDesc(data != NULL, "SpriteSetTextureAtlasMapping::create() NULL data.");

	if (data)
	{
		if (header->endian == 0x04030201)
			endian.switchEndian();

		_id = objid;
		setName(name);

		destroy();

		_numAtlasMappings = endian.swapUint32(header->numAtlasMappings);

		mappings = (Uint8*) (data + endian.swapUint32(header->atlasMappings));
		subtex = (Uint32*) (data + endian.swapUint32(header->subTexIDMappings));

		_atlasMappings = new Uint8[_numAtlasMappings];
		_subTexIDMappings = new Uint32[_numAtlasMappings];

		AssertWDesc(_atlasMappings != NULL, "SpriteSetTextureAtlasmapping::create Memory failed");
		AssertWDesc(_subTexIDMappings != NULL, "SpriteSetTextureAtlasmapping::create Memory failed");

		if (_atlasMappings && _subTexIDMappings)
		{
			_memUsage = sizeof(Uint8) * _numAtlasMappings + sizeof(Uint32) * _numAtlasMappings;

			memcpy(_atlasMappings, mappings, _numAtlasMappings);
			memcpy(_subTexIDMappings, subtex, _numAtlasMappings * sizeof(Uint32));
		}
		else
			error = EOSErrorNoMemory;
	}
	else
		error = EOSErrorNULL;

	if (error == EOSErrorNone)
	{
		if (_spriteSetMgr)
			_spriteSetMgr->updateUsage();
	}

	AssertWDesc(error == EOSErrorNone, "SpriteSetTextureAtlasMapping::create error");

	return error;
}

void SpriteSetTextureAtlasMapping::setUsed(Boolean used)
{
	_used = used;

	if (_used == false)
		destroy();

	if (_spriteSetMgr)
		_spriteSetMgr->updateUsage();
}

void SpriteSetTextureAtlasMapping::setRefID(ObjectID objid)
{
	_id = objid;
}

void SpriteSetTextureAtlasMapping::setName(const Char* name)
{
	if (_name)
		delete _name;

	if (name)
	{
		_name = new Char[strlen(name) + 1];

		if (_name)
			strcpy(_name, name);
	}
	else
		_name = NULL;
}

SpriteSet::SpriteSet()
{
	_used = false;
	_id = 0xFFFFFFFF;
	_name = NULL;

	_numSprites = 0;
	_sprites = NULL;
	_numSubSprites = 0;
	_subSprites = NULL;
	_numSpriteMappings = 0;
	_spriteMappings = NULL;

	_numTextureAtlas = 0;
	_textureAtlas = NULL;

	_numSpriteTranslates = 0;
	_spriteTranslates = NULL;

	_numSpriteTransforms = 0;
	_spriteTransforms = NULL;

	_numSpriteMatrices = 0;
	_spriteMatrices = NULL;

	_numSpriteColorOps = 0;
	_spriteColorOps = NULL;

	_spriteSetTextureAtlasMapping = NULL;
	_textureAtlasMapping = NULL;
	_textureAtlasSubTexIDMapping = NULL;

	_memUsage = 0;
	_spriteSetMgr = NULL;
}

SpriteSet::~SpriteSet()
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	_spriteSetMgr = NULL;

	destroy();
}

#ifdef OBSOLETE
EOSError SpriteSet::create(TextureAtlas* atlas, Uint32 numSprites, MasterSpriteRAW* sprites, Uint32 numSubSprites, SpriteRAW* subSprites, Uint32 numSpriteMappings, SpriteMapping* mappings)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	AssertWDesc(atlas != NULL, "SpriteSet::create(): NULL atlas");
	AssertWDesc(sprites != NULL, "SpriteSet::create(): NULL sprites");
	AssertWDesc(subSprites != NULL, "SpriteSet::create(): NULL subSprites");
	AssertWDesc(mappings != NULL, "SpriteSet::create(): NULL mappings");

	AssertWDesc(numSprites > 0, "SpriteSet::create(): numSprites == 0");
	AssertWDesc(numSubSprites > 0, "SpriteSet::create(): numSubSprites == 0");
	AssertWDesc(numSpriteMappings > 0, "SpriteSet::create(): numSpriteMappings == 0");

	_memUsage = 0;

	addTextureAtlas(atlas);

	_sprites = new Sprite[numSprites];
	_numSprites = numSprites;

	_subSprites = new SubSprite[numSubSprites];
	_numSubSprites = numSubSprites;

	_spriteMappings = new SpriteMapping[numSpriteMappings];
	_numSpriteMappings = numSpriteMappings;

	AssertWDesc(_sprites != NULL, "SpriteSet::create(): NULL _sprites.")
	AssertWDesc(_subSprites != NULL, "SpriteSet::create(): NULL _subSprites.")
	AssertWDesc(_spriteMappings != NULL, "SpriteSet::create(): NULL _spriteMappings.")

	if (_sprites && _subSprites && _spriteMappings)
	{
		_memUsage = sizeof(Sprite) * numSprites + sizeof(SubSprite) * numSubSprites + sizeof(SpriteMapping) * numSpriteMappings;

		for (i=0;i<_numSprites;i++)
		{
			_sprites[i].spriteSet = this;
			_sprites[i].control = sprites[i].control;
			_sprites[i].x = sprites[i].x;
			_sprites[i].y = sprites[i].y;
			_sprites[i].scaleX = sprites[i].scaleX;
			_sprites[i].scaleY = sprites[i].scaleY;
			_sprites[i].rotateZ = sprites[i].rotateZ;
			_sprites[i].redMultiplier = sprites[i].redMultiplier;
			_sprites[i].greenMultiplier = sprites[i].greenMultiplier;
			_sprites[i].blueMultiplier = sprites[i].blueMultiplier;
			_sprites[i].alphaMultiplier = sprites[i].alphaMultiplier;
			_sprites[i].redOffset = sprites[i].redOffset;
			_sprites[i].greenOffset = sprites[i].greenOffset;
			_sprites[i].blueOffset = sprites[i].blueOffset;
			_sprites[i].alphaOffset = sprites[i].alphaOffset;
			_sprites[i].width = sprites[i].width;
			_sprites[i].height = sprites[i].height;
			_sprites[i].numChildren = sprites[i].numChildren;
			_sprites[i].children = &_spriteMappings[sprites[i].firstChildIndex];
		}

		for (i=0;i<_numSubSprites;i++)
		{
			_subSprites[i].spriteSet = this;
			_subSprites[i].control = subSprites[i].control;
			_subSprites[i].x = subSprites[i].x;
			_subSprites[i].y = subSprites[i].y;
			_subSprites[i].scaleX = subSprites[i].scaleX;
			_subSprites[i].scaleY = subSprites[i].scaleY;
			_subSprites[i].rotateZ = subSprites[i].rotateZ;
			_subSprites[i].redMultiplier = subSprites[i].redMultiplier;
			_subSprites[i].greenMultiplier = subSprites[i].greenMultiplier;
			_subSprites[i].blueMultiplier = subSprites[i].blueMultiplier;
			_subSprites[i].alphaMultiplier = subSprites[i].alphaMultiplier;
			_subSprites[i].redOffset = subSprites[i].redOffset;
			_subSprites[i].greenOffset = subSprites[i].greenOffset;
			_subSprites[i].blueOffset = subSprites[i].blueOffset;
			_subSprites[i].alphaOffset = subSprites[i].alphaOffset;
			_subSprites[i].numChildren = subSprites[i].numChildren;
			_subSprites[i].children = &_spriteMappings[subSprites[i].firstChildIndex];
		}

		memcpy(_spriteMappings, mappings, sizeof(SpriteMapping) * numSpriteMappings);
	}
	else
	{
		destroy();
		error = EOSErrorNoMemory;
	}

	if (error == EOSErrorNone)
	{
		if (_spriteSetMgr)
			_spriteSetMgr->updateUsage();
	}

	AssertWDesc(error == EOSErrorNone, "SpriteSet::create error");

	return error;
}
#endif

EOSError SpriteSet::create(ObjectID objid, const Char* name, TextureAtlas* atlas, Uint8* data, Uint32 datasize)
{
	EOSError 			error = EOSErrorNone;
	SpriteSetHeader*	header = (SpriteSetHeader*) data;
	Endian				endian;
	MasterSpriteRAW*	masters;
	SpriteRAW*			sprites;
	SpriteMapping*		mappings;
	Point2D*			xlateList;
	SpriteTransform*	xformList;
	SpriteMatrix*		matList;
	SpriteColorOp*		colorList;
	Uint32				i; 

	AssertWDesc(data != NULL, "SpriteSet::create() NULL data.");

	if (data)
	{
		if (header->endian == 0x04030201)
			endian.switchEndian();
	
		_id = objid;
		setName(name);

		destroy();
	
		_numSprites = endian.swapUint32(header->numSprites);
		_numSubSprites = endian.swapUint32(header->numSubSprites);
		_numSpriteMappings = endian.swapUint32(header->numSpriteMappings);
		_numSpriteTranslates = endian.swapUint32(header->numSpriteTranslates);
		_numSpriteTransforms = endian.swapUint32(header->numSpriteTransforms);
		_numSpriteMatrices = endian.swapUint32(header->numSpriteMatrices);
		_numSpriteColorOps = endian.swapUint32(header->numSpriteColorOps);
	
		masters = (MasterSpriteRAW*) (data + endian.swapUint32(header->sprites));
		sprites = (SpriteRAW*) (data + endian.swapUint32(header->subSprites));
		mappings = (SpriteMapping*) (data + endian.swapUint32(header->spriteMappings));
		xlateList = (Point2D*) (data + endian.swapUint32(header->spriteTranslates));
		xformList = (SpriteTransform*) (data + endian.swapUint32(header->spriteTransforms));
		matList = (SpriteMatrix*) (data + endian.swapUint32(header->spriteMatrices));
		colorList = (SpriteColorOp*) (data + endian.swapUint32(header->spriteColorOps));

		_sprites = new Sprite[_numSprites];
		_subSprites = new SubSprite[_numSubSprites];
		_spriteMappings = new SpriteMapping[_numSpriteMappings];

		if (_numSpriteTranslates > 0)
			_spriteTranslates = new Point2D[_numSpriteTranslates];

		if (_numSpriteTransforms > 0)
			_spriteTransforms = new SpriteTransform[_numSpriteTransforms];

		if (_numSpriteMatrices > 0)
			_spriteMatrices = new SpriteMatrix[_numSpriteMatrices];

		if (_numSpriteColorOps > 0)
			_spriteColorOps = new SpriteColorOp[_numSpriteColorOps];

		AssertWDesc(_sprites != NULL, "SpriteSet::create Memory failed");
		AssertWDesc(_subSprites != NULL, "SpriteSet::create Memory failed");
		AssertWDesc(_spriteMappings != NULL, "SpriteSet::create Memory failed");
		AssertWDesc(_spriteTranslates != NULL || _spriteTransforms != NULL || _spriteMatrices != NULL, "SpriteSet::create Memory failed");
	
		if (_sprites && _subSprites && _spriteMappings && (_spriteTranslates || _spriteTransforms || _spriteMatrices))
		{
			_memUsage = sizeof(Sprite) * _numSprites + sizeof(SubSprite) * _numSubSprites + sizeof(SpriteMapping) * _numSpriteMappings + sizeof(Point2D) * _numSpriteTranslates + sizeof(SpriteTransform) * _numSpriteTransforms + sizeof(SpriteMatrix) * _numSpriteMatrices + sizeof(SpriteColorOp) * _numSpriteColorOps;

			for (i=0;i<_numSprites;i++)
			{
				_sprites[i].spriteSet = this;
				_sprites[i].control = endian.swapUint32(masters[i].control);

				if (masters[i].spriteTranslate != 0xFFFFFFFF)
					_sprites[i].xy = &_spriteTranslates[endian.swapUint32(masters[i].spriteTranslate)];
				else
					_sprites[i].xy = NULL;

				if (_sprites[i].control & CONTROL_TRANSFORM_MATRIX)
					_sprites[i].transform = (Float32*) &_spriteMatrices[endian.swapUint32(masters[i].spriteTransform)];
				else
					_sprites[i].transform = (Float32*) &_spriteTransforms[endian.swapUint32(masters[i].spriteTransform)];

				if (masters[i].spriteColorOp != 0xFFFFFFFF)
					_sprites[i].colorOp = &_spriteColorOps[endian.swapUint32(masters[i].spriteColorOp)];
				else
					_sprites[i].colorOp = NULL;

				_sprites[i].width = endian.swapFloat32(masters[i].width);
				_sprites[i].height = endian.swapFloat32(masters[i].height);

				_sprites[i].numChildren = endian.swapUint32(masters[i].numChildren);
				_sprites[i].children = &_spriteMappings[endian.swapUint32(masters[i].firstChildIndex)];
			}

			for (i=0;i<_numSubSprites;i++)
			{
				_subSprites[i].spriteSet = this;
				_subSprites[i].control = endian.swapUint32(sprites[i].control);

				if (sprites[i].spriteTranslate != 0xFFFFFFFF)
					_subSprites[i].xy = &_spriteTranslates[endian.swapUint32(sprites[i].spriteTranslate)];
				else
					_subSprites[i].xy = NULL;

				if (_subSprites[i].control & CONTROL_TRANSFORM_MATRIX)
					_subSprites[i].transform = (Float32*) &_spriteMatrices[endian.swapUint32(sprites[i].spriteTransform)];
				else
					_subSprites[i].transform = (Float32*) &_spriteTransforms[endian.swapUint32(sprites[i].spriteTransform)];

				if (sprites[i].spriteColorOp != 0xFFFFFFFF)
					_subSprites[i].colorOp = &_spriteColorOps[endian.swapUint32(sprites[i].spriteColorOp)];
				else
					_subSprites[i].colorOp = NULL;

				_subSprites[i].numChildren = endian.swapUint32(sprites[i].numChildren);
				_subSprites[i].children = &_spriteMappings[endian.swapUint32(sprites[i].firstChildIndex)];
			}

			for (i=0;i<_numSpriteMappings;i++)
			{
				_spriteMappings[i].mapControl = endian.swapUint32(mappings[i].mapControl);
				_spriteMappings[i].offset = endian.swapUint32(mappings[i].offset);
			}

			for (i=0;i<_numSpriteTranslates;i++)
			{
				_spriteTranslates[i].x = endian.swapFloat32(xlateList[i].x);
				_spriteTranslates[i].y = endian.swapFloat32(xlateList[i].y);
			}

			for (i=0;i<_numSpriteTransforms;i++)
			{
				_spriteTransforms[i].scaleX = endian.swapFloat32(xformList[i].scaleX);
				_spriteTransforms[i].scaleY = endian.swapFloat32(xformList[i].scaleY);
				_spriteTransforms[i].rotateZ = endian.swapFloat32(xformList[i].rotateZ);
			}

			for (i=0;i<_numSpriteMatrices;i++)
			{
				_spriteMatrices[i].a = endian.swapFloat32(matList[i].a);
				_spriteMatrices[i].b = endian.swapFloat32(matList[i].b);
				_spriteMatrices[i].c = endian.swapFloat32(matList[i].c);
				_spriteMatrices[i].d = endian.swapFloat32(matList[i].d);
			}

			for (i=0;i<_numSpriteColorOps;i++)
			{
				_spriteColorOps[i].redMultiplier = endian.swapFloat32(colorList[i].redMultiplier);
				_spriteColorOps[i].greenMultiplier = endian.swapFloat32(colorList[i].greenMultiplier);
				_spriteColorOps[i].blueMultiplier = endian.swapFloat32(colorList[i].blueMultiplier);
				_spriteColorOps[i].alphaMultiplier = endian.swapFloat32(colorList[i].alphaMultiplier);
				_spriteColorOps[i].redOffset = endian.swapFloat32(colorList[i].redOffset);
				_spriteColorOps[i].greenOffset = endian.swapFloat32(colorList[i].greenOffset);
				_spriteColorOps[i].blueOffset = endian.swapFloat32(colorList[i].blueOffset);
				_spriteColorOps[i].alphaOffset = endian.swapFloat32(colorList[i].alphaOffset);
			}
		}
		else
			error = EOSErrorNoMemory;
	}
	else
		error = EOSErrorNULL;

	if (error == EOSErrorNone)
	{
		if (_spriteSetMgr)
			_spriteSetMgr->updateUsage();
	}

	AssertWDesc(error == EOSErrorNone, "SpriteSet::create error");

	return error;
}

EOSError SpriteSet::createFromFile(const Char* filename, ObjectID objid, const Char* name, TextureAtlas* atlas, Uint8* buffer, Uint32 maxBufferSize)
{
	EOSError	error = EOSErrorNone;
	FileDescriptor	fdesc;
	File			file;
	
	AssertWDesc(name != NULL, "SpriteSet::createFromFile() NULL name");
	
	fdesc.setFileAccessType(FileAccessTypeReadOnly);
	
	fdesc.setFilename(filename);
	
	file.setFileDescriptor(fdesc);
	
	error = file.open();
	
	if (error == EOSErrorNone)
	{
		error = file.readUint8(buffer, file.length());
		
		if (error == EOSErrorNone)
		{
			error = create(objid, name, atlas, buffer, file.length());
		}
		
		file.close();
	}
	else
	{
		AssertWDesc(1 == 0, "Could not load\n");
	}
	
	return error;
}

void SpriteSet::destroy(void)
{
	if (_sprites)
	{
		delete [] _sprites;

		_sprites = NULL;
		_numSprites = 0;
	}

	if (_subSprites)
	{
		delete [] _subSprites;

		_subSprites = NULL;
		_numSubSprites = 0;
	}

	if (_spriteMappings)
	{
		delete [] _spriteMappings;

		_spriteMappings = NULL;
		_numSpriteMappings = 0;
	}

	if (_spriteTranslates)
	{
		delete [] _spriteTranslates;

		_spriteTranslates = NULL;
		_numSpriteTranslates = 0;
	}

	if (_spriteTransforms)
	{
		delete [] _spriteTransforms;

		_spriteTransforms = NULL;
		_numSpriteTransforms = 0;
	}

	if (_spriteMatrices)
	{
		delete [] _spriteMatrices;

		_spriteMatrices = NULL;
		_numSpriteMatrices = 0;
	}

	if (_spriteColorOps)
	{
		delete [] _spriteColorOps;

		_spriteColorOps = NULL;
		_numSpriteColorOps = 0;
	}

	if (_textureAtlas)
	{
		delete _textureAtlas;

		_textureAtlas = NULL;
		_numTextureAtlas = 0;
	}

	if (_spriteSetTextureAtlasMapping)
	{
		_spriteSetTextureAtlasMapping->setUsed(false);
	}

	_spriteSetTextureAtlasMapping = NULL;
	_textureAtlasMapping = NULL;
	_textureAtlasSubTexIDMapping = NULL;

	_memUsage = 0;

	if (_spriteSetMgr)
		_spriteSetMgr->updateUsage();
}

void SpriteSet::addTextureAtlas(TextureAtlas* atlas)
{
	TextureAtlas**	newAtlas;

	newAtlas = new TextureAtlas*[_numTextureAtlas + 1];

	if (newAtlas)
	{
		if (_textureAtlas)
		{
			memcpy(newAtlas, _textureAtlas, sizeof(TextureAtlas*) * _numTextureAtlas);
			delete _textureAtlas;
		}

		_textureAtlas = newAtlas;

		_textureAtlas[_numTextureAtlas] = atlas;

		_numTextureAtlas++;
	}
}

TextureAtlasSubTexture*	SpriteSet::getTextureAtlasSubTexture(ObjectID objid)
{
	TextureAtlas*			atlas;
	TextureAtlasSubTexture*	subtex;

	if (_textureAtlasMapping)
	{
		atlas = _textureAtlas[_textureAtlasMapping[objid]];
		subtex = atlas->getSubTexture(_textureAtlasSubTexIDMapping[objid]);
	}
	else
	{
		atlas = _textureAtlas[0];
		subtex = atlas->getSubTexture(objid);
	}

	return subtex;
}

TextureAtlasSubTexture*	SpriteSet::findTextureAtlasSubTexture(const Char* name)
{
	TextureAtlasSubTexture*	subtex = NULL;
	Uint32					i;

	for (i=0;i<_numTextureAtlas;i++)
	{
		subtex = _textureAtlas[i]->findSubTextureFromName(name);

		if (subtex)
			break;
	}

	return subtex;
}

void SpriteSet::setTextureAtlasMapping(SpriteSetTextureAtlasMapping* mapping)
{
	_spriteSetTextureAtlasMapping = mapping;
	_textureAtlasMapping = mapping->getTextureAtlasMappings();
	_textureAtlasSubTexIDMapping = mapping->getTextureAtlasSubTexIDMappings();
}

void SpriteSet::setUsed(Boolean used)
{
	_used = used;

	if (_spriteSetMgr)
		_spriteSetMgr->updateUsage();
}

void SpriteSet::setRefID(ObjectID objid)
{
	_id = objid;
}

void SpriteSet::setName(const Char* name)
{
	if (name)
	{
		if (_name)
		{
			delete _name;
			_name = NULL;
		}
	
		_name = new Char[strlen(name) + 1];
	
		if (_name)
		{
			strcpy(_name, name);
		}
	}
}

