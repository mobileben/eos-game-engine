/******************************************************************************
 *
 * File: TextAtlasMappingXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Mapping XML
 * 
 *****************************************************************************/

#ifndef __TEXATLASMAPPINGXML_H__
#define __TEXATLASMAPPINGXML_H__

#include "Platform.h"
#include "Graphics.h"
#include "toolkit/XMLElement.h"
#include "toolkit/XMLParser.h"
#include <libxml/xmlreader.h>

typedef enum
{
	TexAtlasMappingXMLElementTypeTextureAtlasMapping = 0,
	TexAtlasMappingXMLElementTypeName,
	TexAtlasMappingXMLElementTypeMaster,
	TexAtlasMappingXMLElementTypeAtlas,
	TexAtlasMappingXMLElementTypeLast,
	TexAtlasMappingXMLElementTypeIllegal = TexAtlasMappingXMLElementTypeLast,
} TexAtlasMappingXMLElementType;


class XMLElementName : public XMLElementString
{
public:
	XMLElementName() { setType(TexAtlasMappingXMLElementTypeName); }
	~XMLElementName() {}
};

class XMLElementMaster : public XMLElementString
{
public:
	XMLElementMaster() { setType(TexAtlasMappingXMLElementTypeMaster); }
	~XMLElementMaster() {}
};

class XMLElementAtlas : public XMLElementString
{
public:
	XMLElementAtlas() { setType(TexAtlasMappingXMLElementTypeAtlas); }
	~XMLElementAtlas() {}
};

class XMLElementTextureAtlasMapping : public XMLElementNode
{
protected:
	XMLElementName*		_name;
	XMLElementMaster*	_master;

	Uint32				_numAtlases;
	XMLElementAtlas**	_atlases;

public:
	inline XMLElementName*		getName(void) { return _name; }
	inline XMLElementMaster*	getMaster() { return _master; }
	inline Uint32				getNumAtlases(void) { return _numAtlases; }
	XMLElementAtlas*			getAtlasAtIndex(Uint32 index);

	XMLElementTextureAtlasMapping();
	~XMLElementTextureAtlasMapping();

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TexAtlasMappingXMLParser : public XMLParser
{
public:
	TexAtlasMappingXMLParser();
	~TexAtlasMappingXMLParser();

	XMLElement*			_buildXMLElementFromName(const xmlChar* name);

	EOSError			buildDB(void);
	EOSError			exportDB(Char* filename);

	XMLElementName*		getName(void);
	XMLElementMaster*	getMaster(void);
	
	Uint32				getNumAtlases(void);
	XMLElementAtlas*	getAtlasAtIndex(Uint32 index);
};

#endif /* __TEXATLASMAPPINGXML_H__ */
