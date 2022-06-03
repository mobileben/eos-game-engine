/******************************************************************************
 *
 * File: TextAtlasMappingXML.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Mapping XML
 * 
 *****************************************************************************/

#include "Platform.h"
#include "TexAtlasMappingXML.h"

Char* _xmlNames[TexAtlasMappingXMLElementTypeLast + 1] = 
{
	"textureatlasmapping",
	"name",
	"master",
	"atlas",
	"TexAtlasMappingXMLElementTypeIllegal"
};


TexAtlasMappingXMLElementType	nameToTexAtlasMappingXMLElementType(const xmlChar* name)
{
	TexAtlasMappingXMLElementType 	type = TexAtlasMappingXMLElementTypeIllegal;
	Uint32			i;

	for (i=0;i<TexAtlasMappingXMLElementTypeLast;i++)
	{
		if (!strcmp(_xmlNames[i], (const char*) name))
		{
			type = (TexAtlasMappingXMLElementType) i;
			break;
		}
	}

	return type;
}

XMLElementTextureAtlasMapping::XMLElementTextureAtlasMapping() : _name(NULL), _master(NULL), _numAtlases(0), _atlases(NULL)
{
	setType(TexAtlasMappingXMLElementTypeTextureAtlasMapping);
}

XMLElementTextureAtlasMapping::~XMLElementTextureAtlasMapping()
{
	if (_atlases)
		delete _atlases;

	_atlases = NULL;
	_numAtlases = 0;
}

XMLElementAtlas* XMLElementTextureAtlasMapping::getAtlasAtIndex(Uint32 index)
{
	if (_atlases && index < _numAtlases)
		return _atlases[index];
	else
		return NULL;
}

EOSError XMLElementTextureAtlasMapping::addChild(XMLElement* child)
{
	EOSError 			error = EOSErrorNone;
	XMLElementAtlas**	newlist;
	Uint32						i;
 
	switch (child->getType())
	{
		case TexAtlasMappingXMLElementTypeAtlas:
			newlist = new XMLElementAtlas*[_numAtlases + 1];

			if (newlist)
			{
				if (_atlases)
				{
					for (i=0;i<_numAtlases;i++)
					{
						newlist[i] = _atlases[i];
					}

					delete _atlases;
				}

				newlist[_numAtlases] = (XMLElementAtlas*) child;
				_atlases = newlist;
				_numAtlases++;
			}
			else
				error = EOSErrorNoMemory;
			break;

		case TexAtlasMappingXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case TexAtlasMappingXMLElementTypeMaster:
			_master = (XMLElementMaster*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementTextureAtlasMapping::end(const xmlChar* element)
{
	if (_name && _master && _atlases)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TexAtlasMappingXMLParser::TexAtlasMappingXMLParser()
{
}

TexAtlasMappingXMLParser::~TexAtlasMappingXMLParser()
{
}

XMLElement* TexAtlasMappingXMLParser::_buildXMLElementFromName(const xmlChar* name)
{
	XMLElement*						element = NULL;
	TexAtlasMappingXMLElementType	type;

	type = nameToTexAtlasMappingXMLElementType(name);

	if (type != TexAtlasMappingXMLElementTypeIllegal)
	{
		switch (type)
		{
			case TexAtlasMappingXMLElementTypeTextureAtlasMapping: 
				element = new XMLElementTextureAtlasMapping;
				break;

			case TexAtlasMappingXMLElementTypeName: 
				element = new XMLElementName;
				break;

			case TexAtlasMappingXMLElementTypeMaster: 
				element = new XMLElementMaster;
				break;

			case TexAtlasMappingXMLElementTypeAtlas: 
				element = new XMLElementAtlas;
				break;
		}
	}

	return element;
}

EOSError TexAtlasMappingXMLParser::buildDB(void)
{
	return EOSErrorUnsupported;
}


EOSError TexAtlasMappingXMLParser::exportDB(Char* filename)
{
	return EOSErrorUnsupported;
}

XMLElementName* TexAtlasMappingXMLParser::getName(void)
{
	if (getRoot() && getRoot()->getType() == TexAtlasMappingXMLElementTypeTextureAtlasMapping)
	{
		XMLElementTextureAtlasMapping*	mapping = (XMLElementTextureAtlasMapping*) getRoot();

		return mapping->getName();
	}
	else
		return NULL;
}

XMLElementMaster* TexAtlasMappingXMLParser::getMaster(void)
{
	if (getRoot() && getRoot()->getType() == TexAtlasMappingXMLElementTypeTextureAtlasMapping)
	{
		XMLElementTextureAtlasMapping*	mapping = (XMLElementTextureAtlasMapping*) getRoot();

		return mapping->getMaster();
	}
	else
		return NULL;
}
	
Uint32 TexAtlasMappingXMLParser::getNumAtlases(void)
{
	if (getRoot() && getRoot()->getType() == TexAtlasMappingXMLElementTypeTextureAtlasMapping)
	{
		XMLElementTextureAtlasMapping*	mapping = (XMLElementTextureAtlasMapping*) getRoot();

		return mapping->getNumAtlases();
	}
	else
		return 0;
}

XMLElementAtlas* TexAtlasMappingXMLParser::getAtlasAtIndex(Uint32 index)
{
	if (getRoot() && getRoot()->getType() == TexAtlasMappingXMLElementTypeTextureAtlasMapping)
	{
		XMLElementTextureAtlasMapping*	mapping = (XMLElementTextureAtlasMapping*) getRoot();

		return mapping->getAtlasAtIndex(index);
	}
	else
		return NULL;
}

