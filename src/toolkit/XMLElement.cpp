/******************************************************************************
 *
 * File: XMLElement.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * XML Elements for parsing XML. This uses libxml2
 * 
 *****************************************************************************/

#include "toolkit/XMLElement.h"

XMLElement::XMLElement() : _type(XMLElementTypeIllegal), _elementName(NULL), _isLeafType(false)
{
}

XMLElement::~XMLElement()
{
	if (_elementName)
	{
		delete _elementName;
		_elementName = NULL;
	}
}

void XMLElement::setElementName(const xmlChar* name)
{
	if (_elementName)
	{
		delete _elementName;
		_elementName = NULL;
	}

	if (name)
	{
		_elementName = new Char[strlen((const char*) name) + 1];

		if (_elementName)
			strcpy(_elementName, (const char*) name);
	}
}

Char* XMLElement::getElementName(void)
{
	return _elementName;
}

EOSError XMLElement::end(const xmlChar* elementName)
{
	EOSError error = EOSErrorIllegalGrammar;

	if (_elementName && elementName)
	{
		if (!strcmp(_elementName, (const char*) elementName))
			error = EOSErrorNone;
	}

	return error;
}

XMLElementNode::XMLElementNode() : _numChildren(0), _children(NULL)
{
}

XMLElementNode::~XMLElementNode()
{
	Uint32	i;

	if (_children)
	{
		for (i=0;i<_numChildren;i++)
		{
			delete _children[i];
			_children[i] = NULL;
		}

		delete _children;
	}
}

XMLElement* XMLElementNode::getChildAtIndex(Uint32 index)
{
	XMLElement* child = NULL;

	if (_children && index < _numChildren)
		return _children[index];

	return child;
}

EOSError XMLElementNode::addChild(XMLElement* child)
{
	EOSError 		error = EOSErrorNone;
	XMLElement**	newlist;
	Uint32			i;

	newlist = new XMLElement*[_numChildren + 1];

	if (newlist)
	{
		if (_children)
		{
			for (i=0;i<_numChildren;i++)
				newlist[i] = _children[i];

			delete _children;
		}

		newlist[_numChildren] = child;
		_children = newlist;

		_numChildren++;
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

XMLElementLeaf::XMLElementLeaf()
{
	setAsLeafType();
}

XMLElementLeaf::~XMLElementLeaf()
{
}

EOSError XMLElementLeaf::setValue(const xmlChar* value)
{
	return EOSErrorNone;
}

XMLElementString::XMLElementString() : _string(NULL), _case(StringCaseAny)
{
}

XMLElementString::~XMLElementString()
{
	if (_string)
	{
		delete _string;
		_string = NULL;
	}
}

EOSError XMLElementString::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorNone;
	Uint32		i;
	Uint32		len;

	_string = new Char[strlen((const char*) value) + 1];

	if (_string)
	{
		if (_case == StringCaseUpper)
		{
			len = (Uint32) strlen((const char*) value);

			for (i=0;i<len;i++)
			{
				_string[i] = toupper(value[i]);
			}

			_string[len] = 0;
		}
		else if (_case == StringCaseLower)
		{
			len = (Uint32) strlen((const char*) value);

			for (i=0;i<len;i++)
			{
				_string[i] = tolower(value[i]);
			}

			_string[len] = 0;
		}
		else
			strcpy(_string, (const char*) value);
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

EOSError XMLElementString::end(const xmlChar* elementName)
{
	if (_string)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSint32::XMLElementSint32() : _value(0)
{
}

XMLElementSint32::~XMLElementSint32()
{
}

EOSError XMLElementSint32::setValue(const xmlChar* value)
{
	EOSError error = EOSErrorNone;
	size_t	len;
	size_t	i;
	Boolean	numeric = false;
	Uint32	decimalPointCount = 0;

	len = strlen((const char*) value);

	for (i=0;i<len;i++)
	{
		if (value[i] == ' ' || value[i] == '\t' || value[i] == '-' || (value[i] >= '0' && value[i] <= '9'))
		{
			if ((value[i] >= '0' && value[i] <= '9'))
			{
				numeric = true;
			}
		}
		else
		{
			if (decimalPointCount == 0 && value[i] == '.')
			{
				decimalPointCount++;
			}
			else
			{
				error = EOSErrorIllegalGrammar;
				break;
			}
		}
	}

	if (error == EOSErrorNone)
	{
		if (numeric)
		{
			if (decimalPointCount == 0)
				_value = atoi((const char*) value);
			else
				_value = (Sint32) (atof((const char*) value));
		}
		else
			error = EOSErrorIllegalGrammar;
	}

	return error;
}

XMLElementFloat32::XMLElementFloat32() : _value(0)
{
}

XMLElementFloat32::~XMLElementFloat32()
{
}

EOSError XMLElementFloat32::setValue(const xmlChar* value)
{
	EOSError error = EOSErrorNone;
	size_t	len;
	size_t	i;
	Boolean	numeric;

	len = strlen((const char*) value);

	for (i=0;i<len;i++)
	{
		if (value[i] == ' ' || value[i] == '\t' || value[i] == '-' || value[i] == '.' || (value[i] >= '0' && value[i] <= '9'))
		{
			if ((value[i] >= '0' && value[i] <= '9'))
			{
				numeric = true;
			}
		}
		else
		{
			error = EOSErrorIllegalGrammar;
			break;
		}
	}

	if (error == EOSErrorNone)
	{
		if (numeric)
			_value = atof((const char*) value);
		else
			error = EOSErrorIllegalGrammar;
	}

	return error;
}

XMLElementHex::XMLElementHex() : _value(0)
{
}

XMLElementHex::~XMLElementHex()
{
}

EOSError XMLElementHex::setValue(const xmlChar* value)
{
	EOSError error = EOSErrorNone;
	size_t	len;
	size_t	i;
	Char*	tmpstr;
	Uint32 	val;
	Uint32	mult = 0;
	Sint32	idx;

	len = strlen((const char*) value);

	tmpstr = new Char[len + 1];

	if (tmpstr)
	{
		strcpy(tmpstr, (const char*) value);

		for (i=0;i<len;i++)
		{
			tmpstr[i] = tolower(value[i]);

			if ((tmpstr[i] >= '0' && tmpstr[i] <= '9') || (tmpstr[i] >= 'a' && tmpstr[i] <= 'f'))
			{
			}
			else
			{
				error = EOSErrorIllegalGrammar;
				break;
			}
		}
	
		if (error == EOSErrorNone)
		{
			_value = 0;

			// Convert to hex here
			len = strlen(tmpstr);

			mult = 1;

			for (idx=len-1;idx>=0;idx--)
			{
				if (tmpstr[idx] >= '0' && tmpstr[idx] <= '9')
				{
					val = tmpstr[idx] - '0';
				}
				else if (tmpstr[idx] >= 'a' && tmpstr[idx] <= 'f')
				{
					val = tmpstr[idx] - 'a' + 10;
				}
				else
					val = 0;

				_value += val * mult;

				mult *= 16;
			}
		}

		delete tmpstr;
	}

	return error;
}

XMLElementBoolean::XMLElementBoolean() : _value(false)
{
}

XMLElementBoolean::~XMLElementBoolean()
{
}

EOSError XMLElementBoolean::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorNone;
	Uint32		i;
	Uint32		len;
	Char*		tstr;

	tstr = new Char[strlen((const char*) value) + 1];

	if (tstr)
	{
		len = (Uint32) strlen((const char*) value);

		for (i=0;i<len;i++)
		{
			tstr[i] = toupper(value[i]);
		}

		tstr[len] = 0;

		if (!strcmp(tstr, "TRUE"))
			_value = true;
		else if (!strcmp(tstr, "FALSE"))
			_value = false;
		else
			error = EOSErrorIllegalGrammar;
	}
	else
		error = EOSErrorNoMemory;

	return error;
}


