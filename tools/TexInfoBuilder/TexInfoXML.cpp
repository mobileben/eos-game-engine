/******************************************************************************
 *
 * File: TexInfoXML.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture info XML
 * 
 *****************************************************************************/

#include "TexInfoXML.h"
#include "File.h"

Char* _colorFormatNames[TextureFormatLast + 1] = 
{
	"TextureFormatTGA",
	"TextureFormatGL_RGB_888",
	"TextureFormatGL_RGBA_8888",
	"TextureFormatGL_RGB_565",
	"TextureFormatGL_RGBA_5551",
	"TextureFormatGL_RGBA_4444",
	"TextureFormatGL_P8_RGBA_8888",
	"TextureFormatGL_P4_RGBA_4444",
	"TextureFormatPVRTC_4BPP_RGBA",
	"TextureFormatPVRTC_4BPP_RGB",
	"TextureFormatPVRTC_2BPP_RGBA",
	"TextureFormatPVRTC_2BPP_RGB",
	"TextureFormatRAWPVRTC_4BPP_RGBA",
	"TextureFormatRAWPVRTC_4BPP_RGB",
	"TextureFormatRAWPVRTC_2BPP_RGBA",
	"TextureFormatRAWPVRTC_2BPP_RGB",
	"TextureFormatIllegal",
};

Char* _texMipMapNames[TextureState::TEXTURE_MIPMAP_LAST + 1] = 
{
	"TEXTURE_MIPMAP_NEAREST",
	"TEXTURE_MIPMAP_LINEAR",
	"TEXTURE_MIPMAP_NEAREST_NEAREST",
	"TEXTURE_MIPMAP_NEAREST_LINEAR",
	"TEXTURE_MIPMAP_LINEAR_NEAREST",
	"TEXTURE_MIPMAP_LINEAR_LINEAR",
	"TEXTURE_MIPMAP_ILLEGAL",
};

Char* _texWrapNames[TextureState::TEXTURE_WRAP_LAST + 1] = 
{
	"TEXTURE_WRAP_CLAMP",
	"TEXTURE_WRAP_REPEAT",
	"TEXTURE_WRAP_CLAMP_BORDER",
	"TEXTURE_WRAP_CLAMP_EDGE",
	"TEXTURE_WRAP_ILLEGAL"
};

Char* _texEnvModeNames[TextureState::TEXTURE_ENV_MODE_LAST + 1] = 
{
	"TEXTURE_ENV_MODE_REPLACE",
	"TEXTURE_ENV_MODE_MODULATE",
	"TEXTURE_ENV_MODE_DECAL",
	"TEXTURE_ENV_MODE_BLEND",
	"TEXTURE_ENV_MODE_ADD",
	"TEXTURE_ENV_MODE_COMBINE",
	"TEXTURE_ENV_MODE_ILLEGAL",
};

Char* _TexInfoXMLElementType[TexInfoXMLElementTypeLast + 1] = 
{
	"texinfodb", 
	"texinfo", 
	"refid", 
	"name", 
	"filename", 
	"width", 
	"height", 
	"colorformat", 
	"minmipmapfilter", 
	"magmipmapfilter", 
	"wraps", 
	"wrapt", 
	"texenvmode", 
	"TexInfoXMLElementTypeIllegal", 
};

TexInfoXMLElementType	nameToTexInfoXMLElementType(const xmlChar* name)
{
	TexInfoXMLElementType 	type = TexInfoXMLElementTypeIllegal;
	Uint32			i;

	for (i=0;i<TexInfoXMLElementTypeLast;i++)
	{
		if (!strcmp(_TexInfoXMLElementType[i], (const char*) name))
		{
			type = (TexInfoXMLElementType) i;
			break;
		}
	}

	return type;
}

XMLElementColorFormat::XMLElementColorFormat() : _format(TextureFormatIllegal)
{
	setType(TexInfoXMLElementTypeColorFormat);
}

XMLElementColorFormat::~XMLElementColorFormat()
{
}

EOSError XMLElementColorFormat::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureFormatLast;i++)
	{
		if (!strcmp(_colorFormatNames[i], (const char*) value))
		{
			_format = (TextureFormat) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementColorFormat::end(const xmlChar* elementName)
{
	if (_format != TextureFormatIllegal)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementMipMapFilter::XMLElementMipMapFilter() : _mipmap(TextureState::TEXTURE_MIPMAP_ILLEGAL)
{
}

XMLElementMipMapFilter::~XMLElementMipMapFilter()
{
}

EOSError XMLElementMipMapFilter::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureState::TEXTURE_MIPMAP_LAST;i++)
	{
		if (!strcmp(_texMipMapNames[i], (const char*) value))
		{
			_mipmap = (TextureState::TEXTURE_MIPMAP) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementMipMapFilter::end(const xmlChar* elementName)
{
	if (_mipmap != TextureState::TEXTURE_MIPMAP_ILLEGAL)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementWrap::XMLElementWrap() : _wrap(TextureState::TEXTURE_WRAP_ILLEGAL)
{
}

XMLElementWrap::~XMLElementWrap()
{
}

EOSError XMLElementWrap::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureState::TEXTURE_WRAP_LAST;i++)
	{
		if (!strcmp(_texWrapNames[i], (const char*) value))
		{
			_wrap = (TextureState::TEXTURE_WRAP) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementWrap::end(const xmlChar* elementName)
{
	if (_wrap != TextureState::TEXTURE_WRAP_ILLEGAL)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTexEnvMode::XMLElementTexEnvMode() : _texenvmode(TextureState::TEXTURE_ENV_MODE_ILLEGAL)
{
	setType(TexInfoXMLElementTypeTexEnvMode);
}

XMLElementTexEnvMode::~XMLElementTexEnvMode()
{
}

EOSError XMLElementTexEnvMode::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<TextureState::TEXTURE_ENV_MODE_LAST;i++)
	{
		if (!strcmp(_texEnvModeNames[i], (const char*) value))
		{
			_texenvmode = (TextureState::TEXTURE_ENV_MODE) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementTexEnvMode::end(const xmlChar* elementName)
{
	if (_texenvmode != TextureState::TEXTURE_ENV_MODE_ILLEGAL)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTexInfo::XMLElementTexInfo() : _refID(NULL), _name(NULL), _filename(NULL), _width(NULL), _height(NULL), _colorFormat(NULL), 
										_minMipMapFilter(NULL), _magMipMapFilter(NULL), _wrapS(NULL), _wrapT(NULL), _texEnvMode(NULL)
{
	setType(TexInfoXMLElementTypeTexInfo);
}

XMLElementTexInfo::~XMLElementTexInfo()
{
	_refID = NULL;
	_name = NULL;
	_filename = NULL;
	_width = NULL;
	_height = NULL;
	_colorFormat = NULL;
	_minMipMapFilter = NULL;
	_magMipMapFilter = NULL;
	_wrapS = NULL;
	_wrapT = NULL;
	_texEnvMode = NULL;
}

EOSError XMLElementTexInfo::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case TexInfoXMLElementTypeRefID:
			_refID = (XMLElementRefID*) child;
			break;

		case TexInfoXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case TexInfoXMLElementTypeFilename:
			_filename = (XMLElementFilename*) child;
			break;

		case TexInfoXMLElementTypeWidth:
			_width = (XMLElementWidth*) child;
			break;

		case TexInfoXMLElementTypeHeight:
			_height = (XMLElementHeight*) child;
			break;

		case TexInfoXMLElementTypeColorFormat:
			_colorFormat = (XMLElementColorFormat*) child;
			break;

		case TexInfoXMLElementTypeMinMipMapFilter:
			_minMipMapFilter = (XMLElementMinMipMapFilter*) child;
			break;

		case TexInfoXMLElementTypeMagMipMapFilter:
			_magMipMapFilter = (XMLElementMagMipMapFilter*) child;
			break;

		case TexInfoXMLElementTypeWrapS:
			_wrapS = (XMLElementWrapS*) child;
			break;

		case TexInfoXMLElementTypeWrapT:
			_wrapT = (XMLElementWrapT*) child;
			break;

		case TexInfoXMLElementTypeTexEnvMode:
			_texEnvMode = (XMLElementTexEnvMode*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementTexInfo::end(const xmlChar* element)
{
	if (_name && _filename && _width && _height && _colorFormat)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementTexInfoDB::XMLElementTexInfoDB() : _name(NULL), _numTexInfo(0), _texInfo(NULL)
{
	setType(TexInfoXMLElementTypeTexInfoDB);
}

XMLElementTexInfoDB::~XMLElementTexInfoDB()
{
	_name = NULL;
	_numTexInfo = NULL;
	_texInfo = NULL;
}

XMLElementTexInfo* XMLElementTexInfoDB::getTexInfoAtIndex(Uint32 index)
{
	if (_texInfo && index < _numTexInfo)
		return _texInfo[index];
	else
		return NULL;
}

EOSError XMLElementTexInfoDB::addChild(XMLElement* child)
{
	EOSError 			error = EOSErrorNone;
	XMLElementTexInfo**	newlist;
	Uint32				i;

	switch (child->getType())
	{
		case TexInfoXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case TexInfoXMLElementTypeTexInfo:
			newlist = new XMLElementTexInfo*[_numTexInfo + 1];

			if (newlist)
			{
				if (_texInfo)
				{
					for (i=0;i<_numTexInfo;i++)
					{
						newlist[i] = _texInfo[i];
					}

					delete _texInfo;
				}

				newlist[_numTexInfo] = (XMLElementTexInfo*) child;
				_texInfo = newlist;
				_numTexInfo++;
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

EOSError XMLElementTexInfoDB::end(const xmlChar* element)
{
	if (_name && _texInfo)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

TexInfoXMLParser::TexInfoXMLParser() : _stackIndex(0), _root(NULL), _currElement(NULL)
{
	Uint32	i;

	for (i=0;i<MaxXMLStack;i++)
		_stack[i] = NULL;

	memset(&_exportDB, 0, sizeof(TexInfoDBExport));
}

TexInfoXMLParser::~TexInfoXMLParser()
{
	if (_exportDB.texInfo)
		delete _exportDB.texInfo;

	if (_exportDB.names)
		delete _exportDB.names;
}

EOSError TexInfoXMLParser::_addElement(const xmlChar* name)
{
	EOSError			error = EOSErrorNone;
	TexInfoXMLElementType		type;
	XMLElement*			element = NULL;
	XMLElementNode*		current = NULL;

	type = nameToTexInfoXMLElementType(name);

	if (type != TexInfoXMLElementTypeIllegal)
	{
		switch (type)
		{
			case TexInfoXMLElementTypeTexInfoDB: 
				element = new XMLElementTexInfoDB;
				break;

			case TexInfoXMLElementTypeTexInfo: 
				element = new XMLElementTexInfo;
				break;

			case TexInfoXMLElementTypeRefID: 
				element = new XMLElementRefID;
				break;

			case TexInfoXMLElementTypeName: 
				element = new XMLElementName;
				break;

			case TexInfoXMLElementTypeFilename: 
				element = new XMLElementFilename;
				break;

			case TexInfoXMLElementTypeWidth: 
				element = new XMLElementWidth;
				break;

			case TexInfoXMLElementTypeHeight: 
				element = new XMLElementHeight;
				break;

			case TexInfoXMLElementTypeColorFormat: 
				element = new XMLElementColorFormat;
				break;

			case TexInfoXMLElementTypeMinMipMapFilter: 
				element = new XMLElementMinMipMapFilter;
				break;

			case TexInfoXMLElementTypeMagMipMapFilter: 
				element = new XMLElementMagMipMapFilter;
				break;

			case TexInfoXMLElementTypeWrapS: 
				element = new XMLElementWrapS;
				break;

			case TexInfoXMLElementTypeWrapT: 
				element = new XMLElementWrapT;
				break;

			case TexInfoXMLElementTypeTexEnvMode: 
				element = new XMLElementTexEnvMode;
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


EOSError TexInfoXMLParser::_addValueToCurrentElement(const xmlChar* value)
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

EOSError TexInfoXMLParser::_endElement(const xmlChar* name)
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

EOSError TexInfoXMLParser::_parseLine(xmlTextReaderPtr reader, Boolean& done)
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

EOSError TexInfoXMLParser::_addName(TexInfoDBExport& db, Char* name, Uint32& offset)
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

EOSError TexInfoXMLParser::parse(Char* filename)
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

EOSError TexInfoXMLParser::buildDB(void)
{
	EOSError							error = EOSErrorNone;
	XMLElementTexInfoDB*				texInfoDB;
	XMLElementTexInfo*				texInfo;
	TexInfoExport*					texInfoExp;
	Uint32								i;

	if (_root && _root->getType() == TexInfoXMLElementTypeTexInfoDB)
	{
		texInfoDB = (XMLElementTexInfoDB*) _root;

		if (texInfoDB)
		{
			error = _addName(_exportDB, texInfoDB->getName()->getString(), _exportDB.nameOffset);

			_exportDB.texInfo = new TexInfoExport[texInfoDB->getNumTexInfo()];

			if (_exportDB.texInfo)
			{
				memset(_exportDB.texInfo, 0, sizeof(TexInfoExport) * texInfoDB->getNumTexInfo());
				_exportDB.numTexInfo = texInfoDB->getNumTexInfo();

				for (i=0;i<texInfoDB->getNumTexInfo();i++)
				{
					texInfo = texInfoDB->getTexInfoAtIndex(i);
					texInfoExp = &_exportDB.texInfo[i];

					error = _addName(_exportDB, texInfo->getName()->getString(), texInfoExp->nameOffset);

					if (error == EOSErrorNone)
						error = _addName(_exportDB, texInfo->getFilename()->getString(), texInfoExp->filenameOffset);

					if (error == EOSErrorNone)
					{
						Uint32	tstate = 0;

						if (texInfo->getRefID())
							texInfoExp->refID = texInfo->getRefID()->getValue();
						else
							texInfoExp->refID = UndefinedObjectID;

						texInfoExp->width = texInfo->getWidth()->getValue();
						texInfoExp->height = texInfo->getHeight()->getValue();
						texInfoExp->colorFormat = texInfo->getColorFormat()->getColorFormat();

						if (texInfo->getMinMipMapFilter())
						{
							switch (texInfo->getMinMipMapFilter()->getMipMapFilter())
							{
								case TextureState::TEXTURE_MIPMAP_NEAREST:
									tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST;
									break;

								case TextureState::TEXTURE_MIPMAP_LINEAR:
									tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR;
									break;

								case TextureState::TEXTURE_MIPMAP_NEAREST_NEAREST:
									tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_NEAREST;
									break;

								case TextureState::TEXTURE_MIPMAP_NEAREST_LINEAR:
									tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_LINEAR;
									break;

								case TextureState::TEXTURE_MIPMAP_LINEAR_NEAREST:
									tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_NEAREST;
									break;

								case TextureState::TEXTURE_MIPMAP_LINEAR_LINEAR:
									tstate |= TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_LINEAR;
									break;
							}
						}

						if (texInfo->getMagMipMapFilter())
						{
							switch (texInfo->getMagMipMapFilter()->getMipMapFilter())
							{
								case TextureState::TEXTURE_MIPMAP_NEAREST:
									tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST;
									break;

								case TextureState::TEXTURE_MIPMAP_LINEAR:
									tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR;
									break;

								case TextureState::TEXTURE_MIPMAP_NEAREST_NEAREST:
									tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_NEAREST;
									break;

								case TextureState::TEXTURE_MIPMAP_NEAREST_LINEAR:
									tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_LINEAR;
									break;

								case TextureState::TEXTURE_MIPMAP_LINEAR_NEAREST:
									tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_NEAREST;
									break;

								case TextureState::TEXTURE_MIPMAP_LINEAR_LINEAR:
									tstate |= TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_LINEAR;
									break;
							}
						}

						if (texInfo->getWrapS())
						{
							switch (texInfo->getWrapS()->getWrap())
							{
								case TextureState::TEXTURE_WRAP_CLAMP:
									tstate |= TEXTURE_STATE_WRAP_S_CLAMP;
									break;

								case TextureState::TEXTURE_WRAP_REPEAT:
									tstate |= TEXTURE_STATE_WRAP_S_REPEAT;
									break;

								case TextureState::TEXTURE_WRAP_CLAMP_BORDER:
									tstate |= TEXTURE_STATE_WRAP_S_CLAMP_BORDER;
									break;

								case TextureState::TEXTURE_WRAP_CLAMP_EDGE:
									tstate |= TEXTURE_STATE_WRAP_S_CLAMP_EDGE;
									break;
							}
						}

						if (texInfo->getWrapT())
						{
							switch (texInfo->getWrapT()->getWrap())
							{
								case TextureState::TEXTURE_WRAP_CLAMP:
									tstate |= TEXTURE_STATE_WRAP_T_CLAMP;
									break;

								case TextureState::TEXTURE_WRAP_REPEAT:
									tstate |= TEXTURE_STATE_WRAP_T_REPEAT;
									break;

								case TextureState::TEXTURE_WRAP_CLAMP_BORDER:
									tstate |= TEXTURE_STATE_WRAP_T_CLAMP_BORDER;
									break;

								case TextureState::TEXTURE_WRAP_CLAMP_EDGE:
									tstate |= TEXTURE_STATE_WRAP_T_CLAMP_EDGE;
									break;
							}
						}

						if (texInfo->getTexEnvMode())
						{
							switch (texInfo->getTexEnvMode()->getTexEnvMode())
							{
								case TextureState::TEXTURE_ENV_MODE_REPLACE:
									tstate |= TEXTURE_STATE_ENV_REPLACE;
									break;

								case TextureState::TEXTURE_ENV_MODE_MODULATE:
									tstate |= TEXTURE_STATE_ENV_MODULATE;
									break;

								case TextureState::TEXTURE_ENV_MODE_DECAL:
									tstate |= TEXTURE_STATE_ENV_DECAL;
									break;

								case TextureState::TEXTURE_ENV_MODE_BLEND:
									tstate |= TEXTURE_STATE_ENV_BLEND;
									break;

								case TextureState::TEXTURE_ENV_MODE_ADD:
									tstate |= TEXTURE_STATE_ENV_ADD;
									break;

								case TextureState::TEXTURE_ENV_MODE_COMBINE:
									tstate |= TEXTURE_STATE_ENV_COMBINE;
									break;
							}
						}

						texInfoExp->texState = tstate;
					}

					if (error != EOSErrorNone)
						break;
				}
			}
		}
		else
			error = EOSErrorResourceNotAvailable;
	}

	return error;
}

EOSError TexInfoXMLParser::exportDB(Char* filename)
{
	EOSError				error = EOSErrorNone;
	TexInfoDBHeader			header;
	File					file;
	FileDescriptor			fd;
	Uint32					offset = 0;
	Uint32					i;
	Uint8*					image;
	Uint32					totalsize = 0;

	fd.setFilename(filename);
	fd.setFileAccessType(FileAccessTypeWriteOnly);

	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		totalsize = sizeof(TexInfoDBHeader);
		totalsize += _exportDB.numTexInfo * sizeof(TexInfoExport);
		totalsize += _exportDB.namesSize;

#ifdef _PLATFORM_MAC
		image = (Uint8*) malloc(totalsize);
#else
		image = new Uint8[totalsize];
#endif /* _PLATFORM_MAC */
		
		if (image)
		{
			header.endian = 0x01020304;
			header.version = 0;
			header.nameOffset = _exportDB.nameOffset;
			header.numTexInfo = _exportDB.numTexInfo;
			header.texInfo = 0;
			header.names = 0;

			offset += sizeof(TexInfoDBHeader);
			header.texInfo = offset;

			//	Copy all of the texInfo
			for (i=0;i<header.numTexInfo;i++)
			{
				memcpy(&image[offset], &_exportDB.texInfo[i], sizeof(TexInfoExport));
				offset += sizeof(TexInfoExport);
			}

			//	Copy the names
			header.names = offset;
			memcpy(&image[offset], _exportDB.names, _exportDB.namesSize);
			offset += _exportDB.namesSize;

			//	Now write the header, which is at the beginning of the file
			memcpy(image, &header, sizeof(TexInfoDBHeader));

			file.writeUint8(image, totalsize);

#ifndef _PLATFORM_MAC
			delete image;
#endif /* _PLATFORM_MAC */
		}
		else
			error = EOSErrorNoMemory;

		file.close();
	}

	return error;
}

