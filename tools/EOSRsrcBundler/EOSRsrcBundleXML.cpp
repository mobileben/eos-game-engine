/******************************************************************************
 *
 * File: EOSRsrcBundleXML.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * EOS Rsrc Bundle
 * 
 *****************************************************************************/

#include "Platform.h"
#include "EOSRsrcBundleXML.h"
#include "File.h"

Char* _colorFormatNames[TextureFormatLast + 1] = 
{
	"TextureFormatTGA",
	"TextureFormatGL_RGB_888",
	"TextureFormatGL_RGBA_8888",
	"TextureFormatGL_RGB_565",
	"TextureFormatGL_RGBA_5551",
	"TextureFormatGL_RGBA_4444",
	"TextureFormatGL_P8_RGBA_8888",
	"TextureFormatGL_P4_RGBA_4444",
	"TextureFormatPVRTC_4BPP_RGBA",
	"TextureFormatPVRTC_4BPP_RGB",
	"TextureFormatPVRTC_2BPP_RGBA",
	"TextureFormatPVRTC_2BPP_RGB",
	"TextureFormatRAWPVRTC_4BPP_RGBA",
	"TextureFormatRAWPVRTC_4BPP_RGB",
	"TextureFormatRAWPVRTC_2BPP_RGBA",
	"TextureFormatRAWPVRTC_2BPP_RGB",
	"TextureFormatIllegal",
};

Char* _texMipMapNames[TextureState::TEXTURE_MIPMAP_LAST + 1] = 
{
	"TEXTURE_MIPMAP_NEAREST",
	"TEXTURE_MIPMAP_LINEAR",
	"TEXTURE_MIPMAP_NEAREST_NEAREST",
	"TEXTURE_MIPMAP_NEAREST_LINEAR",
	"TEXTURE_MIPMAP_LINEAR_NEAREST",
	"TEXTURE_MIPMAP_LINEAR_LINEAR",
	"TEXTURE_MIPMAP_ILLEGAL",
};

Char* _texWrapNames[TextureState::TEXTURE_WRAP_LAST + 1] = 
{
	"TEXTURE_WRAP_CLAMP",
	"TEXTURE_WRAP_REPEAT",
	"TEXTURE_WRAP_CLAMP_BORDER",
	"TEXTURE_WRAP_CLAMP_EDGE",
	"TEXTURE_WRAP_ILLEGAL"
};

Char* _texEnvModeNames[TextureState::TEXTURE_ENV_MODE_LAST + 1] = 
{
	"TEXTURE_ENV_MODE_REPLACE",
	"TEXTURE_ENV_MODE_MODULATE",
	"TEXTURE_ENV_MODE_DECAL",
	"TEXTURE_ENV_MODE_BLEND",
	"TEXTURE_ENV_MODE_ADD",
	"TEXTURE_ENV_MODE_COMBINE",
	"TEXTURE_ENV_MODE_ILLEGAL",
};

Char* _xmlElementType[EOSRsrcBundleXMLElementTypeLast + 1] = 
{
	"bundle",
	"name",
	"filename",
	"texname",
	"spritename",
	"mappingname",
	"id",
	"width",
	"height",
	"colorformat",
	"minmipmapfilter",
	"magmipmapfilter",
	"wraps",
	"wrapt",
	"texenvmode",
	"tex",
	"texdb",
	"texatlas",
	"texatlasdb",
	"sprtexatlasmapping",
	"sprtexatlasmappingdb",
	"sprite",
	"spritedb",
	"spranim",
	"spranimdb",
	"texatlasnamelist",
	"EOSRsrcBundleXMLElementTypeIllegal",
};

EOSRsrcBundleXMLElementType	nameToEOSRsrcBundleXMLElementType(const xmlChar* name)
{
	EOSRsrcBundleXMLElementType 	type = EOSRsrcBundleXMLElementTypeIllegal;
	Uint32			i;

	for (i=0;i<EOSRsrcBundleXMLElementTypeLast;i++)
	{
		if (!strcmp(_xmlElementType[i], (const char*) name))
		{
			type = (EOSRsrcBundleXMLElementType) i;
			break;
		}
	}

	return type;
}

XMLElementColorFormat::XMLElementColorFormat() : _format(TextureFormatIllegal)
{
	setType(EOSRsrcBundleXMLElementTypeColorFormat);
}

XMLElementColorFormat::~XMLElementColorFormat()
{
}

EOSError XMLElementColorFormat::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureFormatLast;i++)
	{
		if (!strcmp(_colorFormatNames[i], (const char*) value))
		{
			_format = (TextureFormat) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementColorFormat::end(const xmlChar* elementName)
{
	if (_format != TextureFormatIllegal)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementMipMapFilter::XMLElementMipMapFilter() : _mipmap(TextureState::TEXTURE_MIPMAP_ILLEGAL)
{
}

XMLElementMipMapFilter::~XMLElementMipMapFilter()
{
}

EOSError XMLElementMipMapFilter::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureState::TEXTURE_MIPMAP_LAST;i++)
	{
		if (!strcmp(_texMipMapNames[i], (const char*) value))
		{
			_mipmap = (TextureState::TEXTURE_MIPMAP) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementMipMapFilter::end(const xmlChar* elementName)
{
	if (_mipmap != TextureState::TEXTURE_MIPMAP_ILLEGAL)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementWrap::XMLElementWrap() : _wrap(TextureState::TEXTURE_WRAP_ILLEGAL)
{
}

XMLElementWrap::~XMLElementWrap()
{
}

EOSError XMLElementWrap::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureState::TEXTURE_WRAP_LAST;i++)
	{
		if (!strcmp(_texWrapNames[i], (const char*) value))
		{
			_wrap = (TextureState::TEXTURE_WRAP) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementWrap::end(const xmlChar* elementName)
{
	if (_wrap != TextureState::TEXTURE_WRAP_ILLEGAL)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTexEnvMode::XMLElementTexEnvMode() : _texenvmode(TextureState::TEXTURE_ENV_MODE_ILLEGAL)
{
	setType(EOSRsrcBundleXMLElementTypeTexEnvMode);
}

XMLElementTexEnvMode::~XMLElementTexEnvMode()
{
}

EOSError XMLElementTexEnvMode::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureState::TEXTURE_ENV_MODE_LAST;i++)
	{
		if (!strcmp(_texEnvModeNames[i], (const char*) value))
		{
			_texenvmode = (TextureState::TEXTURE_ENV_MODE) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementTexEnvMode::end(const xmlChar* elementName)
{
	if (_texenvmode != TextureState::TEXTURE_ENV_MODE_ILLEGAL)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTexture::XMLElementTexture() : _name(NULL), _filename(NULL), _width(NULL), _height(NULL), _colorFormat(NULL), 
										_minMipMapFilter(NULL), _magMipMapFilter(NULL), _wrapS(NULL), _wrapT(NULL), _texEnvMode(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeTexture);
}

XMLElementTexture::~XMLElementTexture()
{
	_name = NULL;
	_filename = NULL;
	_width = NULL;
	_height = NULL;
	_colorFormat = NULL;
	_minMipMapFilter = NULL;
	_magMipMapFilter = NULL;
	_wrapS = NULL;
	_wrapT = NULL;
	_texEnvMode = NULL;
}

EOSError XMLElementTexture::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeFilename:
			_filename = (XMLElementFilename*) child;
			break;

		case EOSRsrcBundleXMLElementTypeWidth:
			_width = (XMLElementWidth*) child;
			break;

		case EOSRsrcBundleXMLElementTypeHeight:
			_height = (XMLElementHeight*) child;
			break;

		case EOSRsrcBundleXMLElementTypeColorFormat:
			_colorFormat = (XMLElementColorFormat*) child;
			break;

		case EOSRsrcBundleXMLElementTypeMinMipMapFilter:
			_minMipMapFilter = (XMLElementMinMipMapFilter*) child;
			break;

		case EOSRsrcBundleXMLElementTypeMagMipMapFilter:
			_magMipMapFilter = (XMLElementMagMipMapFilter*) child;
			break;

		case EOSRsrcBundleXMLElementTypeWrapS:
			_wrapS = (XMLElementWrapS*) child;
			break;

		case EOSRsrcBundleXMLElementTypeWrapT:
			_wrapT = (XMLElementWrapT*) child;
			break;

		case EOSRsrcBundleXMLElementTypeTexEnvMode:
			_texEnvMode = (XMLElementTexEnvMode*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementTexture::end(const xmlChar* element)
{
	if (_name && _filename && _width && _height && _colorFormat)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTextureDB::XMLElementTextureDB() : _name(NULL), _numTextures(0), _textures(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeTextureDB);
}

XMLElementTextureDB::~XMLElementTextureDB()
{
	_name = NULL;
	_numTextures = NULL;
	_textures = NULL;
}

XMLElementTexture* XMLElementTextureDB::getTextureAtIndex(Uint32 index)
{
	if (_textures && index < _numTextures)
		return _textures[index];
	else
		return NULL;
}

EOSError XMLElementTextureDB::addChild(XMLElement* child)
{
	EOSError 			error = EOSErrorNone;
	XMLElementTexture**	newlist;
	Uint32				i;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeTexture:
			newlist = new XMLElementTexture*[_numTextures + 1];

			if (newlist)
			{
				if (_textures)
				{
					for (i=0;i<_numTextures;i++)
					{
						newlist[i] = _textures[i];
					}

					delete _textures;
				}

				newlist[_numTextures] = (XMLElementTexture*) child;
				_textures = newlist;
				_numTextures++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementTextureDB::end(const xmlChar* element)
{
	if (_name && _textures)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}


XMLElementTextureAtlas::XMLElementTextureAtlas() : _name(NULL), _filename(NULL), _textureToBind(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeTextureAtlas);
}

XMLElementTextureAtlas::~XMLElementTextureAtlas()
{
	_name = NULL;
	_filename = NULL;
	_textureToBind = NULL;
}

EOSError XMLElementTextureAtlas::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeFilename:
			_filename = (XMLElementFilename*) child;
			break;

		case EOSRsrcBundleXMLElementTypeTextureName:
			_textureToBind = (XMLElementName*) child;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementTextureAtlas::end(const xmlChar* element)
{
	if (_name && _filename)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTextureAtlasDB::XMLElementTextureAtlasDB() : _name(NULL), _numTextureAtlases(0), _textureAtlases(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeTextureAtlasDB);
}

XMLElementTextureAtlasDB::~XMLElementTextureAtlasDB()
{
	_name = NULL;
	_numTextureAtlases = NULL;
	_textureAtlases = NULL;
}

XMLElementTextureAtlas* XMLElementTextureAtlasDB::getTextureAtlasAtIndex(Uint32 index)
{
	if (_textureAtlases && index < _numTextureAtlases)
		return _textureAtlases[index];
	else
		return NULL;
}

EOSError XMLElementTextureAtlasDB::addChild(XMLElement* child)
{
	EOSError 					error = EOSErrorNone;
	XMLElementTextureAtlas**	newlist;
	Uint32						i;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeTextureAtlas:
			newlist = new XMLElementTextureAtlas*[_numTextureAtlases + 1];

			if (newlist)
			{
				if (_textureAtlases)
				{
					for (i=0;i<_numTextureAtlases;i++)
					{
						newlist[i] = _textureAtlases[i];
					}

					delete _textureAtlases;
				}

				newlist[_numTextureAtlases] = (XMLElementTextureAtlas*) child;
				_textureAtlases = newlist;
				_numTextureAtlases++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementTextureAtlasDB::end(const xmlChar* element)
{
	if (_name && _textureAtlases)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSpriteTextureAtlasMapping::XMLElementSpriteTextureAtlasMapping() : _name(NULL), _filename(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMapping);
}

XMLElementSpriteTextureAtlasMapping::~XMLElementSpriteTextureAtlasMapping()
{
	_name = NULL;
	_filename = NULL;
}

EOSError XMLElementSpriteTextureAtlasMapping::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeFilename:
			_filename = (XMLElementFilename*) child;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSpriteTextureAtlasMapping::end(const xmlChar* element)
{
	if (_name && _filename)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSpriteTextureAtlasMappingDB::XMLElementSpriteTextureAtlasMappingDB() : _name(NULL), _numMappings(0), _mappings(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingDB);
}

XMLElementSpriteTextureAtlasMappingDB::~XMLElementSpriteTextureAtlasMappingDB()
{
	_name = NULL;
	_numMappings = NULL;
	_mappings = NULL;
}

XMLElementSpriteTextureAtlasMapping* XMLElementSpriteTextureAtlasMappingDB::getSpriteTextureAtlasMappingAtIndex(Uint32 index)
{
	if (_mappings && index < _numMappings)
		return _mappings[index];
	else
		return NULL;
}

EOSError XMLElementSpriteTextureAtlasMappingDB::addChild(XMLElement* child)
{
	EOSError 					error = EOSErrorNone;
	XMLElementSpriteTextureAtlasMapping**	newlist;
	Uint32						i;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMapping:
			newlist = new XMLElementSpriteTextureAtlasMapping*[_numMappings + 1];

			if (newlist)
			{
				if (_mappings)
				{
					for (i=0;i<_numMappings;i++)
					{
						newlist[i] = _mappings[i];
					}

					delete _mappings;
				}

				newlist[_numMappings] = (XMLElementSpriteTextureAtlasMapping*) child;
				_mappings = newlist;
				_numMappings++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSpriteTextureAtlasMappingDB::end(const xmlChar* element)
{
	if (_name && _mappings)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTextureAtlasNameList::XMLElementTextureAtlasNameList() : _numNames(0), _names(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeTextureAtlasNameList);
}

XMLElementTextureAtlasNameList::~XMLElementTextureAtlasNameList()
{
	_numNames = NULL;
	_names = NULL;
}

XMLElementName* XMLElementTextureAtlasNameList::getNameAtIndex(Uint32 index)
{
	if (index < _numNames)
		return _names[index];
	else
		return NULL;
}

EOSError XMLElementTextureAtlasNameList::addChild(XMLElement* child)
{
	EOSError 			error = EOSErrorNone;
	XMLElementName**	newlist;
	Uint32				i;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			newlist = new XMLElementName*[_numNames + 1];

			if (newlist)
			{
				if (_names)
				{
					for (i=0;i<_numNames;i++)
					{
						newlist[i] = _names[i];
					}

					delete _names;
				}

				newlist[_numNames] = (XMLElementName*) child;
				_names = newlist;
				_numNames++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementTextureAtlasNameList::end(const xmlChar* element)
{
	if (_names)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSprite::XMLElementSprite() : _name(NULL), _filename(NULL), _texAtlasNameList(NULL), _sprTexAtlasMappingName(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeSprite);
}

XMLElementSprite::~XMLElementSprite()
{
	_name = NULL;
	_filename = NULL;
	_texAtlasNameList = NULL;
	_sprTexAtlasMappingName = NULL;
}

EOSError XMLElementSprite::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeFilename:
			_filename = (XMLElementFilename*) child;
			break;

		case EOSRsrcBundleXMLElementTypeTextureAtlasNameList:
			_texAtlasNameList = (XMLElementTextureAtlasNameList*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingName:
			_sprTexAtlasMappingName = (XMLElementSpriteTextureAtlasMappingName*) child;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSprite::end(const xmlChar* element)
{
	if (_name && _filename)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSpriteDB::XMLElementSpriteDB() : _name(NULL), _numSprites(0), _sprites(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeSpriteDB);
}

XMLElementSpriteDB::~XMLElementSpriteDB()
{
	_name = NULL;
	_numSprites = NULL;
	_sprites = NULL;
}

XMLElementSprite* XMLElementSpriteDB::getSpriteAtIndex(Uint32 index)
{
	if (_sprites && index < _numSprites)
		return _sprites[index];
	else
		return NULL;
}

EOSError XMLElementSpriteDB::addChild(XMLElement* child)
{
	EOSError 					error = EOSErrorNone;
	XMLElementSprite**	newlist;
	Uint32						i;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSprite:
			newlist = new XMLElementSprite*[_numSprites + 1];

			if (newlist)
			{
				if (_sprites)
				{
					for (i=0;i<_numSprites;i++)
					{
						newlist[i] = _sprites[i];
					}

					delete _sprites;
				}

				newlist[_numSprites] = (XMLElementSprite*) child;
				_sprites = newlist;
				_numSprites++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSpriteDB::end(const xmlChar* element)
{
	if (_name && _sprites)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSpriteAnim::XMLElementSpriteAnim() : _name(NULL), _filename(NULL), _spriteName(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeSpriteAnim);
}

XMLElementSpriteAnim::~XMLElementSpriteAnim()
{
	_name = NULL;
	_filename = NULL;
	_spriteName = NULL;
}

EOSError XMLElementSpriteAnim::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeFilename:
			_filename = (XMLElementFilename*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSpriteName:
			_spriteName = (XMLElementSpriteName*) child;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSpriteAnim::end(const xmlChar* element)
{
	if (_name && _filename)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSpriteAnimDB::XMLElementSpriteAnimDB() : _name(NULL), _numSpriteAnims(0), _spriteAnims(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeSpriteAnimDB);
}

XMLElementSpriteAnimDB::~XMLElementSpriteAnimDB()
{
	_name = NULL;
	_numSpriteAnims = NULL;
	_spriteAnims = NULL;
}

XMLElementSpriteAnim* XMLElementSpriteAnimDB::getSpriteAnimAtIndex(Uint32 index)
{
	if (_spriteAnims && index < _numSpriteAnims)
		return _spriteAnims[index];
	else
		return NULL;
}

EOSError XMLElementSpriteAnimDB::addChild(XMLElement* child)
{
	EOSError 				error = EOSErrorNone;
	XMLElementSpriteAnim**	newlist;
	Uint32					i;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSpriteAnim:
			newlist = new XMLElementSpriteAnim*[_numSpriteAnims + 1];

			if (newlist)
			{
				if (_spriteAnims)
				{
					for (i=0;i<_numSpriteAnims;i++)
					{
						newlist[i] = _spriteAnims[i];
					}

					delete _spriteAnims;
				}

				newlist[_numSpriteAnims] = (XMLElementSpriteAnim*) child;
				_spriteAnims = newlist;
				_numSpriteAnims++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSpriteAnimDB::end(const xmlChar* element)
{
	if (_name && _spriteAnims)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}


XMLElementBundle::XMLElementBundle() : _name(NULL), _textureDB(NULL), _textureAtlasDB(NULL), _mappingDB(NULL), _spriteDB(NULL), _spriteAnimDB(NULL)
{
	setType(EOSRsrcBundleXMLElementTypeBundle);
}

XMLElementBundle::~XMLElementBundle()
{
	_name = NULL;
	_textureDB = NULL;
	_textureAtlasDB = NULL;
	_mappingDB = NULL;
	_spriteDB = NULL;
	_spriteAnimDB = NULL;
}

EOSError XMLElementBundle::addChild(XMLElement* child)
{
	EOSError 			error = EOSErrorNone;

	switch (child->getType())
	{
		case EOSRsrcBundleXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case EOSRsrcBundleXMLElementTypeTextureDB:
			_textureDB = (XMLElementTextureDB*) child;
			break;

		case EOSRsrcBundleXMLElementTypeTextureAtlasDB:
			_textureAtlasDB = (XMLElementTextureAtlasDB*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingDB:
			_mappingDB = (XMLElementSpriteTextureAtlasMappingDB*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSpriteDB:
			_spriteDB = (XMLElementSpriteDB*) child;
			break;

		case EOSRsrcBundleXMLElementTypeSpriteAnimDB:
			_spriteAnimDB = (XMLElementSpriteAnimDB*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementBundle::end(const xmlChar* element)
{
	if (_name)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

EOSRsrcBundleXMLParser::EOSRsrcBundleXMLParser()
{
	memset(&_exportDB, 0, sizeof(EOSRsrcBundleDatabaseExport));
}

EOSRsrcBundleXMLParser::~EOSRsrcBundleXMLParser()
{
	if (_exportDB.textures)
		delete _exportDB.textures;
}

XMLElement* EOSRsrcBundleXMLParser::_buildXMLElementFromName(const xmlChar* name)
{
	XMLElement*				element = NULL;
	EOSRsrcBundleXMLElementType		type;

	type = nameToEOSRsrcBundleXMLElementType(name);

	if (type != EOSRsrcBundleXMLElementTypeIllegal)
	{
		switch (type)
		{
			case EOSRsrcBundleXMLElementTypeBundle: 
				element = new XMLElementBundle;
				break;

			case EOSRsrcBundleXMLElementTypeName: 
				element = new XMLElementName;
				break;

			case EOSRsrcBundleXMLElementTypeID: 
				element = new XMLElementID;
				break;

			case EOSRsrcBundleXMLElementTypeFilename: 
				element = new XMLElementFilename;
				break;

			case EOSRsrcBundleXMLElementTypeTextureName: 
				element = new XMLElementTextureName;
				break;

			case EOSRsrcBundleXMLElementTypeSpriteName: 
				element = new XMLElementSpriteName;
				break;

			case EOSRsrcBundleXMLElementTypeWidth: 
				element = new XMLElementWidth;
				break;

			case EOSRsrcBundleXMLElementTypeHeight: 
				element = new XMLElementHeight;
				break;

			case EOSRsrcBundleXMLElementTypeColorFormat: 
				element = new XMLElementColorFormat;
				break;

			case EOSRsrcBundleXMLElementTypeMinMipMapFilter: 
				element = new XMLElementMinMipMapFilter;
				break;

			case EOSRsrcBundleXMLElementTypeMagMipMapFilter: 
				element = new XMLElementMagMipMapFilter;
				break;

			case EOSRsrcBundleXMLElementTypeWrapS: 
				element = new XMLElementWrapS;
				break;

			case EOSRsrcBundleXMLElementTypeWrapT: 
				element = new XMLElementWrapT;
				break;

			case EOSRsrcBundleXMLElementTypeTexEnvMode: 
				element = new XMLElementTexEnvMode;
				break;

			case EOSRsrcBundleXMLElementTypeTexture: 
				element = new XMLElementTexture;
				break;

			case EOSRsrcBundleXMLElementTypeTextureDB: 
				element = new XMLElementTextureDB;
				break;

			case EOSRsrcBundleXMLElementTypeTextureAtlas: 
				element = new XMLElementTextureAtlas;
				break;

			case EOSRsrcBundleXMLElementTypeTextureAtlasDB: 
				element = new XMLElementTextureAtlasDB;
				break;

			case EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMapping: 
				element = new XMLElementSpriteTextureAtlasMapping;
				break;

			case EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingDB: 
				element = new XMLElementSpriteTextureAtlasMappingDB;
				break;

			case EOSRsrcBundleXMLElementTypeSprite: 
				element = new XMLElementSprite;
				break;

			case EOSRsrcBundleXMLElementTypeSpriteDB: 
				element = new XMLElementSpriteDB;
				break;

			case EOSRsrcBundleXMLElementTypeSpriteAnim: 
				element = new XMLElementSpriteAnim;
				break;

			case EOSRsrcBundleXMLElementTypeSpriteAnimDB: 
				element = new XMLElementSpriteAnimDB;
				break;

			case EOSRsrcBundleXMLElementTypeTextureAtlasNameList: 
				element = new XMLElementTextureAtlasNameList;
				break;

			case EOSRsrcBundleXMLElementTypeSpriteTextureAtlasMappingName: 
				element = new XMLElementSpriteTextureAtlasMappingName;
				break;
		}
	}

	return element;
}

EOSError EOSRsrcBundleXMLParser::_addName(EOSRsrcBundleDatabaseExport& db, Char* name, Uint32& offset)
{
	Char*		newlist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (db.names)
	{
		while (curr < db.namesSize)
		{
			strsize = strlen(&db.names[curr]);

			if (!strcmp(name, &db.names[curr]))
			{
				offset = (Uint32) curr;
				found = true;
				break;
			}

			curr += strsize + 1;
		}
	}

	if (found == false)
	{
		strsize = strlen(name);
	
		newlist = new Char[db.namesSize + strsize + 1];
	
		if (newlist)
		{
			if (db.names)
			{
				memcpy(newlist, db.names, db.namesSize);
				delete db.names;
			}
	
			strcpy(&newlist[db.namesSize], name);
			db.names = newlist;
			offset = db.namesSize;
			db.namesSize += (Uint32) strsize + 1;
		}
		else
			error = EOSErrorNoMemory;
	}

	return error;
}

EOSError EOSRsrcBundleXMLParser::_addData(EOSRsrcBundleDatabaseExport& db, Uint8* data, Uint32 length, Uint32& offset)
{
	Uint8*		newdata;
	EOSError 	error = EOSErrorNone;

	newdata = new Uint8[db.dataSize + length];

	if (newdata)
	{
		if (db.data)
		{
			memcpy(newdata, db.data, db.dataSize);
			delete db.data;
		}

		memcpy(&newdata[db.dataSize], data, length);
		db.data = newdata;
		offset = db.dataSize;
		db.dataSize += length;
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

EOSError EOSRsrcBundleXMLParser::_addDataFromFile(EOSRsrcBundleDatabaseExport& db, Char* name, Uint32& length, Uint32& offset)
{
	EOSError					error = EOSErrorNone;
	File						file;
	FileDescriptor				fd;
	Uint8*						data;

	length = 0;

	fd.setFilename(name);
	fd.setFileAccessType(FileAccessTypeReadOnly);
	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		length = file.length();

		data = new Uint8[length];

		if (data)
		{
			file.readUint8(data, length);

			error = _addData(_exportDB, data, length, offset);

			file.close();

			delete data;
		}
		else
			error = EOSErrorNoMemory;
	}
	else
	{
		fprintf(stderr, "Could not open file %s\n", name);
	}

	return error;
}

EOSError EOSRsrcBundleXMLParser::_addNameList(EOSRsrcBundleDatabaseExport& db, Uint32 num, XMLElementName** namelist, Uint32& offset)
{
	EOSError					error = EOSErrorNone;
	Uint32*						newlist;
	Uint32						i;

	newlist = new Uint32[db.numNameOffsets + num];

	if (newlist)
	{
		if (db.nameOffsets)
		{
			memcpy(newlist, db.nameOffsets, sizeof(Uint32) * db.numNameOffsets);
			delete db.nameOffsets;
		}

		for (i=0;i<num;i++)
		{
			error = _addName(db, namelist[i]->getString(), newlist[db.numNameOffsets + i]);

			if (error != EOSErrorNone)
				break;
		}

		db.nameOffsets = newlist;
		offset = db.numNameOffsets;
		db.numNameOffsets += num;
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

EOSError EOSRsrcBundleXMLParser::buildDB(void)
{
	EOSError					error = EOSErrorNone;
	XMLElementBundle*			bundle;
	XMLElementTextureDB*		texDB;
	XMLElementTextureAtlasDB*	texAtlasDB;
	XMLElementSpriteTextureAtlasMappingDB*			mappingDB;
	XMLElementSpriteDB*			spriteDB;
	XMLElementSpriteAnimDB*		spriteAnimDB;
	Uint32						i;
	Uint32						num;
	XMLElementTexture*			tex;
	EOSRsrcBundleTextureExport*		texExp;
	XMLElementTextureAtlas*		texAtlas;
	EOSRsrcBundleTextureAtlasExport*	texAtlasExp;
	XMLElementSpriteTextureAtlasMapping*			mapping;
	EOSRsrcBundleSpriteTextureAtlasMappingExport*	mappingExp;
	XMLElementSprite*			sprite;
	EOSRsrcBundleSpriteExport*			spriteExp;
	XMLElementSpriteAnim*		spriteAnim;
	EOSRsrcBundleSpriteAnimExport*		spriteAnimExp;

	memset(&_exportDB, 0, sizeof(EOSRsrcBundleDatabaseExport));

	if (getRoot() && getRoot()->getType() == EOSRsrcBundleXMLElementTypeBundle)
	{
		bundle = (XMLElementBundle*) getRoot();

		error = _addName(_exportDB, bundle->getName()->getString(), _exportDB.nameOffset);

		texDB = bundle->getTextureDB();
		texAtlasDB = bundle->getTextureAtlasDB();
		mappingDB = bundle->getSpriteTextureAtlasMappingDB();
		spriteDB = bundle->getSpriteDB();
		spriteAnimDB = bundle->getSpriteAnimDB();

		if (texDB)
		{
			if (error == EOSErrorNone)
			{
				num = texDB->getNumTextures();

				_exportDB.textures = new EOSRsrcBundleTextureExport[num];

				if (_exportDB.textures)
				{
					memset(_exportDB.textures, 0, sizeof(EOSRsrcBundleTextureExport) * num);
					_exportDB.numTextures = num;

					for (i=0;i<num;i++)
					{
						tex = texDB->getTextureAtIndex(i);
						texExp = &_exportDB.textures[i];

						error = _addName(_exportDB, tex->getName()->getString(), texExp->nameOffset);

						if (error == EOSErrorNone)
							error = _addDataFromFile(_exportDB, tex->getFilename()->getString(), texExp->dataSize, texExp->dataOffset);

						if (error == EOSErrorNone)
						{
							Uint32	tstate = 0;

							texExp->width = tex->getWidth()->getValue();
							texExp->height = tex->getHeight()->getValue();
							texExp->colorFormat = tex->getColorFormat()->getColorFormat();

							if (tex->getMinMipMapFilter())
							{
								switch (tex->getMinMipMapFilter()->getMipMapFilter())
								{
									case TextureState::TEXTURE_MIPMAP_NEAREST:
										tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST;
										break;

									case TextureState::TEXTURE_MIPMAP_LINEAR:
										tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR;
										break;

									case TextureState::TEXTURE_MIPMAP_NEAREST_NEAREST:
										tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_NEAREST;
										break;

									case TextureState::TEXTURE_MIPMAP_NEAREST_LINEAR:
										tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_LINEAR;
										break;

									case TextureState::TEXTURE_MIPMAP_LINEAR_NEAREST:
										tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_NEAREST;
										break;

									case TextureState::TEXTURE_MIPMAP_LINEAR_LINEAR:
										tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_LINEAR;
										break;
								}
							}

							if (tex->getMagMipMapFilter())
							{
								switch (tex->getMagMipMapFilter()->getMipMapFilter())
								{
									case TextureState::TEXTURE_MIPMAP_NEAREST:
										tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST;
										break;

									case TextureState::TEXTURE_MIPMAP_LINEAR:
										tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR;
										break;

									case TextureState::TEXTURE_MIPMAP_NEAREST_NEAREST:
										tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_NEAREST;
										break;

									case TextureState::TEXTURE_MIPMAP_NEAREST_LINEAR:
										tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_LINEAR;
										break;

									case TextureState::TEXTURE_MIPMAP_LINEAR_NEAREST:
										tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_NEAREST;
										break;

									case TextureState::TEXTURE_MIPMAP_LINEAR_LINEAR:
										tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_LINEAR;
										break;
								}
							}

							if (tex->getWrapS())
							{
								switch (tex->getWrapS()->getWrap())
								{
									case TextureState::TEXTURE_WRAP_CLAMP:
										tstate |= TEXTURE_STATE_WRAP_S_CLAMP;
										break;

									case TextureState::TEXTURE_WRAP_REPEAT:
										tstate |= TEXTURE_STATE_WRAP_S_REPEAT;
										break;

									case TextureState::TEXTURE_WRAP_CLAMP_BORDER:
										tstate |= TEXTURE_STATE_WRAP_S_CLAMP_BORDER;
										break;

									case TextureState::TEXTURE_WRAP_CLAMP_EDGE:
										tstate |= TEXTURE_STATE_WRAP_S_CLAMP_EDGE;
										break;
								}
							}

							if (tex->getWrapT())
							{
								switch (tex->getWrapT()->getWrap())
								{
									case TextureState::TEXTURE_WRAP_CLAMP:
										tstate |= TEXTURE_STATE_WRAP_T_CLAMP;
										break;

									case TextureState::TEXTURE_WRAP_REPEAT:
										tstate |= TEXTURE_STATE_WRAP_T_REPEAT;
										break;

									case TextureState::TEXTURE_WRAP_CLAMP_BORDER:
										tstate |= TEXTURE_STATE_WRAP_T_CLAMP_BORDER;
										break;

									case TextureState::TEXTURE_WRAP_CLAMP_EDGE:
										tstate |= TEXTURE_STATE_WRAP_T_CLAMP_EDGE;
										break;
								}
							}

							if (tex->getTexEnvMode())
							{
								switch (tex->getTexEnvMode()->getTexEnvMode())
								{
									case TextureState::TEXTURE_ENV_MODE_REPLACE:
										tstate |= TEXTURE_STATE_ENV_REPLACE;
										break;

									case TextureState::TEXTURE_ENV_MODE_MODULATE:
										tstate |= TEXTURE_STATE_ENV_MODULATE;
										break;

									case TextureState::TEXTURE_ENV_MODE_DECAL:
										tstate |= TEXTURE_STATE_ENV_DECAL;
										break;

									case TextureState::TEXTURE_ENV_MODE_BLEND:
										tstate |= TEXTURE_STATE_ENV_BLEND;
										break;

									case TextureState::TEXTURE_ENV_MODE_ADD:
										tstate |= TEXTURE_STATE_ENV_ADD;
										break;

									case TextureState::TEXTURE_ENV_MODE_COMBINE:
										tstate |= TEXTURE_STATE_ENV_COMBINE;
										break;
								}
							}

							texExp->texState = tstate;
						}
					}
				}
				else
					error = EOSErrorNoMemory;
			}
		}

		if (texAtlasDB)
		{
			if (error == EOSErrorNone)
			{
				num = texAtlasDB->getNumTextureAtlases();

				_exportDB.textureAtlases = new EOSRsrcBundleTextureAtlasExport[num];

				if (_exportDB.textureAtlases)
				{
					memset(_exportDB.textureAtlases, 0, sizeof(EOSRsrcBundleTextureAtlasExport) * num);
					_exportDB.numTextureAtlases = num;

					for (i=0;i<num;i++)
					{
						texAtlas = texAtlasDB->getTextureAtlasAtIndex(i);
						texAtlasExp = &_exportDB.textureAtlases[i];

						error = _addName(_exportDB, texAtlas->getName()->getString(), texAtlasExp->nameOffset);

						if (error == EOSErrorNone)
							error = _addDataFromFile(_exportDB, texAtlas->getFilename()->getString(), texAtlasExp->dataSize, texAtlasExp->dataOffset);

						if (error == EOSErrorNone)
						{
							if (texAtlas->getTextureName())
								error = _addName(_exportDB, texAtlas->getTextureName()->getString(), texAtlasExp->texNameOffset);
							else
								texAtlasExp->texNameOffset = 0xFFFFFFFF;
						}
					}
				}
				else
					error = EOSErrorNoMemory;
			}
		}

		if (mappingDB)
		{
			num = mappingDB->getNumSpriteTextureAtlasMappings();

			_exportDB.spriteTextureAtlasMappings = new EOSRsrcBundleSpriteTextureAtlasMappingExport[num];

			if (_exportDB.spriteTextureAtlasMappings)
			{
				memset(_exportDB.spriteTextureAtlasMappings, 0, sizeof(EOSRsrcBundleSpriteExport) * num);
				_exportDB.numSpriteTextureAtlasMappings = num;

				for (i=0;i<num;i++)
				{
					mapping = mappingDB->getSpriteTextureAtlasMappingAtIndex(i);
					mappingExp = &_exportDB.spriteTextureAtlasMappings[i];

					error = _addName(_exportDB, mapping->getName()->getString(), mappingExp->nameOffset);

					if (error == EOSErrorNone)
					{
						error = _addDataFromFile(_exportDB, mapping->getFilename()->getString(), mappingExp->dataSize, mappingExp->dataOffset);
					}
				}
			}
			else
				error = EOSErrorNoMemory;
		}

		if (spriteDB)
		{
			num = spriteDB->getNumSprites();

			_exportDB.sprites = new EOSRsrcBundleSpriteExport[num];

			if (_exportDB.sprites)
			{
				memset(_exportDB.sprites, 0, sizeof(EOSRsrcBundleSpriteExport) * num);
				_exportDB.numSprites = num;

				for (i=0;i<num;i++)
				{
					sprite = spriteDB->getSpriteAtIndex(i);
					spriteExp = &_exportDB.sprites[i];

					error = _addName(_exportDB, sprite->getName()->getString(), spriteExp->nameOffset);

					if (error == EOSErrorNone)
						error = _addDataFromFile(_exportDB, sprite->getFilename()->getString(), spriteExp->dataSize, spriteExp->dataOffset);

					if (error == EOSErrorNone)
					{
						if (sprite->getTextureAtlasNameList())
						{
							spriteExp->numTexAtlasNames = sprite->getTextureAtlasNameList()->getNumNames();
							error = _addNameList(_exportDB, sprite->getTextureAtlasNameList()->getNumNames(), sprite->getTextureAtlasNameList()->getNameList(), spriteExp->texAtlasNameListOffset);
						}
						else
						{
							spriteExp->numTexAtlasNames = 0;
							spriteExp->texAtlasNameListOffset = 0xFFFFFFFF;
						}
					}

					if (error == EOSErrorNone)
					{
						if (sprite->getSpriteTextureAtlasMappingName())
						{
							error = _addName(_exportDB, sprite->getSpriteTextureAtlasMappingName()->getString(), spriteExp->sprTexAtlasNameOffset);
						}
						else
						{
							spriteExp->sprTexAtlasNameOffset = 0xFFFFFFFF;
						}
					}
				}
			}
		}

		if (spriteAnimDB)
		{
			num = spriteAnimDB->getNumSpriteAnims();

			_exportDB.spriteAnims = new  EOSRsrcBundleSpriteAnimExport[num];

			if (_exportDB.spriteAnims)
			{
				memset(_exportDB.spriteAnims, 0, sizeof(EOSRsrcBundleSpriteAnimExport));
				_exportDB.numSpriteAnims = num;

				for (i=0;i<num;i++)
				{
					spriteAnim = spriteAnimDB->getSpriteAnimAtIndex(i);
					spriteAnimExp = &_exportDB.spriteAnims[i];

					error = _addName(_exportDB, sprite->getName()->getString(), spriteAnimExp->nameOffset);

					if (error == EOSErrorNone)
						error = _addDataFromFile(_exportDB, spriteAnim->getFilename()->getString(), spriteAnimExp->dataSize, spriteAnimExp->dataOffset);

					if (error == EOSErrorNone)
					{
						if (spriteAnim->getSpriteName())
						{
							error = _addName(_exportDB, spriteAnim->getSpriteName()->getString(), spriteAnimExp->sprNameOffset);
						}
						else
						{
							spriteAnimExp->sprNameOffset = 0xFFFFFFFF;
						}
					}
				}
			}
			else
				error = EOSErrorNoMemory;
		}
	}

	return error;
}

EOSError EOSRsrcBundleXMLParser::exportDB(Char* filename)
{
	EOSError				error = EOSErrorNone;
	EOSRsrcBundleDatabaseHeader	header;
	File					file;
	FileDescriptor			fd;
	Uint32					offset = 0;
	Uint32					i;
	Uint8*					image;
	Uint32					totalsize = 0;
	fd.setFilename(filename);
	fd.setFileAccessType(FileAccessTypeWriteOnly);

	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		totalsize = sizeof(EOSRsrcBundleDatabaseHeader);
		totalsize += _exportDB.numTextures * sizeof(EOSRsrcBundleTextureExport);
		totalsize += _exportDB.numTextureAtlases * sizeof(EOSRsrcBundleTextureAtlasExport);
		totalsize += _exportDB.numSpriteTextureAtlasMappings * sizeof(EOSRsrcBundleSpriteTextureAtlasMappingExport);
		totalsize += _exportDB.numSprites * sizeof(EOSRsrcBundleSpriteExport);
		totalsize += _exportDB.numSpriteAnims * sizeof(EOSRsrcBundleSpriteAnimExport);
		totalsize += _exportDB.numNameOffsets * sizeof(Uint32);
		totalsize += _exportDB.namesSize;
		totalsize += _exportDB.dataSize;

#ifdef _PLATFORM_MAC
		image = (Uint8*) malloc(sizeof(Uint8) * totalsize);
#else
		image = new Uint8[totalsize];
#endif

		if (image)
		{
			header.endian = 0x01020304;
			header.version = 0;
			header.numTextures = _exportDB.numTextures;
			header.textures = 0;
			header.numTextureAtlases = _exportDB.numTextureAtlases;
			header.textureAtlases = 0;
			header.numSpriteMappings = _exportDB.numSpriteTextureAtlasMappings;
			header.spriteMappings = 0;
			header.numSprites = _exportDB.numSprites;
			header.sprites = 0;
			header.numSpriteAnims = _exportDB.numSpriteAnims;
			header.spriteAnims = 0;
			header.numNameOffsets = _exportDB.numNameOffsets;
			header.names = 0;

			offset += sizeof(EOSRsrcBundleDatabaseHeader);

			//	Copy all of the Textures
			header.textures = offset;

			for (i=0;i<header.numTextures;i++)
			{
				memcpy(&image[offset], &_exportDB.textures[i], sizeof(EOSRsrcBundleTextureExport));
				offset += sizeof(EOSRsrcBundleTextureExport);
			}

			//	Copy all of the Texture Atlases
			header.textureAtlases = offset;

			for (i=0;i<header.numTextureAtlases;i++)
			{
				memcpy(&image[offset], &_exportDB.textureAtlases[i], sizeof(EOSRsrcBundleTextureAtlasExport));
				offset += sizeof(EOSRsrcBundleTextureAtlasExport);
			}

			//	Copy all of the Sprite Mappings
			header.spriteMappings = offset;

			for (i=0;i<header.numSpriteMappings;i++)
			{
				memcpy(&image[offset], &_exportDB.spriteTextureAtlasMappings[i], sizeof(EOSRsrcBundleSpriteTextureAtlasMappingExport));
				offset += sizeof(EOSRsrcBundleSpriteTextureAtlasMappingExport);
			}

			//	Copy all of the Sprites
			header.sprites = offset;

			for (i=0;i<header.numSprites;i++)
			{
				memcpy(&image[offset], &_exportDB.sprites[i], sizeof(EOSRsrcBundleSpriteExport));
				offset += sizeof(EOSRsrcBundleSpriteExport);
			}

			//	Copy all of the Sprite Anims
			header.spriteAnims = offset;

			for (i=0;i<header.numSpriteAnims;i++)
			{
				memcpy(&image[offset], &_exportDB.spriteAnims[i], sizeof(EOSRsrcBundleSpriteAnimExport));
				offset += sizeof(EOSRsrcBundleSpriteAnimExport);
			}

			//	Copy all of the Name Offset
			header.nameOffsets = offset;
			memcpy(&image[offset], _exportDB.nameOffsets, sizeof(Uint32) * header.numNameOffsets);
			offset += sizeof(Uint32) * header.numNameOffsets;

			//	Copy the names
			header.names = offset;
			memcpy(&image[offset], _exportDB.names, _exportDB.namesSize);
			offset += _exportDB.namesSize;

			//	Now blast over our data
			header.data = offset;
			memcpy(&image[offset], _exportDB.data, _exportDB.dataSize);
			offset += _exportDB.dataSize;

			//	Now write the header, which is at the beginning of the file
			memcpy(image, &header, sizeof(EOSRsrcBundleDatabaseHeader));

			file.writeUint8(image, totalsize);

			//	Note that image will be freed by file
#ifndef _PLATFORM_MAC			
			delete image;
#endif /* _PLATFORM_MAC */
		}
		else
			error = EOSErrorNoMemory;

		file.close();
	}

	return error;
}

