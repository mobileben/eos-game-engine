/******************************************************************************
 *
 * File: TexInfoXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture info XML
 * 
 *****************************************************************************/

#ifndef __TEXINFOXML_H__
#define __TEXINFOXML_H__

#include "Platform.h"
#include "Graphics.h"
#include "Texture.h"
#include "TextureState.h"
#include "toolkit/XMLElement.h"

#include <libxml/xmlreader.h>

typedef struct
{
	Uint32			nameOffset;
	Uint32			numTexInfo;
	TexInfoExport*	texInfo;
	Uint32			namesSize;
	Char*			names;	
} TexInfoDBExport;

typedef enum
{
	TexInfoXMLElementTypeTexInfoDB = 0, 
	TexInfoXMLElementTypeTexInfo, 
	TexInfoXMLElementTypeRefID, 
	TexInfoXMLElementTypeName, 
	TexInfoXMLElementTypeFilename, 
	TexInfoXMLElementTypeWidth, 
	TexInfoXMLElementTypeHeight, 
	TexInfoXMLElementTypeColorFormat, 
	TexInfoXMLElementTypeMinMipMapFilter, 
	TexInfoXMLElementTypeMagMipMapFilter, 
	TexInfoXMLElementTypeWrapS, 
	TexInfoXMLElementTypeWrapT, 
	TexInfoXMLElementTypeTexEnvMode, 
	TexInfoXMLElementTypeLast, 
	TexInfoXMLElementTypeIllegal = TexInfoXMLElementTypeLast, 
} TexInfoXMLElementType;

class XMLElementRefID : public XMLElementSint32
{
public:
	XMLElementRefID() { setType(TexInfoXMLElementTypeRefID); }
	~XMLElementRefID() {}
};

class XMLElementName : public XMLElementString
{
public:
	XMLElementName() { setType(TexInfoXMLElementTypeName); }
	~XMLElementName() {}
};

class XMLElementFilename : public XMLElementString
{
public:
	XMLElementFilename() { setType(TexInfoXMLElementTypeFilename); }
	~XMLElementFilename() {}
};

class XMLElementWidth : public XMLElementSint32
{
public:
	XMLElementWidth() { setType(TexInfoXMLElementTypeWidth); }
	~XMLElementWidth() {}
};

class XMLElementHeight : public XMLElementSint32
{
public:
	XMLElementHeight() { setType(TexInfoXMLElementTypeHeight); }
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
	XMLElementMinMipMapFilter() { setType(TexInfoXMLElementTypeMinMipMapFilter); }
	~XMLElementMinMipMapFilter() {}
};

class XMLElementMagMipMapFilter : public XMLElementMipMapFilter
{
public:
	XMLElementMagMipMapFilter() { setType(TexInfoXMLElementTypeMagMipMapFilter); }
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
	XMLElementWrapS() { setType(TexInfoXMLElementTypeWrapS); }
	~XMLElementWrapS() {}
};

class XMLElementWrapT : public XMLElementWrap
{
public:
	XMLElementWrapT() { setType(TexInfoXMLElementTypeWrapT); }
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

class XMLElementTexInfo : public XMLElementNode
{
protected:
	XMLElementRefID*			_refID;
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
	XMLElementTexInfo();
	~XMLElementTexInfo();

	inline XMLElementRefID*				getRefID(void) { return _refID; }
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

class XMLElementTexInfoDB : public XMLElementNode
{
protected:
	XMLElementName*		_name;

	Uint32				_numTexInfo;
	XMLElementTexInfo**	_texInfo;

public:
	XMLElementTexInfoDB();
	~XMLElementTexInfoDB();

	inline Uint32			getNumTexInfo(void) { return _numTexInfo; }
	inline XMLElementName*	getName(void) { return _name; }
	XMLElementTexInfo*		getTexInfoAtIndex(Uint32 index);

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class TexInfoXMLParser
{
private:
	static const Sint32			MaxXMLStack = 64;

	xmlTextReaderPtr			_reader;

	XMLElement*					_root;

	XMLElement*					_currElement;
	XMLElement*					_stack[MaxXMLStack];
	Sint32						_stackIndex;

	TexInfoDBExport				_exportDB;

	EOSError					_addElement(const xmlChar* name);
	EOSError					_addValueToCurrentElement(const xmlChar* value);
	EOSError					_endElement(const xmlChar* name);

	EOSError					_parseLine(xmlTextReaderPtr reader, Boolean& done);

	EOSError					_addName(TexInfoDBExport& db, Char* name, Uint32& offset);

public:
	TexInfoXMLParser();
	~TexInfoXMLParser();

	EOSError	parse(Char* filename);

	EOSError	buildDB(void);
	EOSError	exportDB(Char* filename);
};

#endif /* __TEXINFOXML_H__ */
