/******************************************************************************
 *
 * File: TextureAtlasXML.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Object
 * 
 *****************************************************************************/

#include "toolkit/TextureAtlasXML.h"

Char* TextureAtlasXMLParser::_xmlElementTypeNamespace[TextureAtlasXMLParser::TextureAtlasXMLElementTypeLast] = 
{
	"texatlas",
	"dimensions",
	"subtex",
	"location",
	"vertices",
	"uvs",
	"xy0",
	"xy1",
	"xy2",
	"xy3",
	"uv0",
	"uv1",
	"uv2",
	"uv3",
	"name",
	"refid",
	"x",
	"y",
	"width",
	"height",
	"u",
	"v",
};

TextureAtlasXMLParser::TextureAtlasXMLParser()
{
	setXMLElementTypeNamespace(TextureAtlasXMLElementTypeLast, _xmlElementTypeNamespace);

	memset(&_exportDB, 0, sizeof(TextureAtlasDBExport));
}

TextureAtlasXMLParser::~TextureAtlasXMLParser()
{
	destroyXMLElementNamespace();

	if (_exportDB.subTex)
		delete _exportDB.subTex;

	if (_exportDB.names)
		delete _exportDB.names;
}

EOSError TextureAtlasXMLParser::_addName(TextureAtlasDBExport& db, Char* name, Uint32& offset)
{
	Char*		newlist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (db.names)
	{
		strsize = strlen(db.names);

		while (curr < db.namesSize)
		{
			if (!strcmp(name, &db.names[curr]))
			{
				offset = (Uint32) curr;
				found = true;
				break;
			}

			curr += strsize + 1;
		}
	}

	if (found == false)
	{
		strsize = strlen(name);
	
		newlist = new Char[db.namesSize + strsize + 1];
	
		if (newlist)
		{
			if (db.names)
			{
				memcpy(newlist, db.names, db.namesSize);
				delete db.names;
			}
	
			strcpy(&newlist[db.namesSize], name);
			db.names = newlist;
			offset = db.namesSize;
			db.namesSize += (Uint32) strsize + 1;
		}
		else
			error = EOSErrorNoMemory;
	}

	return error;
}

XMLElement* TextureAtlasXMLParser::_buildXMLElementFromName(const xmlChar* name)
{
	XMLElement*		element = NULL;
	XMLElementType	type;

	type = nameToXMLElementType(name);

	if (type != XMLElementTypeIllegal)
	{
		switch (type)
		{
			case TextureAtlasXMLElementTypeTextureAtlas:
				element = new TextureAtlasXMLElementTextureAtlas;
				break;

			case TextureAtlasXMLElementTypeDimensions:
				element = new TextureAtlasXMLElementDimensions;
				break;

			case TextureAtlasXMLElementTypeSubTexture:
				element = new TextureAtlasXMLElementSubTex;
				break;

			case TextureAtlasXMLElementTypeLocation:
				element = new TextureAtlasXMLElementLocation;
				break;

			case TextureAtlasXMLElementTypeVertices:
				element = new TextureAtlasXMLElementVertices;
				break;

			case TextureAtlasXMLElementTypeUVs:
				element = new TextureAtlasXMLElementUVs;
				break;

			case TextureAtlasXMLElementTypeXY0:
				element = new TextureAtlasXMLElementXY0;
				break;

			case TextureAtlasXMLElementTypeXY1:
				element = new TextureAtlasXMLElementXY1;
				break;

			case TextureAtlasXMLElementTypeXY2:
				element = new TextureAtlasXMLElementXY2;
				break;

			case TextureAtlasXMLElementTypeXY3:
				element = new TextureAtlasXMLElementXY3;
				break;

			case TextureAtlasXMLElementTypeUV0:
				element = new TextureAtlasXMLElementUV0;
				break;

			case TextureAtlasXMLElementTypeUV1:
				element = new TextureAtlasXMLElementUV1;
				break;

			case TextureAtlasXMLElementTypeUV2:
				element = new TextureAtlasXMLElementUV2;
				break;

			case TextureAtlasXMLElementTypeUV3:
				element = new TextureAtlasXMLElementUV3;
				break;

			case TextureAtlasXMLElementTypeName:
				element = new TextureAtlasXMLElementName;
				break;

			case TextureAtlasXMLElementTypeRefID:
				element = new TextureAtlasXMLElementRefID;
				break;

			case TextureAtlasXMLElementTypeX:
				element = new TextureAtlasXMLElementX;
				break;

			case TextureAtlasXMLElementTypeY:
				element = new TextureAtlasXMLElementY;
				break;

			case TextureAtlasXMLElementTypeWidth:
				element = new TextureAtlasXMLElementWidth;
				break;

			case TextureAtlasXMLElementTypeHeight:
				element = new TextureAtlasXMLElementHeight;
				break;

			case TextureAtlasXMLElementTypeU:
				element = new TextureAtlasXMLElementU;
				break;

			case TextureAtlasXMLElementTypeV:
				element = new TextureAtlasXMLElementV;
				break;
		}
	}

	return element;
}

SubTextureExport* TextureAtlasXMLParser::findSubTextureExport(Char* name)
{
	SubTextureExport* 	exp = NULL;
	Uint32			  	i;
	Char*				str;

	for (i=0;i<_exportDB.numSubTex;i++)
	{
		str = &_exportDB.names[_exportDB.subTex[i].nameOffset];

		if (!strcmp(str, name))
		{
			exp = &_exportDB.subTex[i];
			break;
		}
	}

	return exp;
}

EOSError TextureAtlasXMLParser::buildDB(void)
{
	EOSError							error = EOSErrorNone;
	TextureAtlasXMLElementTextureAtlas*	texAtlas;
	TextureAtlasXMLElementSubTex*		subTex;
	SubTextureExport*					subTexExp;
	Uint32								i, j;

	if (getRoot() && getRoot()->getType() == TextureAtlasXMLElementTypeTextureAtlas)
	{
		texAtlas = (TextureAtlasXMLElementTextureAtlas*) getRoot();

		error = _addName(_exportDB, texAtlas->getName()->getString(), _exportDB.nameOffset);

		_exportDB.width = texAtlas->getDimensions()->getWidth()->getValue();
		_exportDB.height = texAtlas->getDimensions()->getHeight()->getValue();

		if (error == EOSErrorNone)
		{
			_exportDB.subTex = new SubTextureExport[texAtlas->getNumSubTex()];

			if (_exportDB.subTex)
			{
				_exportDB.numSubTex = texAtlas->getNumSubTex();
				memset(_exportDB.subTex, 0, sizeof(SubTextureExport) * texAtlas->getNumSubTex());

				for (i=0;i<_exportDB.numSubTex;i++)
				{
					subTex = texAtlas->getSubTexAtIndex(i);
					subTexExp = &_exportDB.subTex[i];

					error = _addName(_exportDB, subTex->getName()->getString(), subTexExp->nameOffset);

					if (error == EOSErrorNone)
					{
						subTexExp->refID = subTex->getRefID()->getValue();

						for (j=0;j<4;j++)
						{
							subTexExp->vertices[j].x = subTex->getVertices()->getVertexAtIndex(j)->getX()->getValue();
							subTexExp->vertices[j].y = subTex->getVertices()->getVertexAtIndex(j)->getY()->getValue();
							subTexExp->uvs[j].x = subTex->getUVs()->getUVAtIndex(j)->getU()->getValue();
							subTexExp->uvs[j].y = subTex->getUVs()->getUVAtIndex(j)->getV()->getValue();
						}
					}
				}
			}
			else
				error = EOSErrorNone;
		}
	}

	return error;
}

EOSError TextureAtlasXMLParser::exportDB(Char* filename)
{
	return EOSErrorUnsupported;
}

TextureAtlasXMLElementName* TextureAtlasXMLParser::getName(void)
{
	if (getRoot() && getRoot()->getType() == TextureAtlasXMLElementTypeTextureAtlas)
	{
		TextureAtlasXMLElementTextureAtlas* texAtlas = (TextureAtlasXMLElementTextureAtlas*) getRoot();

		return texAtlas->getName();
	}
	else
		return NULL;
}

TextureAtlasXMLElementDimensions* TextureAtlasXMLParser::getDimensions(void)
{
	if (getRoot() && getRoot()->getType() == TextureAtlasXMLElementTypeTextureAtlas)
	{
		TextureAtlasXMLElementTextureAtlas* texAtlas = (TextureAtlasXMLElementTextureAtlas*) getRoot();

		return texAtlas->getDimensions();
	}
	else
		return NULL;
}

Uint32 TextureAtlasXMLParser::getNumSubTex(void)
{
	if (getRoot() && getRoot()->getType() == TextureAtlasXMLElementTypeTextureAtlas)
	{
		TextureAtlasXMLElementTextureAtlas* texAtlas = (TextureAtlasXMLElementTextureAtlas*) getRoot();

		return texAtlas->getNumSubTex();
	}
	else
		return 0;
}

TextureAtlasXMLElementSubTex* TextureAtlasXMLParser::getSubTexAtIndex(Uint32 index)
{
	if (getRoot() && getRoot()->getType() == TextureAtlasXMLElementTypeTextureAtlas)
	{
		TextureAtlasXMLElementTextureAtlas* texAtlas = (TextureAtlasXMLElementTextureAtlas*) getRoot();

		return texAtlas->getSubTexAtIndex(index);
	}
	else
		return NULL;
}

TextureAtlasXMLElementXY::TextureAtlasXMLElementXY() : _x(NULL), _y(NULL)
{
}

TextureAtlasXMLElementXY::~TextureAtlasXMLElementXY()
{
	_x = NULL;
	_y = NULL;
}

EOSError TextureAtlasXMLElementXY::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeX:
			_x = (TextureAtlasXMLElementX*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeY:
			_y = (TextureAtlasXMLElementY*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError TextureAtlasXMLElementXY::end(const xmlChar* element)
{
	if (_x && _y)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TextureAtlasXMLElementUV::TextureAtlasXMLElementUV() : _u(NULL), _v(NULL)
{
}

TextureAtlasXMLElementUV::~TextureAtlasXMLElementUV()
{
	_u = NULL;
	_v = NULL;
}

EOSError TextureAtlasXMLElementUV::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeU:
			_u = (TextureAtlasXMLElementU*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeV:
			_v = (TextureAtlasXMLElementV*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError TextureAtlasXMLElementUV::end(const xmlChar* element)
{
	if (_u && _v)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TextureAtlasXMLElementUVs::TextureAtlasXMLElementUVs() : _numUVs(0), _uvs(NULL)
{
	setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeUVs);
}

TextureAtlasXMLElementUVs::~TextureAtlasXMLElementUVs()
{
	if (_uvs)
		delete _uvs;

	_uvs = NULL;
	_numUVs = 0;
}

TextureAtlasXMLElementUV* TextureAtlasXMLElementUVs::getUVAtIndex(Uint32 index)
{
	if (_uvs && index < _numUVs)
		return _uvs[index];
	else
		return NULL;
}

EOSError TextureAtlasXMLElementUVs::addChild(XMLElement* child)
{
	EOSError 					error = EOSErrorNone;
	TextureAtlasXMLElementUV**	newlist;
	Uint32						i;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV0:
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV1:
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV2:
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeUV3:
			newlist = new TextureAtlasXMLElementUV*[_numUVs + 1];

			if (newlist)
			{
				if (_uvs)
				{
					for (i=0;i<_numUVs;i++)
					{
						newlist[i] = _uvs[i];
					}

					delete _uvs;
				}

				newlist[_numUVs] = (TextureAtlasXMLElementUV*) child;
				_uvs = newlist;
				_numUVs++;
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

EOSError TextureAtlasXMLElementUVs::end(const xmlChar* element)
{
	if (_uvs)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TextureAtlasXMLElementVertices::TextureAtlasXMLElementVertices() : _numVertices(0), _vertices(NULL)
{
	setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeVertices);
}

TextureAtlasXMLElementVertices::~TextureAtlasXMLElementVertices()
{
	if (_vertices)
		delete _vertices;

	_vertices = NULL;
	_numVertices = 0;
}

TextureAtlasXMLElementXY* TextureAtlasXMLElementVertices::getVertexAtIndex(Uint32 index)
{
	if (_vertices && index < _numVertices)
		return _vertices[index];
	else
		return NULL;
}

EOSError TextureAtlasXMLElementVertices::addChild(XMLElement* child)
{
	EOSError 					error = EOSErrorNone;
	TextureAtlasXMLElementXY**	newlist;
	Uint32						i;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY0:
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY1:
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY2:
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeXY3:
			newlist = new TextureAtlasXMLElementXY*[_numVertices + 1];

			if (newlist)
			{
				if (_vertices)
				{
					for (i=0;i<_numVertices;i++)
					{
						newlist[i] = _vertices[i];
					}

					delete _vertices;
				}

				newlist[_numVertices] = (TextureAtlasXMLElementXY*) child;
				_vertices = newlist;
				_numVertices++;
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

EOSError TextureAtlasXMLElementVertices::end(const xmlChar* element)
{
	if (_vertices)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TextureAtlasXMLElementLocation::TextureAtlasXMLElementLocation() : _x(NULL), _y(NULL), _width(NULL), _height(NULL)
{
	setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeLocation);
}

TextureAtlasXMLElementLocation::~TextureAtlasXMLElementLocation()
{
	_x = NULL;
	_y = NULL;
	_width = NULL;
	_height = NULL;
}

EOSError TextureAtlasXMLElementLocation::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeX:
			_x = (TextureAtlasXMLElementX*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeY:
			_y = (TextureAtlasXMLElementY*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeWidth:
			_width = (TextureAtlasXMLElementWidth*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeHeight:
			_height = (TextureAtlasXMLElementHeight*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError TextureAtlasXMLElementLocation::end(const xmlChar* element)
{
	if (_x && _y && _width && _height)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TextureAtlasXMLElementSubTex::TextureAtlasXMLElementSubTex() : _refID(NULL), _name(NULL), _location(NULL), _vertices(NULL), _uvs(NULL)
{
	setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeSubTexture);
}

TextureAtlasXMLElementSubTex::~TextureAtlasXMLElementSubTex()
{
	_refID = NULL;
	_name = NULL;
	_location = NULL;
	_vertices = NULL;
	_uvs = NULL;
}

EOSError TextureAtlasXMLElementSubTex::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeRefID:
			_refID = (TextureAtlasXMLElementRefID*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeName:
			_name = (TextureAtlasXMLElementName*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeLocation:
			_location = (TextureAtlasXMLElementLocation*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeVertices:
			_vertices = (TextureAtlasXMLElementVertices*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeUVs:
			_uvs = (TextureAtlasXMLElementUVs*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError TextureAtlasXMLElementSubTex::end(const xmlChar* element)
{
	if (_refID && _name && (_location || (_vertices && _uvs)))
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TextureAtlasXMLElementDimensions::TextureAtlasXMLElementDimensions() : _width(NULL), _height(NULL)
{
	setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeDimensions);
}

TextureAtlasXMLElementDimensions::~TextureAtlasXMLElementDimensions()
{
	_width = NULL;
	_height = NULL;
}

EOSError TextureAtlasXMLElementDimensions::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeWidth:
			_width = (TextureAtlasXMLElementWidth*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeHeight:
			_height = (TextureAtlasXMLElementHeight*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError TextureAtlasXMLElementDimensions::end(const xmlChar* element)
{
	if (_width && _height)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TextureAtlasXMLElementTextureAtlas::TextureAtlasXMLElementTextureAtlas() : _name(NULL), _dimensions(NULL), _numSubTex(0), _subTex(NULL)
{
	setType(TextureAtlasXMLParser::TextureAtlasXMLElementTypeTextureAtlas);
}

TextureAtlasXMLElementTextureAtlas::~TextureAtlasXMLElementTextureAtlas()
{
	if (_subTex)
		delete _subTex;

	_subTex = NULL;
	_numSubTex = 0;

	_name = NULL;
	_dimensions = NULL;
}

TextureAtlasXMLElementSubTex* TextureAtlasXMLElementTextureAtlas::getSubTexAtIndex(Uint32 index)
{
	if (_subTex && index < _numSubTex)
		return _subTex[index];
	else
		return NULL;
}

EOSError TextureAtlasXMLElementTextureAtlas::addChild(XMLElement* child)
{
	EOSError 					error = EOSErrorNone;
	TextureAtlasXMLElementSubTex**	newlist;
	Uint32						i;

	switch (child->getType())
	{
		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeName:
			_name = (TextureAtlasXMLElementName*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeDimensions:
			_dimensions = (TextureAtlasXMLElementDimensions*) child;
			break;

		case TextureAtlasXMLParser::TextureAtlasXMLElementTypeSubTexture:
			newlist = new TextureAtlasXMLElementSubTex*[_numSubTex + 1];

			if (newlist)
			{
				if (_subTex)
				{
					for (i=0;i<_numSubTex;i++)
					{
						newlist[i] = _subTex[i];
					}

					delete _subTex;
				}

				newlist[_numSubTex] = (TextureAtlasXMLElementSubTex*) child;
				_subTex = newlist;
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

EOSError TextureAtlasXMLElementTextureAtlas::end(const xmlChar* element)
{
	if (_name && _dimensions && _subTex)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

