/******************************************************************************
 *
 * File: XMLParser.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * XML Parsers for parsing XML. This uses libxml2
 * 
 *****************************************************************************/

#include "toolkit/XMLParser.h"

XMLParser::XMLParser() : _stackIndex(0), _root(NULL), _currElement(NULL), _numXMLElementTypes(0), _xmlElementTypeNames(NULL)
{
	Uint32	i;

	for (i=0;i<MaxXMLStack;i++)
		_stack[i] = NULL;
}

XMLParser::~XMLParser()
{
}

EOSError XMLParser::setXMLElementTypeNamespace(Uint32 num, Char** names)
{
	EOSError 	error = EOSErrorNone;
	Uint32		i;

	destroyXMLElementNamespace();

	if (_xmlElementTypeNames == NULL)
	{
		_xmlElementTypeNames = new Char*[num];

		if (_xmlElementTypeNames)
		{
			memset(_xmlElementTypeNames, 0, sizeof(Char*) * num);

			_numXMLElementTypes = num;

			for (i=0;i<num;i++)
			{
				_xmlElementTypeNames[i] = new Char[strlen(names[i]) + 1];

				if (_xmlElementTypeNames[i])
					strcpy(_xmlElementTypeNames[i], names[i]);
				else
				{
					error = EOSErrorNoMemory;
					break;
				}
			}
		}
		else
			error = EOSErrorNoMemory;
	}

	if (error != EOSErrorNone)
		destroyXMLElementNamespace();

	return error;
}

void XMLParser::destroyXMLElementNamespace(void)
{
	Uint32	i;

	for (i=0;i<_numXMLElementTypes;i++)
	{
		if (_xmlElementTypeNames[i])
			delete _xmlElementTypeNames[i];
	}

	if (_xmlElementTypeNames)
		delete _xmlElementTypeNames;

	_xmlElementTypeNames = NULL;
	_numXMLElementTypes = 0;
}

EOSError XMLParser::_addElement(const xmlChar* name)
{
	EOSError			error = EOSErrorNone;
	XMLElement*			element = NULL;
	XMLElementNode*		current = NULL;

	element = _buildXMLElementFromName(name);

	if (element)
	{
		element->setElementName(name);

		//	If root is NULL, this is the root NULL
		if (_root == NULL)
		{
			_root = _currElement = element;

			_stack[_stackIndex++] = element;

			if (_stackIndex >= MaxXMLStack)
			{
				error = EOSErrorOutOfBounds;
				fprintf(stderr, "MaxXMLStack out of bounds\n");
			}
		}
		else
		{
			if (_currElement && _currElement->isLeafType() == false)
			{
				current = (XMLElementNode*) _currElement;
				current->addChild(element);
			}

			//	Add it to our stack
			_stack[_stackIndex++] = element;
			_currElement = element;

			if (_stackIndex >= MaxXMLStack)
			{
				error = EOSErrorOutOfBounds;
				fprintf(stderr, "MaxXMLStack out of bounds\n");
			}
		}
	}
	else
	{
		fprintf(stderr, "Could not find ELEMENT %s\n", name);
		error = EOSErrorIllegalGrammar;
	}

	return error;
}

EOSError XMLParser::_addValueToCurrentElement(const xmlChar* value)
{
	EOSError					error = EOSErrorNone;
	XMLElementLeaf*	current;

	if (_currElement && _currElement->isLeafType())
	{
		current = (XMLElementLeaf*) _currElement;
		current->setValue(value);
	}
	else
		error = EOSErrorIllegalGrammar;

	return error;
}

EOSError XMLParser::_endElement(const xmlChar* name)
{
	EOSError					error = EOSErrorNone;

	if (_currElement)
	{
		error = _currElement->end(name);

		if (error == EOSErrorNone)
		{
			_stackIndex--;

			if (_stackIndex >= 0)
			{
				_stack[_stackIndex] = NULL;

				if (_stackIndex == 0)
					_currElement = NULL;
				else
					_currElement = _stack[_stackIndex - 1];
			}
			else
			{
				fprintf(stderr, "XMLStack underrun\n");
				error = EOSErrorOutOfBounds;
			}
		}
	}
	else
		error = EOSErrorIllegalGrammar;

	return error;
}

EOSError XMLParser::_parseLine(xmlTextReaderPtr reader, Boolean& done)
{
	EOSError		error = EOSErrorNone;
	Sint32	 		ret = xmlTextReaderRead(reader);
	xmlReaderTypes	type;

	if (ret == 1)
	{
		//	Parse the node
		type = (xmlReaderTypes) xmlTextReaderNodeType(reader);

		switch (type)
		{
			case XML_READER_TYPE_ELEMENT:
				error = _addElement(xmlTextReaderConstName(reader));
				break;

			case XML_READER_TYPE_END_ELEMENT:
				error = _endElement(xmlTextReaderConstName(reader));
				break;

			case XML_READER_TYPE_TEXT:
				error =	_addValueToCurrentElement(xmlTextReaderConstValue(reader));
				break;
		}
	}

	if (ret == 0 || error != EOSErrorNone)
		done = true;
	else
		done = false;

	return error;
}

XMLElementType XMLParser::nameToXMLElementType(const xmlChar* name)
{
	XMLElementType 	type = XMLElementTypeIllegal;
	Uint32			i;

	for (i=0;i<_numXMLElementTypes;i++)
	{
		if (!strcmp(_xmlElementTypeNames[i], (const char*) name))
		{
			type = i;
			break;
		}
	}

	return type;
}

EOSError XMLParser::parse(Char* filename)
{
	EOSError 	error = EOSErrorNone;
	Boolean		done = false;

	_reader = xmlReaderForFile(filename, NULL, XML_PARSE_DTDVALID);

	if (_reader != NULL)
	{	
		error = _parseLine(_reader, done);

		while (done == false)
		{
			error = _parseLine(_reader, done);

			if (error != EOSErrorNone)
			{
				fprintf(stderr, "Error around line %d: %s\n", xmlTextReaderGetParserLineNumber(_reader), xmlTextReaderConstName(_reader));
			}
		}

        if (xmlTextReaderIsValid(_reader) != 1)
		{
			fprintf(stderr, "XML file does not validate.\n");
		}

		xmlFreeTextReader(_reader);
	}
	else
		error = EOSErrorResourceNotAvailable;

	xmlCleanupParser();

	return error;
}


