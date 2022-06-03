/******************************************************************************
 *
 * File: TextureAtlasXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Object
 * 
 *****************************************************************************/

#ifndef __TEXTUREATLASXML_H__
#define __TEXTUREATLASXML_H__

#include "toolkit/XMLElement.h"
#include "toolkit/XMLParser.h"
#include "Graphics.h"

typedef struct
{
	Uint32	refID;
	Uint32	nameOffset;
	Point2D	vertices[4];
	Point2D uvs[4];
} SubTextureExport;

typedef struct
{
	Uint32				nameOffset;
	Uint32				width;
	Uint32				height;
	Uint32				numSubTex;
	SubTextureExport*	subTex;
	Uint32				namesSize;
	Char*				names;
} TextureAtlasDBExport;

class TextureAtlasXMLElementName;
class TextureAtlasXMLElementDimensions;
class TextureAtlasXMLElementSubTex;

class TextureAtlasXMLParser : public XMLParser
{
public:
	typedef enum
	{
		TextureAtlasXMLElementTypeTextureAtlas = 0,
		TextureAtlasXMLElementTypeDimensions,
		TextureAtlasXMLElementTypeSubTexture,
		TextureAtlasXMLElementTypeLocation,
		TextureAtlasXMLElementTypeVertices,
		TextureAtlasXMLElementTypeUVs,
		TextureAtlasXMLElementTypeXY0,
		TextureAtlasXMLElementTypeXY1,
		TextureAtlasXMLElementTypeXY2,
		TextureAtlasXMLElementTypeXY3,
		TextureAtlasXMLElementTypeUV0,
		TextureAtlasXMLElementTypeUV1,
		TextureAtlasXMLElementTypeUV2,
		TextureAtlasXMLElementTypeUV3,
		TextureAtlasXMLElementTypeName,
		TextureAtlasXMLElementTypeRefID,
		TextureAtlasXMLElementTypeX,
		TextureAtlasXMLElementTypeY,
		TextureAtlasXMLElementTypeWidth,
		TextureAtlasXMLElementTypeHeight,
		TextureAtlasXMLElementTypeU,
		TextureAtlasXMLElementTypeV,
		TextureAtlasXMLElementTypeLast,
		TextureAtlasXMLElementTypeIllegal = TextureAtlasXMLElementTypeLast,
 	} TextureAtlasXMLElementType;

	static Char*			_xmlElementTypeNamespace[TextureAtlasXMLElementTypeLast];

protected:
	XMLElement*				_buildXMLElementFromName(const xmlChar* name);

	TextureAtlasDBExport	_exportDB;

	EOSError				_addName(TextureAtlasDBExport& db, Char* name, Uint32& offset);

public:
	TextureAtlasXMLParser();
	~TextureAtlasXMLParser();

	inline TextureAtlasDBExport*	getTextureAtlasDBExport(void) { return &_exportDB; }
	SubTextureExport*				findSubTextureExport(Char* name);

	EOSError			buildDB(void);
	EOSError			exportDB(Char* filename);

	TextureAtlasXMLElementName* 		getName(void);
	TextureAtlasXMLElementDimensions* 	getDimensions(void);
	Uint32 								getNumSubTex(void);
	TextureAtlasXMLElementSubTex* 		getSubTexAtIndex(Uint32 index);
};

class TextureAtlasXMLElementName : public XMLElementString
{
public:
	TextureAtlasXMLElementName() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeName); }
	~TextureAtlasXMLElementName() {}
};

class TextureAtlasXMLElementRefID : public XMLElementSint32
{
public:
	TextureAtlasXMLElementRefID() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeRefID); }
	~TextureAtlasXMLElementRefID() {}
};

class TextureAtlasXMLElementX : public XMLElementFloat32
{
public:
	TextureAtlasXMLElementX() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeX); }
	~TextureAtlasXMLElementX() {}
};

class TextureAtlasXMLElementY : public XMLElementFloat32
{
public:
	TextureAtlasXMLElementY() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeY); }
	~TextureAtlasXMLElementY() {}
};

class TextureAtlasXMLElementWidth : public XMLElementSint32
{
public:
	TextureAtlasXMLElementWidth() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeWidth); }
	~TextureAtlasXMLElementWidth() {}
};

class TextureAtlasXMLElementHeight : public XMLElementSint32
{
public:
	TextureAtlasXMLElementHeight() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeHeight); }
	~TextureAtlasXMLElementHeight() {}
};

class TextureAtlasXMLElementU : public XMLElementFloat32
{
public:
	TextureAtlasXMLElementU() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeU); }
	~TextureAtlasXMLElementU() {}
};

class TextureAtlasXMLElementV : public XMLElementFloat32
{
public:
	TextureAtlasXMLElementV() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeV); }
	~TextureAtlasXMLElementV() {}
};

class TextureAtlasXMLElementXY : public XMLElementNode
{
protected:
	TextureAtlasXMLElementX*			_x;
	TextureAtlasXMLElementY*			_y;

public:
	TextureAtlasXMLElementXY();
	~TextureAtlasXMLElementXY();

	inline TextureAtlasXMLElementX* getX(void) { return _x; }
	inline TextureAtlasXMLElementY* getY(void) { return _y; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TextureAtlasXMLElementXY0 : public TextureAtlasXMLElementXY
{
public:
	TextureAtlasXMLElementXY0() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY0); }
	~TextureAtlasXMLElementXY0();
};

class TextureAtlasXMLElementXY1 : public TextureAtlasXMLElementXY
{
public:
	TextureAtlasXMLElementXY1() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY1); }
	~TextureAtlasXMLElementXY1();
};

class TextureAtlasXMLElementXY2 : public TextureAtlasXMLElementXY
{
public:
	TextureAtlasXMLElementXY2() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY2); }
	~TextureAtlasXMLElementXY2();
};

class TextureAtlasXMLElementXY3 : public TextureAtlasXMLElementXY
{
public:
	TextureAtlasXMLElementXY3() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY3); }
	~TextureAtlasXMLElementXY3();
};

class TextureAtlasXMLElementVertices : public XMLElementNode
{
protected:
	Uint32								_numVertices;
	TextureAtlasXMLElementXY**			_vertices;

public:
	TextureAtlasXMLElementVertices();
	~TextureAtlasXMLElementVertices();

	inline Uint32 					getNumVertices(void) { return _numVertices; }
	TextureAtlasXMLElementXY* 		getVertexAtIndex(Uint32 index);

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TextureAtlasXMLElementUV : public XMLElementNode
{
protected:
	TextureAtlasXMLElementU*			_u;
	TextureAtlasXMLElementV*			_v;

public:
	TextureAtlasXMLElementUV();
	~TextureAtlasXMLElementUV();

	inline TextureAtlasXMLElementU* getU(void) { return _u; }
	inline TextureAtlasXMLElementV* getV(void) { return _v; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TextureAtlasXMLElementUV0 : public TextureAtlasXMLElementUV
{
public:
	TextureAtlasXMLElementUV0() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV0); }
	~TextureAtlasXMLElementUV0();
};

class TextureAtlasXMLElementUV1 : public TextureAtlasXMLElementUV
{
public:
	TextureAtlasXMLElementUV1() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV1); }
	~TextureAtlasXMLElementUV1();
};

class TextureAtlasXMLElementUV2 : public TextureAtlasXMLElementUV
{
public:
	TextureAtlasXMLElementUV2() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV2); }
	~TextureAtlasXMLElementUV2();
};

class TextureAtlasXMLElementUV3 : public TextureAtlasXMLElementUV
{
public:
	TextureAtlasXMLElementUV3() { setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV3); }
	~TextureAtlasXMLElementUV3();
};

class TextureAtlasXMLElementUVs : public XMLElementNode
{
protected:
	Uint32								_numUVs;
	TextureAtlasXMLElementUV**			_uvs;

public:
	TextureAtlasXMLElementUVs();
	~TextureAtlasXMLElementUVs();

	inline Uint32 					getNumUVs(void) { return _numUVs; }
	TextureAtlasXMLElementUV* 		getUVAtIndex(Uint32 index);

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TextureAtlasXMLElementLocation : public XMLElementNode
{
protected:
	TextureAtlasXMLElementX*		_x;
	TextureAtlasXMLElementY*		_y;
	TextureAtlasXMLElementWidth*	_width;
	TextureAtlasXMLElementHeight*	_height;

public:
	TextureAtlasXMLElementLocation();
	~TextureAtlasXMLElementLocation();

	inline TextureAtlasXMLElementX* 		getX(void) { return _x; }
	inline TextureAtlasXMLElementY* 		getY(void) { return _y; }
	inline TextureAtlasXMLElementWidth* 	getWidth(void) { return _width; }
	inline TextureAtlasXMLElementHeight* 	getHeight(void) { return _height; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TextureAtlasXMLElementSubTex : public XMLElementNode
{
protected:
	TextureAtlasXMLElementRefID*	_refID;
	TextureAtlasXMLElementName*		_name;
	TextureAtlasXMLElementLocation*	_location;
	TextureAtlasXMLElementVertices*	_vertices;
	TextureAtlasXMLElementUVs*		_uvs;

public:
	TextureAtlasXMLElementSubTex();
	~TextureAtlasXMLElementSubTex();

	inline TextureAtlasXMLElementRefID* 	getRefID(void) { return _refID; }
	inline TextureAtlasXMLElementName* 		getName(void) { return _name; }
	inline TextureAtlasXMLElementLocation* 	getLocation(void) { return _location; }
	inline TextureAtlasXMLElementVertices* 	getVertices(void) { return _vertices; }
	inline TextureAtlasXMLElementUVs* 		getUVs(void) { return _uvs; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TextureAtlasXMLElementDimensions : public XMLElementNode
{
protected:
	TextureAtlasXMLElementX*		_x;
	TextureAtlasXMLElementY*		_y;
	TextureAtlasXMLElementWidth*	_width;
	TextureAtlasXMLElementHeight*	_height;

public:
	TextureAtlasXMLElementDimensions();
	~TextureAtlasXMLElementDimensions();

	inline TextureAtlasXMLElementX* 		getX(void) { return _x; }
	inline TextureAtlasXMLElementY* 		getY(void) { return _y; }
	inline TextureAtlasXMLElementWidth* 	getWidth(void) { return _width; }
	inline TextureAtlasXMLElementHeight* 	getHeight(void) { return _height; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class TextureAtlasXMLElementTextureAtlas : public XMLElementNode
{
protected:
	TextureAtlasXMLElementName*			_name;
	TextureAtlasXMLElementDimensions*	_dimensions;

	Uint32								_numSubTex;
	TextureAtlasXMLElementSubTex**		_subTex;

public:
	TextureAtlasXMLElementTextureAtlas();
	~TextureAtlasXMLElementTextureAtlas();

	inline TextureAtlasXMLElementName* 			getName(void) { return _name; }
	inline TextureAtlasXMLElementDimensions* 	getDimensions(void) { return _dimensions; }
	inline Uint32 								getNumSubTex(void) { return _numSubTex; }
	TextureAtlasXMLElementSubTex* 				getSubTexAtIndex(Uint32 index);

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

#endif /* __TEXTVREATLASXML_H__ */
