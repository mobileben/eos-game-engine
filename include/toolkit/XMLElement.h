/******************************************************************************
 *
 * File: XMLElement.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * XML Elements for parsing XML. This uses libxml2
 * 
 *****************************************************************************/

#ifndef __XMLELEMENT_H__
#define __XMLELEMENT_H__

#include "Platform.h"
#include "EOSError.h"
#include <libxml/xmlreader.h>

typedef Uint32 XMLElementType;

static const Uint32	XMLElementTypeIllegal = 0xFFFFFFFF;

class XMLElement
{
protected:
	XMLElementType		_type;
	Char*				_elementName;
	Boolean				_isLeafType;

	inline void setType(XMLElementType type) { _type = type; }
	inline void setAsLeafType(void) { _isLeafType = true; }

public:
	XMLElement();
	~XMLElement();

	inline XMLElementType 	getType(void) { return _type; }

	inline Boolean			isLeafType(void) { return _isLeafType; }

	void					setElementName(const xmlChar* name);
	Char*					getElementName(void);

	XMLElementType			nameToXMLElementType(const xmlChar* name);

	virtual EOSError		end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementNode : public XMLElement
{
protected:
	Uint32					_numChildren;
	XMLElement**			_children;

public:
	XMLElementNode();
	~XMLElementNode();

	inline Uint32			getNumChildren(void) { return _numChildren; }
	XMLElement*				getChildAtIndex(Uint32 index);

	virtual EOSError		addChild(XMLElement* child);
};

class XMLElementLeaf : public XMLElement
{
public:
	XMLElementLeaf();
	~XMLElementLeaf();

	virtual EOSError	setValue(const xmlChar* value);
};

class XMLElementString : public XMLElementLeaf
{
public:
	typedef enum
	{
		StringCaseAny = 0,
		StringCaseUpper,
		StringCaseLower
	} StringCase;

protected:
	Char*		_string;
	StringCase	_case;

public:
	XMLElementString();
	~XMLElementString();

	inline Char* 		getString(void) { return _string; }
	inline void			setStringCase(StringCase c) { _case = c; }
	inline StringCase	getStringCase(void) { return _case; }

	EOSError			setValue(const xmlChar* value);
	EOSError			end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementSint32 : public XMLElementLeaf
{
protected:
	Sint32		_value;

public:
	XMLElementSint32();
	~XMLElementSint32();

	inline Sint32 	getValue(void) { return _value; }
	EOSError		setValue(const xmlChar* value);
};

class XMLElementFloat32 : public XMLElementLeaf
{
protected:
	Float32		_value;

public:
	XMLElementFloat32();
	~XMLElementFloat32();

	inline Float32 	getValue(void) { return _value; }
	EOSError		setValue(const xmlChar* value);
};

class XMLElementHex : public XMLElementLeaf
{
protected:
	Uint32		_value;

public:
	XMLElementHex();
	~XMLElementHex();

	inline Uint32 	getValue(void) { return _value; }
	EOSError		setValue(const xmlChar* value);
};


class XMLElementBoolean : public XMLElementLeaf
{
protected:
	Boolean			_value;

public:
	XMLElementBoolean();
	~XMLElementBoolean();

	inline Boolean 	getValue(void) { return _value; }
	EOSError		setValue(const xmlChar* value);
};


#endif /* __XMLELEMENT_H__ */
