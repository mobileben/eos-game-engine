/******************************************************************************
 *
 * File: FontSetXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * 
 *****************************************************************************/

#ifndef __FONTSETXML_H__
#define __FONTSETXML_H__

#include "toolkit/XMLParser.h"
#include "toolkit/TextureAtlasXML.h"
#include "Fonts.h"

typedef struct
{
	Uint32	valid;
	Uint32	nameOffset;
	Boolean	leadingSet;
	Float32	leading;
	Boolean	trailingSet;
	Float32	trailing;
	Float32	dy;
	Point2D	vertices[4];
	Point2D uvs[4];
} FontSymbolExport;

typedef struct
{
	Uint32				nameOffset;
	Uint32				texNameOffset;
	Uint32				min;
	Uint32				max;
	Sint32				height;
	Uint32				nextLine;
	Uint32				spaceWidth;
	Uint32				numSymbols;
	Float32				scaleX;
	Float32				scaleY;
	FontSymbolExport*	symbols; 
	Uint32				namesSize;
	Char*				names;
} FontSetScriptDBExport;

typedef struct
{
	Uint32				nameOffset;
	Uint32				numFontScripts;
	Uint32				namesSize;
	Char*				names;
} FontSetDBExport;

class FontScriptXMLElementRange;

class FontScriptXMLParser : public XMLParser
{
public:
	typedef enum
	{
		FontScriptXMLElementTypeFontScript = 0,
		FontScriptXMLElementTypeRange,
		FontScriptXMLElementTypeSymbols,
		FontScriptXMLElementTypeSymbol,
		FontScriptXMLElementTypeName,
		FontScriptXMLElementTypeMin,
		FontScriptXMLElementTypeMax,
		FontScriptXMLElementTypeHeight,
		FontScriptXMLElementTypeNextLine,
		FontScriptXMLElementTypeSpaceWidth,
		FontScriptXMLElementTypeLeading,
		FontScriptXMLElementTypeTrailing,
		FontScriptXMLElementTypeScaleX,
		FontScriptXMLElementTypeScaleY,
		FontScriptXMLElementTypeDY,
		FontScriptXMLElementTypeLast,
		FontScriptXMLElementTypeIllegal = FontScriptXMLElementTypeLast,
	} FontScriptXMLElementType;

	static Char*		_xmlElementTypeNamespace[FontScriptXMLElementTypeLast];

	EOSError			_addName(FontSetScriptDBExport& db, Char* name, Uint32& offset);

protected:
	XMLElement*			_buildXMLElementFromName(const xmlChar* name);

	FontSetScriptDBExport			_exportDB;	

public:
	FontScriptXMLParser();
	~FontScriptXMLParser();

	inline Uint32				getNumSymbols(void) { return _exportDB.numSymbols; }
	inline FontSetScriptDBExport*	getFontSetScriptDBExport(void) { return &_exportDB; }
	FontSymbolExport*			findFontSymbolExport(Char* name);
	FontSymbolExport*			getFontSymbolExportAtIndex(Uint32 index);

	FontScriptXMLElementRange*	getFontScriptRange(void);
	void						setFontScriptRange(FontScriptXMLElementRange* range);

	void						setTexName(Char* name);

	EOSError	buildDB(void);
	EOSError	exportDB(Char* filename);
};

class FontScriptXMLElementName : public XMLElementString
{
public:
	FontScriptXMLElementName() { setType(FontScriptXMLParser::FontScriptXMLElementTypeName); }
	~FontScriptXMLElementName() {}
};

class FontScriptXMLElementMin : public XMLElementHex
{
public:
	FontScriptXMLElementMin() { setType(FontScriptXMLParser::FontScriptXMLElementTypeMin); }
	~FontScriptXMLElementMin() {}

	inline void setValue(Uint32 value) { _value = value; }
};

class FontScriptXMLElementMax : public XMLElementHex
{
public:
	FontScriptXMLElementMax() { setType(FontScriptXMLParser::FontScriptXMLElementTypeMax); }
	~FontScriptXMLElementMax() {}

	inline void setValue(Uint32 value) { _value = value; }
};

class FontScriptXMLElementHeight : public XMLElementSint32
{
public:
	FontScriptXMLElementHeight() { setType(FontScriptXMLParser::FontScriptXMLElementTypeHeight); }
	~FontScriptXMLElementHeight() {}
};

class FontScriptXMLElementNextLine : public XMLElementSint32
{
public:
	FontScriptXMLElementNextLine() { setType(FontScriptXMLParser::FontScriptXMLElementTypeNextLine); }
	~FontScriptXMLElementNextLine() {}
};

class FontScriptXMLElementSpaceWidth : public XMLElementSint32
{
public:
	FontScriptXMLElementSpaceWidth() { setType(FontScriptXMLParser::FontScriptXMLElementTypeSpaceWidth); }
	~FontScriptXMLElementSpaceWidth() {}
};

class FontScriptXMLElementLeading : public XMLElementFloat32
{
public:
	FontScriptXMLElementLeading() { setType(FontScriptXMLParser::FontScriptXMLElementTypeLeading); }
	~FontScriptXMLElementLeading() {}
};

class FontScriptXMLElementTrailing : public XMLElementFloat32
{
public:
	FontScriptXMLElementTrailing() { setType(FontScriptXMLParser::FontScriptXMLElementTypeTrailing); }
	~FontScriptXMLElementTrailing() {}
};

class FontScriptXMLElementScaleX : public XMLElementFloat32
{
public:
	FontScriptXMLElementScaleX() { setType(FontScriptXMLParser::FontScriptXMLElementTypeScaleX); }
	~FontScriptXMLElementScaleX() {}
};

class FontScriptXMLElementScaleY : public XMLElementFloat32
{
public:
	FontScriptXMLElementScaleY() { setType(FontScriptXMLParser::FontScriptXMLElementTypeScaleY); }
	~FontScriptXMLElementScaleY() {}
};

class FontScriptXMLElementDY : public XMLElementFloat32
{
public:
	FontScriptXMLElementDY() { setType(FontScriptXMLParser::FontScriptXMLElementTypeDY); }
	~FontScriptXMLElementDY() {}
};

class FontScriptXMLElementSymbol : public XMLElementNode
{
protected:
	FontScriptXMLElementName*		_name;
	FontScriptXMLElementLeading*	_leading;
	FontScriptXMLElementTrailing*	_trailing;
	FontScriptXMLElementDY*			_dy;

public:
	FontScriptXMLElementSymbol();
	~FontScriptXMLElementSymbol();

	inline FontScriptXMLElementName* 		getName(void) { return _name; }
	inline FontScriptXMLElementLeading* 	getLeading(void) { return _leading; }
	inline FontScriptXMLElementTrailing* 	getTrailing(void) { return _trailing; }
	inline FontScriptXMLElementDY*	 		getDY(void) { return _dy; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class FontScriptXMLElementRange : public XMLElementNode
{
protected:
	FontScriptXMLElementMin*	_min;
	FontScriptXMLElementMax*	_max;

public:
	FontScriptXMLElementRange();
	~FontScriptXMLElementRange();

	inline FontScriptXMLElementMin* 	getMin(void) { return _min; }
	inline FontScriptXMLElementMax* 	getMax(void) { return _max; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class FontScriptXMLElementSymbols : public XMLElementNode
{
public:
	Uint32								_numSymbols;
	FontScriptXMLElementSymbol**		_symbols;

public:
	FontScriptXMLElementSymbols();
	~FontScriptXMLElementSymbols();

	inline Uint32 						getNumSymbols(void) { return _numSymbols; }
	FontScriptXMLElementSymbol* 		getSymbolAtIndex(Uint32 index);

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class FontScriptXMLElementFontScript : public XMLElementNode
{
public:
	FontScriptXMLElementName*		_name;
	FontScriptXMLElementRange*		_range;
	FontScriptXMLElementHeight*		_height;
	FontScriptXMLElementNextLine*	_nextLine;
	FontScriptXMLElementSpaceWidth*	_spaceWidth;
	FontScriptXMLElementScaleX*		_scaleX;
	FontScriptXMLElementScaleY*		_scaleY;
	FontScriptXMLElementSymbols*	_symbols;

public:
	FontScriptXMLElementFontScript();
	~FontScriptXMLElementFontScript();

	inline FontScriptXMLElementName* 		getName(void) { return _name; }
	inline FontScriptXMLElementRange* 		getRange(void) { return _range; }
	inline void								setRange(FontScriptXMLElementRange* range) { _range = range; }
	inline FontScriptXMLElementHeight* 		getHeight(void) { return _height; }
	inline FontScriptXMLElementScaleX* 		getScaleX(void) { return _scaleX; }
	inline FontScriptXMLElementScaleY*	 	getScaleY(void) { return _scaleY; }
	inline FontScriptXMLElementNextLine* 	getNextLine(void) { return _nextLine; }
	inline FontScriptXMLElementSpaceWidth* 	getSpaceWidth(void) { return _spaceWidth; }
	inline FontScriptXMLElementSymbols* 	getSymbols(void) { return _symbols; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class FontSetXMLParser : public XMLParser
{
public:
	typedef enum
	{
		FontSetXMLElementTypeFontSet = 0,
		FontSetXMLElementTypeFontScripts,
		FontSetXMLElementTypeFontScript,
		FontSetXMLElementTypeName,
		FontSetXMLElementTypeTextureAtlas,
		FontSetXMLElementTypeTemplateName,
		FontSetXMLElementTypeScriptName,
		FontSetXMLElementTypeLast,
		FontSetXMLElementTypeIllegal = FontSetXMLElementTypeLast,
 	} FontSetXMLElementType;

	static Char*		_xmlElementTypeNamespace[FontSetXMLElementTypeLast];

protected:
	Uint32					_numFontScripts;
	FontScriptXMLParser*	_fontTemplateScripts;
	FontScriptXMLParser*	_fontScripts;
	FontScriptXMLParser**	_orderedFontScripts;

	TextureAtlasXMLParser*	_textureAtlases;

	XMLElement*				_buildXMLElementFromName(const xmlChar* name);
	EOSError				_addName(FontSetDBExport& db, Char* name, Uint32& offset);
	EOSError				_addName(Char** namelist, Uint32& namesSize, Char* name, Uint32& offset);

	Uint32					_getNumValidSymbols(FontScriptXMLParser* parser);

	FontSetDBExport			_exportDB;

public:
	FontSetXMLParser();
	~FontSetXMLParser();

	EOSError			buildDB(void);
	EOSError			exportDB(Char* filename);
};

class FontSetXMLElementName : public XMLElementString
{
public:
	FontSetXMLElementName() { setType(FontSetXMLParser::FontSetXMLElementTypeName); }
	~FontSetXMLElementName() {}
};

class FontSetXMLElementTextureAtlas : public XMLElementString
{
public:
	FontSetXMLElementTextureAtlas() { setType(FontSetXMLParser::FontSetXMLElementTypeTextureAtlas); }
	~FontSetXMLElementTextureAtlas() {}
};

class FontSetXMLElementTemplateName : public XMLElementString
{
public:
	FontSetXMLElementTemplateName() { setType(FontSetXMLParser::FontSetXMLElementTypeTemplateName); }
	~FontSetXMLElementTemplateName() {}
};

class FontSetXMLElementScriptName : public XMLElementString
{
public:
	FontSetXMLElementScriptName() { setType(FontSetXMLParser::FontSetXMLElementTypeScriptName); }
	~FontSetXMLElementScriptName() {}
};

class FontSetXMLElementFontScript : public XMLElementNode
{
protected:
	FontSetXMLElementTextureAtlas*		_textureAtlas;
	FontSetXMLElementTemplateName*		_templateName;
	FontSetXMLElementScriptName*		_scriptName;

public:
	FontSetXMLElementFontScript();
	~FontSetXMLElementFontScript();

	inline FontSetXMLElementTextureAtlas* 		getTextureAtlas(void) { return _textureAtlas; }
	inline FontSetXMLElementTemplateName* 		getTemplateName(void) { return _templateName; }
	inline FontSetXMLElementScriptName* 		getScriptName(void) { return _scriptName; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation

};

class FontSetXMLElementFontScripts : public XMLElementNode
{
protected:
	Uint32								_numScripts;
	FontSetXMLElementFontScript**		_scripts;

public:
	FontSetXMLElementFontScripts();
	~FontSetXMLElementFontScripts();

	inline Uint32 						getNumFontScripts(void) { return _numScripts; }
	FontSetXMLElementFontScript* 		getFontScriptAtIndex(Uint32 index);

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class FontSetXMLElementFontSet : public XMLElementNode
{
protected:
	FontSetXMLElementName*			_name;
	FontSetXMLElementFontScripts*	_scripts;

public:
	FontSetXMLElementFontSet();
	~FontSetXMLElementFontSet();

	inline FontSetXMLElementName*			getName(void) { return _name; }
	inline FontSetXMLElementFontScripts*	getFontScripts(void) { return _scripts; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

#endif /* __FONTSETXML_H__ */

