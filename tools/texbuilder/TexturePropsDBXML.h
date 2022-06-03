/******************************************************************************
 *
 * File: TexturePropsDBXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Properties
 * 
 *****************************************************************************/

#ifndef __TEXTUREPROPSDBXML_H__
#define __TEXTUREPROPSDBXML_H__

#include "Platform.h"
#include "Graphics.h"
#include "toolkit/XMLElement.h"
#include <libxml/xmlreader.h>

typedef enum
{
	TexPropsXMLElementTypeTexProperties = 0,
	TexPropsXMLElementTypeSubTex,
	TexPropsXMLElementTypeName,
	TexPropsXMLElementTypeSaturation,
	TexPropsXMLElementTypePadEdgeColor,
	TexPropsXMLElementTypePad,
	TexPropsXMLElementTypeTransparentColor,
	TexPropsXMLElementTypePadColor,
	TexPropsXMLElementTypePadColorTop,
	TexPropsXMLElementTypePadColorBottom,
	TexPropsXMLElementTypePadColorLeft,
	TexPropsXMLElementTypePadColorRight,
	TexPropsXMLElementTypePadAll,
	TexPropsXMLElementTypePadTop,
	TexPropsXMLElementTypePadBottom,
	TexPropsXMLElementTypePadLeft,
	TexPropsXMLElementTypePadRight,
	TexPropsXMLElementTypeHalfTexel,
	TexPropsXMLElementTypePriority,
	TexPropsXMLElementTypeRed,
	TexPropsXMLElementTypeGreen,
	TexPropsXMLElementTypeBlue,
	TexPropsXMLElementTypeAlpha,
	TexPropsXMLElementTypeAlphaDefringe,
	TexPropsXMLElementTypeLast,
	TexPropsXMLElementTypeIllegal = TexPropsXMLElementTypeLast,
} TexPropsXMLElementType;

class XMLElementName : public XMLElementString
{
public:
	XMLElementName() { setType(TexPropsXMLElementTypeName); }
	~XMLElementName() {}
};

class XMLElementSaturation : public XMLElementFloat32
{
public:
	XMLElementSaturation() { setType(TexPropsXMLElementTypeSaturation); }
	~XMLElementSaturation() {}

	EOSError			end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementRed : public XMLElementSint32
{
public:
	XMLElementRed() { setType(TexPropsXMLElementTypeRed); }
	~XMLElementRed() {}
};

class XMLElementGreen : public XMLElementSint32
{
public:
	XMLElementGreen() { setType(TexPropsXMLElementTypeGreen); }
	~XMLElementGreen() {}
};

class XMLElementBlue : public XMLElementSint32
{
public:
	XMLElementBlue() { setType(TexPropsXMLElementTypeBlue); }
	~XMLElementBlue() {}
};

class XMLElementAlpha : public XMLElementSint32
{
public:
	XMLElementAlpha() { setType(TexPropsXMLElementTypeAlpha); }
	~XMLElementAlpha() {}
};

class XMLElementColorIntRGBA : public XMLElementNode
{
protected:
	XMLElementRed* 		_red; 
	XMLElementGreen* 	_green; 
	XMLElementBlue* 	_blue; 
	XMLElementAlpha* 	_alpha; 

public:
	XMLElementColorIntRGBA();
	~XMLElementColorIntRGBA();

	inline XMLElementRed*	getRed(void) { return _red; }
	inline XMLElementGreen*	getGreen(void) { return _green; }
	inline XMLElementBlue*	getBlue(void) { return _blue; }
	inline XMLElementAlpha*	getAlpha(void) { return _alpha; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementTransparentColor : public XMLElementColorIntRGBA
{
public:
	XMLElementTransparentColor() { setType(TexPropsXMLElementTypeTransparentColor); }
	~XMLElementTransparentColor() {}
};

class XMLElementPadColor : public XMLElementColorIntRGBA
{
public:
	XMLElementPadColor() { setType(TexPropsXMLElementTypePadColor); }
	~XMLElementPadColor() {}
};

class XMLElementPadColorTop : public XMLElementColorIntRGBA
{
public:
	XMLElementPadColorTop() { setType(TexPropsXMLElementTypePadColorTop); }
	~XMLElementPadColorTop() {}
};

class XMLElementPadColorBottom : public XMLElementColorIntRGBA
{
public:
	XMLElementPadColorBottom() { setType(TexPropsXMLElementTypePadColorBottom); }
	~XMLElementPadColorBottom() {}
};

class XMLElementPadColorLeft : public XMLElementColorIntRGBA
{
public:
	XMLElementPadColorLeft() { setType(TexPropsXMLElementTypePadColorLeft); }
	~XMLElementPadColorLeft() {}
};

class XMLElementPadColorRight : public XMLElementColorIntRGBA
{
public:
	XMLElementPadColorRight() { setType(TexPropsXMLElementTypePadColorRight); }
	~XMLElementPadColorRight() {}
};

class XMLElementPadAll : public XMLElementSint32
{
public:
	XMLElementPadAll() { setType(TexPropsXMLElementTypePadAll); }
	~XMLElementPadAll() {}
};

class XMLElementPadTop : public XMLElementSint32
{
public:
	XMLElementPadTop() { setType(TexPropsXMLElementTypePadTop); }
	~XMLElementPadTop() {}
};

class XMLElementPadBottom : public XMLElementSint32
{
public:
	XMLElementPadBottom() { setType(TexPropsXMLElementTypePadBottom); }
	~XMLElementPadBottom() {}
};

class XMLElementPadLeft : public XMLElementSint32
{
public:
	XMLElementPadLeft() { setType(TexPropsXMLElementTypePadLeft); }
	~XMLElementPadLeft() {}
};

class XMLElementPadRight : public XMLElementSint32
{
public:
	XMLElementPadRight() { setType(TexPropsXMLElementTypePadRight); }
	~XMLElementPadRight() {}
};

class XMLElementHalfTexel : public XMLElementBoolean
{
public:
	XMLElementHalfTexel() { setType(TexPropsXMLElementTypeHalfTexel); }
	~XMLElementHalfTexel() {}
};

class XMLElementAlphaDefringe : public XMLElementSint32
{
public:
	XMLElementAlphaDefringe() { setType(TexPropsXMLElementTypeAlphaDefringe); }
	~XMLElementAlphaDefringe() {}
};

class XMLElementPriority : public XMLElementBoolean
{
public:
	XMLElementPriority() { setType(TexPropsXMLElementTypePriority); }
	~XMLElementPriority() {}
};

class XMLElementPadEdgeColor : public XMLElementNode
{
protected:
	XMLElementPadColorTop* 		_top; 
	XMLElementPadColorBottom* 	_bottom; 
	XMLElementPadColorLeft* 	_left; 
	XMLElementPadColorRight* 	_right; 

public:
	XMLElementPadEdgeColor();
	~XMLElementPadEdgeColor();

	inline XMLElementPadColorTop*		getPadColorTop(void) { return _top; }
	inline XMLElementPadColorBottom*	getPadColorBottom(void) { return _bottom; }
	inline XMLElementPadColorLeft*		getPadColorLeft(void) { return _left; }
	inline XMLElementPadColorRight*		getPadColorRight(void) { return _right; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementPad : public XMLElementNode
{
protected:
	XMLElementPadTop* 		_top; 
	XMLElementPadBottom* 	_bottom; 
	XMLElementPadLeft* 		_left; 
	XMLElementPadRight* 	_right; 

public:
	XMLElementPad();
	~XMLElementPad();

	inline XMLElementPadTop*		getPadTop(void) { return _top; }
	inline XMLElementPadBottom*		getPadBottom(void) { return _bottom; }
	inline XMLElementPadLeft*		getPadLeft(void) { return _left; }
	inline XMLElementPadRight*		getPadRight(void) { return _right; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementSubTex : public XMLElementNode
{
protected:
	XMLElementName* 			_name; 
	XMLElementSaturation* 		_saturation; 
	XMLElementPad* 				_pad; 
	XMLElementPadAll* 			_padAll; 
	XMLElementHalfTexel* 		_halfTexel; 
	XMLElementTransparentColor* _transparentColor; 
	XMLElementPriority* 		_priority; 
	XMLElementPadEdgeColor* 	_padEdgeColor; 
	XMLElementPadColor* 		_padColor; 
	XMLElementAlphaDefringe*	_alphaDefringe;

public:
	XMLElementSubTex();
	~XMLElementSubTex();

	inline XMLElementName*				getName(void) { return _name; }
	inline XMLElementSaturation*		getSaturation(void) { return _saturation; }
	inline XMLElementPad*				getPad(void) { return _pad; }
	inline XMLElementPadAll*			getPadAll(void) { return _padAll; }
	inline XMLElementHalfTexel*			getHalfTexel(void) { return _halfTexel; }
	inline XMLElementTransparentColor*	getTransparentColor(void) { return _transparentColor; }
	inline XMLElementPriority*			getPriority(void) { return _priority; }
	inline XMLElementPadEdgeColor*		getPadEdgeColor(void) { return _padEdgeColor; }
	inline XMLElementPadColor*			getPadColor(void) { return _padColor; }
	inline XMLElementAlphaDefringe*		getAlphaDefringe(void) { return _alphaDefringe; }

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementTexProperties : public XMLElementNode
{
protected:
	Uint32						_numSubTex;
	XMLElementSubTex** 			_subTex; 

public:
	XMLElementTexProperties();
	~XMLElementTexProperties();

	inline Uint32		getNumSubTex(void) { return _numSubTex; }
	XMLElementSubTex*	getSubTexAtIndex(Uint32 index);

	EOSError		addChild(XMLElement* child);
	EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};


class TexPropsXMLParser
{
private:
	static const Sint32			MaxXMLStack = 64;

	xmlTextReaderPtr			_reader;

	XMLElement*					_root;

	XMLElement*					_currElement;
	XMLElement*					_stack[MaxXMLStack];
	Sint32						_stackIndex;

	EOSError					_addElement(const xmlChar* name);
	EOSError					_addValueToCurrentElement(const xmlChar* value);
	EOSError					_endElement(const xmlChar* name);

	EOSError					_parseLine(xmlTextReaderPtr reader, Boolean& done);

public:
	TexPropsXMLParser();
	~TexPropsXMLParser();

	EOSError	parse(Char* filename);

	Uint32				getNumSubTex(void);
	XMLElementSubTex*	getSubTexAtIndex(Uint32 index);
};

#endif /* __TEXTUREPROPSDBXML_H__ */
