/******************************************************************************
 *
 * File: XMLParser.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * XML Parsers for parsing XML. This uses libxml2
 * 
 *****************************************************************************/

#ifndef __XMLPARSER_H__
#define __XMLPARSER_H__

#include "toolkit/XMLElement.h"

class XMLParser
{
private:
	static const Sint32			MaxXMLStack = 64;

	xmlTextReaderPtr			_reader;

	XMLElement*					_root;

	XMLElement*					_currElement;
	XMLElement*					_stack[MaxXMLStack];
	Sint32						_stackIndex;

	Uint32						_numXMLElementTypes;
	Char**						_xmlElementTypeNames;

	EOSError					_addElement(const xmlChar* name);
	EOSError					_addValueToCurrentElement(const xmlChar* value);
	EOSError					_endElement(const xmlChar* name);

	EOSError					_parseLine(xmlTextReaderPtr reader, Boolean& done);

protected:
	virtual XMLElement*			_buildXMLElementFromName(const xmlChar* name) { return NULL; }

	EOSError					setXMLElementTypeNamespace(Uint32 num, Char** names);
	void						destroyXMLElementNamespace(void);

public:
	XMLParser();
	~XMLParser();

	EOSError					parse(Char* filename);

	XMLElementType				nameToXMLElementType(const xmlChar* name);

	inline XMLElement*			getRoot(void) { return _root; }

	virtual EOSError			buildDB(void) { return EOSErrorUnsupported; }
	virtual EOSError			exportDB(Char* filename) { return EOSErrorUnsupported; }
};

#endif /* __XMLPARSER_H__ */
