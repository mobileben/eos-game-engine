/******************************************************************************
 *
 * File: TexturePropsDBXML.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Properties
 * 
 *****************************************************************************/

#include "Platform.h"
#include "TexturePropsDBXML.h"

Char* _xmlElementType[TexPropsXMLElementTypeLast + 1] = 
{
	"texproperties", 
	"subtex", 
	"name", 
	"saturation", 
	"padedgecolor", 
	"pad",
	"transparentcolor", 
	"padcolor", 
	"padcolortop", 
	"padcolorbottom", 
	"padcolorleft", 
	"padcolorright", 
	"padall", 
	"padtop", 
	"padbottom", 
	"padleft", 
	"padright", 
	"halftexel", 
	"priority", 
	"red", 
	"green", 
	"blue", 
	"alpha", 
	"alphadefringe", 
};

TexPropsXMLElementType	nameToTexPropsXMLElementType(const xmlChar* name)
{
	TexPropsXMLElementType 	type = TexPropsXMLElementTypeIllegal;
	Uint32			i;

	for (i=0;i<TexPropsXMLElementTypeLast;i++)
	{
		if (!strcmp(_xmlElementType[i], (const char*) name))
		{
			type = (TexPropsXMLElementType) i;
			break;
		}
	}

	return type;
}

EOSError XMLElementSaturation::end(const xmlChar* elementName)
{
	if (_value >= 0.0 && _value <= 2.0)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementColorIntRGBA::XMLElementColorIntRGBA() : _red(NULL), _green(NULL), _blue(NULL), _alpha(NULL)
{
}

XMLElementColorIntRGBA::~XMLElementColorIntRGBA()
{
	_red = NULL;
	_green = NULL;
	_blue = NULL;
	_alpha = NULL;
}

EOSError XMLElementColorIntRGBA::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TexPropsXMLElementTypeRed:
			_red = (XMLElementRed*) child;
			break;

		case TexPropsXMLElementTypeGreen:
			_green = (XMLElementGreen*) child;
			break;

		case TexPropsXMLElementTypeBlue:
			_blue = (XMLElementBlue*) child;
			break;

		case TexPropsXMLElementTypeAlpha:
			_alpha = (XMLElementAlpha*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementColorIntRGBA::end(const xmlChar* elementName)
{
	if (_red && _green && _blue && _alpha)
		return XMLElementNode::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementPadEdgeColor::XMLElementPadEdgeColor() : _top(NULL), _bottom(NULL), _left(NULL), _right(NULL)
{
	setType(TexPropsXMLElementTypePadEdgeColor);
}

XMLElementPadEdgeColor::~XMLElementPadEdgeColor()
{
	_top = NULL;
	_bottom = NULL;
	_left = NULL;
	_right = NULL;
}

EOSError XMLElementPadEdgeColor::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TexPropsXMLElementTypePadColorTop:
			_top = (XMLElementPadColorTop*) child;
			break;

		case TexPropsXMLElementTypePadColorBottom:
			_bottom = (XMLElementPadColorBottom*) child;
			break;

		case TexPropsXMLElementTypePadColorLeft:
			_left = (XMLElementPadColorLeft*) child;
			break;

		case TexPropsXMLElementTypePadColorRight:
			_right = (XMLElementPadColorRight*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementPadEdgeColor::end(const xmlChar* elementName)
{
	if (_top || _bottom || _left || _right)
		return XMLElementNode::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementPad::XMLElementPad() : _top(NULL), _bottom(NULL), _left(NULL), _right(NULL)
{
	setType(TexPropsXMLElementTypePad);
}

XMLElementPad::~XMLElementPad()
{
	_top = NULL;
	_bottom = NULL;
	_left = NULL;
	_right = NULL;
}

EOSError XMLElementPad::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TexPropsXMLElementTypePadTop:
			_top = (XMLElementPadTop*) child;
			break;

		case TexPropsXMLElementTypePadBottom:
			_bottom = (XMLElementPadBottom*) child;
			break;

		case TexPropsXMLElementTypePadLeft:
			_left = (XMLElementPadLeft*) child;
			break;

		case TexPropsXMLElementTypePadRight:
			_right = (XMLElementPadRight*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementPad::end(const xmlChar* elementName)
{
	if (_top || _bottom || _left || _right)
		return XMLElementNode::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSubTex::XMLElementSubTex() : _name(NULL), _saturation(NULL), _pad(NULL), _padAll(NULL), _halfTexel(NULL), _transparentColor(NULL),
										_priority(NULL), _padEdgeColor(NULL), _padColor(NULL), _alphaDefringe(NULL)
{
	setType(TexPropsXMLElementTypeSubTex);
}

XMLElementSubTex::~XMLElementSubTex()
{
	_name = NULL;
	_saturation = NULL;
	_pad = NULL;
	_padAll = NULL;
	_halfTexel = NULL;
	_transparentColor = NULL;
	_priority = NULL;
	_padEdgeColor = NULL;
	_padColor = NULL;
	_alphaDefringe = NULL;
}

EOSError XMLElementSubTex::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TexPropsXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case TexPropsXMLElementTypeSaturation:
			_saturation = (XMLElementSaturation*) child;
			break;

		case TexPropsXMLElementTypePad:
			_pad = (XMLElementPad*) child;
			break;

		case TexPropsXMLElementTypePadAll:
			_padAll = (XMLElementPadAll*) child;
			break;

		case TexPropsXMLElementTypeHalfTexel:
			_halfTexel = (XMLElementHalfTexel*) child;
			break;

		case TexPropsXMLElementTypeTransparentColor:
			_transparentColor = (XMLElementTransparentColor*) child;
			break;

		case TexPropsXMLElementTypePriority:
			_priority = (XMLElementPriority*) child;
			break;

		case TexPropsXMLElementTypePadEdgeColor:
			_padEdgeColor = (XMLElementPadEdgeColor*) child;
			break;

		case TexPropsXMLElementTypePadColor:
			_padColor = (XMLElementPadColor*) child;
			break;

		case TexPropsXMLElementTypeAlphaDefringe:
			_alphaDefringe = (XMLElementAlphaDefringe*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSubTex::end(const xmlChar* elementName)
{
	if (_name)
	{
		if (_pad && _padAll)	//	Conflicting
			return EOSErrorIllegalGrammar;

		if (_padEdgeColor && _padColor)	//	Conflicting
			return EOSErrorIllegalGrammar;

		return XMLElementNode::end(elementName);
	}
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTexProperties::XMLElementTexProperties() : _numSubTex(0), _subTex(NULL)
{
	setType(TexPropsXMLElementTypeTexProperties);
}

XMLElementTexProperties::~XMLElementTexProperties()
{
	if (_subTex)
	{
		delete _subTex;
	}

	_numSubTex = 0;
	_subTex = NULL;
}

EOSError XMLElementTexProperties::addChild(XMLElement* child)
{
	EOSError 			error = EOSErrorNone;
	XMLElementSubTex**	newsubtexlist;
	Uint32				i;

	switch (child->getType())
	{
		case TexPropsXMLElementTypeSubTex:
			newsubtexlist = new XMLElementSubTex*[_numSubTex + 1];

			if (newsubtexlist)
			{
				if (_subTex)
				{
					for (i=0;i<_numSubTex;i++)
					{
						newsubtexlist[i] = _subTex[i];
					}

					delete _subTex;
				}

				newsubtexlist[_numSubTex] = (XMLElementSubTex*) child;

				_subTex = newsubtexlist;
				_numSubTex++;
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

EOSError XMLElementTexProperties::end(const xmlChar* elementName)
{
	if (_subTex)
		return XMLElementNode::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSubTex* XMLElementTexProperties::getSubTexAtIndex(Uint32 index)
{
	XMLElementSubTex*	subtex = NULL;

	if (index >=0 && index < _numSubTex)
	{
		if (_subTex)
		{
			subtex = _subTex[index];
		}
	}

	return subtex;
}

TexPropsXMLParser::TexPropsXMLParser() : _stackIndex(0), _root(NULL), _currElement(NULL)
{
	Uint32	i;

	for (i=0;i<MaxXMLStack;i++)
		_stack[i] = NULL;
}

TexPropsXMLParser::~TexPropsXMLParser()
{
}

EOSError TexPropsXMLParser::_addElement(const xmlChar* name)
{
	EOSError			error = EOSErrorNone;
	TexPropsXMLElementType		type;
	XMLElement*			element = NULL;
	XMLElementNode*		current = NULL;

	type = nameToTexPropsXMLElementType(name);

	if (type != TexPropsXMLElementTypeIllegal)
	{
		switch (type)
		{
			case TexPropsXMLElementTypeTexProperties: 
				element = new XMLElementTexProperties;
				break;

			case TexPropsXMLElementTypeSubTex: 
				element = new XMLElementSubTex;
				break;

			case TexPropsXMLElementTypeName: 
				element = new XMLElementName;
				break;

			case TexPropsXMLElementTypeSaturation: 
				element = new XMLElementSaturation;
				break;

			case TexPropsXMLElementTypePadEdgeColor: 
				element = new XMLElementPadEdgeColor;
				break;

			case TexPropsXMLElementTypePad: 
				element = new XMLElementPad;
				break;

			case TexPropsXMLElementTypeTransparentColor: 
				element = new XMLElementTransparentColor;
				break;

			case TexPropsXMLElementTypePadColor: 
				element = new XMLElementPadColor;
				break;

			case TexPropsXMLElementTypePadColorTop: 
				element = new XMLElementPadColorTop;
				break;

			case TexPropsXMLElementTypePadColorBottom: 
				element = new XMLElementPadColorBottom;
				break;

			case TexPropsXMLElementTypePadColorLeft: 
				element = new XMLElementPadColorLeft;
				break;

			case TexPropsXMLElementTypePadColorRight: 
				element = new XMLElementPadColorRight;
				break;

			case TexPropsXMLElementTypePadAll: 
				element = new XMLElementPadAll;
				break;

			case TexPropsXMLElementTypePadTop: 
				element = new XMLElementPadTop;
				break;

			case TexPropsXMLElementTypePadBottom: 
				element = new XMLElementPadBottom;
				break;

			case TexPropsXMLElementTypePadLeft: 
				element = new XMLElementPadLeft;
				break;

			case TexPropsXMLElementTypePadRight: 
				element = new XMLElementPadRight;
				break;

			case TexPropsXMLElementTypeHalfTexel: 
				element = new XMLElementHalfTexel;
				break;

			case TexPropsXMLElementTypePriority: 
				element = new XMLElementPriority;
				break;

			case TexPropsXMLElementTypeRed: 
				element = new XMLElementRed;
				break;

			case TexPropsXMLElementTypeGreen: 
				element = new XMLElementGreen;
				break;

			case TexPropsXMLElementTypeBlue: 
				element = new XMLElementBlue;
				break;

			case TexPropsXMLElementTypeAlpha: 
				element = new XMLElementAlpha;
				break;

			case TexPropsXMLElementTypeAlphaDefringe: 
				element = new XMLElementAlphaDefringe;
				break;
		}

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
	}
	else
	{
		fprintf(stderr, "Could not find ELEMENT %s\n", name);
		error = EOSErrorIllegalGrammar;
	}

	return error;
}

EOSError TexPropsXMLParser::_addValueToCurrentElement(const xmlChar* value)
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

EOSError TexPropsXMLParser::_endElement(const xmlChar* name)
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

EOSError TexPropsXMLParser::_parseLine(xmlTextReaderPtr reader, Boolean& done)
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

EOSError TexPropsXMLParser::parse(Char* filename)
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

Uint32 TexPropsXMLParser::getNumSubTex(void)
{
	Uint32						num = 0;
	XMLElementTexProperties*	texprops;

	if (_root && _root->getType() == TexPropsXMLElementTypeTexProperties)
	{
		texprops = (XMLElementTexProperties*) _root;

		if (texprops)
			num = texprops->getNumSubTex();
	}

	return num;
}

XMLElementSubTex* TexPropsXMLParser::getSubTexAtIndex(Uint32 index)
{
	XMLElementSubTex* 			subtex = NULL;
	XMLElementTexProperties*	texprops;

	if (_root && _root->getType() == TexPropsXMLElementTypeTexProperties)
	{
		texprops = (XMLElementTexProperties*) _root;

		if (texprops)
		{
			subtex = texprops->getSubTexAtIndex(index);
		}
	}

	return subtex;
}

