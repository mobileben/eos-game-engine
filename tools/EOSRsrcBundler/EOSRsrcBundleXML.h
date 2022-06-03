/******************************************************************************
 *
 * File: EOSRsrcBundleXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * EOS Rsrc Bundle
 * 
 *****************************************************************************/

#ifndef __EOSRSRCBUNDLEXML_H__
#define __EOSRSRCBUNDLEXML_H__

#include "Platform.h"
#include "Graphics.h"
#include "Texture.h"
#include "TextureState.h"
#include "EOSRsrcBundle.h"
#include "toolkit/XMLElement.h"
#include "toolkit/XMLParser.h"
#include <libxml/xmlreader.h>

typedef struct
{
	Uint32						nameOffset;
	Uint32						numTextures;
	EOSRsrcBundleTextureExport*		textures;

	Uint32						numTextureAtlases;
	EOSRsrcBundleTextureAtlasExport*	textureAtlases;

	Uint32									numSpriteTextureAtlasMappings;
	EOSRsrcBundleSpriteTextureAtlasMappingExport*	spriteTextureAtlasMappings;

	Uint32						numSprites;
	EOSRsrcBundleSpriteExport*			sprites;

	Uint32						numSpriteAnims;
	EOSRsrcBundleSpriteAnimExport*		spriteAnims;

	Uint32						dataSize;
	Uint8*						data;

	Uint32						numNameOffsets;
	Uint32*						nameOffsets;

	Uint32						namesSize;
	Char*						names;
} EOSRsrcBundleDatabaseExport;

typedef enum
{
	EOSRsrcBundleXMLElementTypeBundle = 0,
	EOSRsrcBundleXMLElementTypeName, 
	EOSRsrcBundleXMLElementTypeFilename, 
	EOSRsrcBundleXMLElementTypeTextureName, 
	EOSRsrcBundleXMLElementTypeSpriteName, 
	EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingName,
	EOSRsrcBundleXMLElementTypeID, 
	EOSRsrcBundleXMLElementTypeWidth, 
	EOSRsrcBundleXMLElementTypeHeight, 
	EOSRsrcBundleXMLElementTypeColorFormat, 
	EOSRsrcBundleXMLElementTypeMinMipMapFilter, 
	EOSRsrcBundleXMLElementTypeMagMipMapFilter, 
	EOSRsrcBundleXMLElementTypeWrapS, 
	EOSRsrcBundleXMLElementTypeWrapT, 
	EOSRsrcBundleXMLElementTypeTexEnvMode, 

	EOSRsrcBundleXMLElementTypeTexture, 
	EOSRsrcBundleXMLElementTypeTextureDB, 
	EOSRsrcBundleXMLElementTypeTextureAtlas, 
	EOSRsrcBundleXMLElementTypeTextureAtlasDB,
	EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMapping, 
	EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingDB,
	EOSRsrcBundleXMLElementTypeSprite, 	//	SpriteSet
	EOSRsrcBundleXMLElementTypeSpriteDB, //	SpriteSet DB
	EOSRsrcBundleXMLElementTypeSpriteAnim, 	//	SpriteAnimSet
	EOSRsrcBundleXMLElementTypeSpriteAnimDB, //	SpriteAnimSet DB

	EOSRsrcBundleXMLElementTypeTextureAtlasNameList,

	EOSRsrcBundleXMLElementTypeLast,
	EOSRsrcBundleXMLElementTypeIllegal = EOSRsrcBundleXMLElementTypeLast
} EOSRsrcBundleXMLElementType;

class XMLElementName : public XMLElementString
{
public:
	XMLElementName() { setType(EOSRsrcBundleXMLElementTypeName); }
	~XMLElementName() {}
};

class XMLElementID : public XMLElementSint32
{
public:
	XMLElementID() { setType(EOSRsrcBundleXMLElementTypeID); }
	~XMLElementID() {}
};

class XMLElementFilename : public XMLElementString
{
public:
	XMLElementFilename() { setType(EOSRsrcBundleXMLElementTypeFilename); }
	~XMLElementFilename() {}
};

class XMLElementTextureName : public XMLElementString
{
public:
	XMLElementTextureName() { setType(EOSRsrcBundleXMLElementTypeTextureName); }
	~XMLElementTextureName() {}
};

class XMLElementSpriteName : public XMLElementString
{
public:
	XMLElementSpriteName() { setType(EOSRsrcBundleXMLElementTypeSpriteName); }
	~XMLElementSpriteName() {}
};

class XMLElementSpriteTextureAtlasMappingName : public XMLElementString
{
public:
	XMLElementSpriteTextureAtlasMappingName() { setType(EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingName); }
	~XMLElementSpriteTextureAtlasMappingName() {}
};

class XMLElementWidth : public XMLElementSint32
{
public:
	XMLElementWidth() { setType(EOSRsrcBundleXMLElementTypeWidth); }
	~XMLElementWidth() {}
};

class XMLElementHeight : public XMLElementSint32
{
public:
	XMLElementHeight() { setType(EOSRsrcBundleXMLElementTypeHeight); }
	~XMLElementHeight() {}
};

class XMLElementColorFormat : public XMLElementString
{
protected:
	TextureFormat	_format;

public:
	XMLElementColorFormat();
	~XMLElementColorFormat();

	inline TextureFormat				getColorFormat(void) { return _format; }


	EOSError							setValue(const xmlChar* value);
	EOSError							end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementMipMapFilter : public XMLElementString
{
protected:
	TextureState::TEXTURE_MIPMAP	_mipmap;

public:
	XMLElementMipMapFilter();
	~XMLElementMipMapFilter();

	inline TextureState::TEXTURE_MIPMAP	getMipMapFilter(void) { return _mipmap; }


	EOSError							setValue(const xmlChar* value);
	EOSError							end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementMinMipMapFilter : public XMLElementMipMapFilter
{
public:
	XMLElementMinMipMapFilter() { setType(EOSRsrcBundleXMLElementTypeMinMipMapFilter); }
	~XMLElementMinMipMapFilter() {}
};

class XMLElementMagMipMapFilter : public XMLElementMipMapFilter
{
public:
	XMLElementMagMipMapFilter() { setType(EOSRsrcBundleXMLElementTypeMagMipMapFilter); }
	~XMLElementMagMipMapFilter() {}
};

class XMLElementWrap : public XMLElementString
{
protected:
	TextureState::TEXTURE_WRAP	_wrap;

public:
	XMLElementWrap();
	~XMLElementWrap();

	inline TextureState::TEXTURE_WRAP	getWrap(void) { return _wrap; }


	EOSError							setValue(const xmlChar* value);
	EOSError							end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementWrapS : public XMLElementWrap
{
public:
	XMLElementWrapS() { setType(EOSRsrcBundleXMLElementTypeWrapS); }
	~XMLElementWrapS() {}
};

class XMLElementWrapT : public XMLElementWrap
{
public:
	XMLElementWrapT() { setType(EOSRsrcBundleXMLElementTypeWrapT); }
	~XMLElementWrapT() {}
};

class XMLElementTexEnvMode : public XMLElementString
{
protected:
	TextureState::TEXTURE_ENV_MODE	_texenvmode;

public:
	XMLElementTexEnvMode();
	~XMLElementTexEnvMode();

	inline TextureState::TEXTURE_ENV_MODE	getTexEnvMode(void) { return _texenvmode; }


	EOSError							setValue(const xmlChar* value);
	EOSError							end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementTexture : public XMLElementNode
{
protected:
	XMLElementName*				_name;
	XMLElementFilename*			_filename;
	XMLElementWidth*			_width;
	XMLElementHeight*			_height;
	XMLElementColorFormat*		_colorFormat;
	XMLElementMinMipMapFilter*	_minMipMapFilter;
	XMLElementMagMipMapFilter*	_magMipMapFilter;
	XMLElementWrapS*			_wrapS;
	XMLElementWrapT*			_wrapT;
	XMLElementTexEnvMode*		_texEnvMode;

public:
	XMLElementTexture();
	~XMLElementTexture();

	inline XMLElementName*				getName(void) { return _name; }
	inline XMLElementFilename*			getFilename(void) { return _filename; }
	inline XMLElementWidth*				getWidth(void) { return _width; }
	inline XMLElementHeight*			getHeight(void) { return _height; }
	inline XMLElementColorFormat*		getColorFormat(void) { return _colorFormat; }
	inline XMLElementMinMipMapFilter*	getMinMipMapFilter(void) { return _minMipMapFilter; }
	inline XMLElementMagMipMapFilter*	getMagMipMapFilter(void) { return _magMipMapFilter; }
	inline XMLElementWrapS*				getWrapS(void) { return _wrapS; }
	inline XMLElementWrapT*				getWrapT(void) { return _wrapT; }
	inline XMLElementTexEnvMode*		getTexEnvMode(void) { return _texEnvMode; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementTextureDB : public XMLElementNode
{
protected:
	XMLElementName*		_name;

	Uint32				_numTextures;
	XMLElementTexture**	_textures;

public:
	XMLElementTextureDB();
	~XMLElementTextureDB();

	inline Uint32			getNumTextures(void) { return _numTextures; }
	inline XMLElementName*	getName(void) { return _name; }
	XMLElementTexture*		getTextureAtIndex(Uint32 index);

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementTextureAtlas : public XMLElementNode
{
protected:
	XMLElementName*				_name;
	XMLElementFilename*			_filename;
	XMLElementName*				_textureToBind;

public:
	XMLElementTextureAtlas();
	~XMLElementTextureAtlas();

	inline XMLElementName*				getName(void) { return _name; }
	inline XMLElementFilename*			getFilename(void) { return _filename; }
	inline XMLElementName*				getTextureName(void) { return _textureToBind; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementTextureAtlasDB : public XMLElementNode
{
protected:
	XMLElementName*		_name;

	Uint32						_numTextureAtlases;
	XMLElementTextureAtlas**	_textureAtlases;

public:
	XMLElementTextureAtlasDB();
	~XMLElementTextureAtlasDB();

	inline Uint32				getNumTextureAtlases(void) { return _numTextureAtlases; }
	inline XMLElementName*		getName(void) { return _name; }
	XMLElementTextureAtlas*		getTextureAtlasAtIndex(Uint32 index);

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementSpriteTextureAtlasMapping : public XMLElementNode
{
protected:
	XMLElementName*				_name;
	XMLElementFilename*			_filename;

public:
	XMLElementSpriteTextureAtlasMapping();
	~XMLElementSpriteTextureAtlasMapping();

	inline XMLElementName*				getName(void) { return _name; }
	inline XMLElementFilename*			getFilename(void) { return _filename; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementSpriteTextureAtlasMappingDB : public XMLElementNode
{
protected:
	XMLElementName*							_name;

	Uint32									_numMappings;
	XMLElementSpriteTextureAtlasMapping**	_mappings;

public:
	XMLElementSpriteTextureAtlasMappingDB();
	~XMLElementSpriteTextureAtlasMappingDB();

	inline Uint32								getNumSpriteTextureAtlasMappings(void) { return _numMappings; }
	inline XMLElementName*						getName(void) { return _name; }
	XMLElementSpriteTextureAtlasMapping*		getSpriteTextureAtlasMappingAtIndex(Uint32 index);

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementTextureAtlasNameList : public XMLElementNode
{
protected:
	Uint32				_numNames;
	XMLElementName**	_names;

public:
	XMLElementTextureAtlasNameList();
	~XMLElementTextureAtlasNameList();

	inline Uint32		getNumNames(void) { return _numNames; }
	XMLElementName*		getNameAtIndex(Uint32 index);
	inline XMLElementName**	getNameList(void) { return _names; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementSprite : public XMLElementNode
{
protected:
	XMLElementName*									_name;
	XMLElementFilename*								_filename;
	XMLElementTextureAtlasNameList*					_texAtlasNameList;
	XMLElementSpriteTextureAtlasMappingName*		_sprTexAtlasMappingName;

public:
	XMLElementSprite();
	~XMLElementSprite();

	inline XMLElementName*								getName(void) { return _name; }
	inline XMLElementFilename*							getFilename(void) { return _filename; }
	inline XMLElementTextureAtlasNameList*				getTextureAtlasNameList(void) { return _texAtlasNameList; }
	inline XMLElementSpriteTextureAtlasMappingName*		getSpriteTextureAtlasMappingName(void) { return _sprTexAtlasMappingName; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementSpriteDB : public XMLElementNode
{
protected:
	XMLElementName*			_name;

	Uint32					_numSprites;
	XMLElementSprite**		_sprites;

public:
	XMLElementSpriteDB();
	~XMLElementSpriteDB();

	inline Uint32						getNumSprites(void) { return _numSprites; }
	inline XMLElementName*				getName(void) { return _name; }
	XMLElementSprite*					getSpriteAtIndex(Uint32 index);

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};


class XMLElementSpriteAnim : public XMLElementNode
{
protected:
	XMLElementName*							_name;
	XMLElementFilename*						_filename;
	XMLElementSpriteName*					_spriteName;

public:
	XMLElementSpriteAnim();
	~XMLElementSpriteAnim();

	inline XMLElementName*					getName(void) { return _name; }
	inline XMLElementFilename*				getFilename(void) { return _filename; }
	inline XMLElementSpriteName*			getSpriteName(void) { return _spriteName; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementSpriteAnimDB : public XMLElementNode
{
protected:
	XMLElementName*			_name;

	Uint32					_numSpriteAnims;
	XMLElementSpriteAnim**	_spriteAnims;

public:
	XMLElementSpriteAnimDB();
	~XMLElementSpriteAnimDB();

	inline Uint32						getNumSpriteAnims(void) { return _numSpriteAnims; }
	inline XMLElementName*				getName(void) { return _name; }
	XMLElementSpriteAnim*				getSpriteAnimAtIndex(Uint32 index);

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class XMLElementBundle : public XMLElementNode
{
protected:
	XMLElementName*				_name;

	XMLElementTextureDB*		_textureDB;
	XMLElementTextureAtlasDB*	_textureAtlasDB;
	XMLElementSpriteTextureAtlasMappingDB*	_mappingDB;
	XMLElementSpriteDB*			_spriteDB;
	XMLElementSpriteAnimDB*		_spriteAnimDB;

public:
	XMLElementBundle();
	~XMLElementBundle();

	inline XMLElementName*				getName(void) { return _name; }
	inline XMLElementTextureDB*			getTextureDB(void) { return _textureDB; }
	inline XMLElementTextureAtlasDB*	getTextureAtlasDB(void) { return _textureAtlasDB; }
	inline XMLElementSpriteTextureAtlasMappingDB*	getSpriteTextureAtlasMappingDB(void) { return _mappingDB; }
	inline XMLElementSpriteDB*			getSpriteDB(void) { return _spriteDB; }
	inline XMLElementSpriteAnimDB*		getSpriteAnimDB(void) { return _spriteAnimDB; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class EOSRsrcBundleXMLParser : public XMLParser
{
private:
	EOSRsrcBundleDatabaseExport	_exportDB;

	EOSError				_addName(EOSRsrcBundleDatabaseExport& db, Char* name, Uint32& offset);
	EOSError				_addData(EOSRsrcBundleDatabaseExport& db, Uint8* data, Uint32 length, Uint32& offset);
	EOSError				_addDataFromFile(EOSRsrcBundleDatabaseExport& db, Char* name, Uint32& length, Uint32& offset);
	EOSError				_addNameList(EOSRsrcBundleDatabaseExport& db, Uint32 num, XMLElementName** namelist, Uint32& offset);

public:
	EOSRsrcBundleXMLParser();
	~EOSRsrcBundleXMLParser();

	XMLElement*		_buildXMLElementFromName(const xmlChar* name);

	EOSError		buildDB(void);
	EOSError		exportDB(Char* filename);
};

#endif /* __EOSRSRCBUNDLEXML_H__ */

