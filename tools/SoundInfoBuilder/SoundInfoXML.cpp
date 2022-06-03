/******************************************************************************
 *
 * File: SoundInfoXML.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sound info XML
 * 
 *****************************************************************************/

#include "Platform.h"
#include "SoundInfoXML.h"
#include "File.h"

Char* _soundFormatNames[SoundFormatLast + 1] = 
{
	"SoundFormatPCM_MONO8",
	"SoundFormatPCM_MONO16",
	"SoundFormatPCM_STEREO8",
	"SoundFormatPCM_STEREO16",
	"SoundFormatWAV",
	"SoundFormatCAF",
	"SoundFormatMP3",
	"SoundFormatAAC",
	"SoundFormatIllegal"
};

Char* _xmlElementType[SoundInfoXMLElementTypeLast + 1] = 
{
	"soundinfodb",
	"soundinfo",
	"refid",
	"name",
	"filename",
	"streaming",
	"bigendian",
	"signed",
	"stereo",
	"samplerate",
	"format",
	"XMLElementTypeIllegal", 
};

SoundInfoXMLElementType	nameToSoundInfoXMLElementType(const xmlChar* name)
{
	SoundInfoXMLElementType 	type = SoundInfoXMLElementTypeIllegal;
	Uint32			i;

	for (i=0;i<SoundInfoXMLElementTypeLast;i++)
	{
		if (!strcmp(_xmlElementType[i], (const char*) name))
		{
			type = (SoundInfoXMLElementType) i;
			break;
		}
	}

	return type;
}

XMLElementSoundFormat::XMLElementSoundFormat() : _format(SoundFormatIllegal)
{
	setType(SoundInfoXMLElementTypeSoundFormat);
}

XMLElementSoundFormat::~XMLElementSoundFormat()
{
}

EOSError XMLElementSoundFormat::setValue(const xmlChar* value)
{
	EOSError 	error = EOSErrorIllegalGrammar;
	Uint32		i;

	for (i=0;i<SoundFormatLast;i++)
	{
		if (!strcmp(_soundFormatNames[i], (const char*) value))
		{
			_format = (SoundFormat) i;
			error = EOSErrorNone;
			break;
		}
	}

	return error;
}

EOSError XMLElementSoundFormat::end(const xmlChar* elementName)
{
	if (_format != SoundFormatIllegal)
		return XMLElementLeaf::end(elementName);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSoundInfo::XMLElementSoundInfo() : _refID(NULL), _name(NULL), _filename(NULL), _streaming(NULL), _bigEndian(NULL), _signed(NULL), 
										_stereo(NULL), _sampleRate(NULL), _format(NULL)
{
	setType(SoundInfoXMLElementTypeSoundInfo);
}

XMLElementSoundInfo::~XMLElementSoundInfo()
{
	_refID = NULL;
	_name = NULL;
	_filename = NULL;
	_streaming = NULL;
	_bigEndian = NULL;
	_signed = NULL;
	_stereo = NULL;
	_sampleRate = NULL;
	_format = NULL;
}

EOSError XMLElementSoundInfo::addChild(XMLElement* child)
{
	EOSError error = EOSErrorNone;

	switch (child->getType())
	{
		case SoundInfoXMLElementTypeRefID:
			_refID = (XMLElementRefID*) child;
			break;

		case SoundInfoXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case SoundInfoXMLElementTypeFilename:
			_filename = (XMLElementFilename*) child;
			break;

		case SoundInfoXMLElementTypeStreaming:
			_streaming = (XMLElementStreaming*) child;
			break;

		case SoundInfoXMLElementTypeBigEndian:
			_bigEndian = (XMLElementBigEndian*) child;
			break;

		case SoundInfoXMLElementTypeSigned:
			_signed = (XMLElementSigned*) child;
			break;

		case SoundInfoXMLElementTypeStereo:
			_stereo = (XMLElementStereo*) child;
			break;

		case SoundInfoXMLElementTypeSampleRate:
			_sampleRate = (XMLElementSampleRate*) child;
			break;

		case SoundInfoXMLElementTypeSoundFormat:
			_format = (XMLElementSoundFormat*) child;
			break;

		default:
			error = EOSErrorIllegalGrammar;
			break;
	}

	if (error == EOSErrorNone)
		error = XMLElementNode::addChild(child);

	return error;
}

EOSError XMLElementSoundInfo::end(const xmlChar* element)
{
	if (_name && _filename && _format)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

XMLElementSoundInfoDB::XMLElementSoundInfoDB() : _name(NULL), _numSoundInfo(0), _soundInfo(NULL)
{
	setType(SoundInfoXMLElementTypeSoundInfoDB);
}

XMLElementSoundInfoDB::~XMLElementSoundInfoDB()
{
	_name = NULL;
	_numSoundInfo = NULL;
	_soundInfo = NULL;
}

XMLElementSoundInfo* XMLElementSoundInfoDB::getSoundInfoAtIndex(Uint32 index)
{
	if (_soundInfo && index < _numSoundInfo)
		return _soundInfo[index];
	else
		return NULL;
}

EOSError XMLElementSoundInfoDB::addChild(XMLElement* child)
{
	EOSError 			error = EOSErrorNone;
	XMLElementSoundInfo**	newlist;
	Uint32				i;

	switch (child->getType())
	{
		case SoundInfoXMLElementTypeName:
			_name = (XMLElementName*) child;
			break;

		case SoundInfoXMLElementTypeSoundInfo:
			newlist = new XMLElementSoundInfo*[_numSoundInfo + 1];

			if (newlist)
			{
				if (_soundInfo)
				{
					for (i=0;i<_numSoundInfo;i++)
					{
						newlist[i] = _soundInfo[i];
					}

					delete _soundInfo;
				}

				newlist[_numSoundInfo] = (XMLElementSoundInfo*) child;
				_soundInfo = newlist;
				_numSoundInfo++;
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

EOSError XMLElementSoundInfoDB::end(const xmlChar* element)
{
	if (_name && _soundInfo)
		return XMLElementNode::end(element);
	else
		return EOSErrorIllegalGrammar;
}

SoundInfoDBXMLParser::SoundInfoDBXMLParser()
{
	memset(&_exportDB, 0, sizeof(SoundInfoDBExport));
}

SoundInfoDBXMLParser::~SoundInfoDBXMLParser()
{
	if (_exportDB.soundInfo)
		delete _exportDB.soundInfo;

	if (_exportDB.names)
		delete _exportDB.names;
}

XMLElement* SoundInfoDBXMLParser::_buildXMLElementFromName(const xmlChar* name)
{
	XMLElement*					element = NULL;
	SoundInfoXMLElementType		type;

	type = nameToSoundInfoXMLElementType(name);

	if (type != SoundInfoXMLElementTypeIllegal)
	{
		switch (type)
		{
			case SoundInfoXMLElementTypeSoundInfoDB: 
				element = new XMLElementSoundInfoDB;
				break;

			case SoundInfoXMLElementTypeSoundInfo: 
				element = new XMLElementSoundInfo;
				break;

			case SoundInfoXMLElementTypeRefID: 
				element = new XMLElementRefID;
				break;

			case SoundInfoXMLElementTypeName: 
				element = new XMLElementName;
				break;

			case SoundInfoXMLElementTypeFilename: 
				element = new XMLElementFilename;
				break;

			case SoundInfoXMLElementTypeStreaming: 
				element = new XMLElementStreaming;
				break;

			case SoundInfoXMLElementTypeBigEndian: 
				element = new XMLElementBigEndian;
				break;

			case SoundInfoXMLElementTypeSigned: 
				element = new XMLElementSigned;
				break;

			case SoundInfoXMLElementTypeStereo: 
				element = new XMLElementStereo;
				break;

			case SoundInfoXMLElementTypeSampleRate: 
				element = new XMLElementSampleRate;
				break;

			case SoundInfoXMLElementTypeSoundFormat: 
				element = new XMLElementSoundFormat;
				break;
		}
	}

	return element;
}

EOSError SoundInfoDBXMLParser::_addName(SoundInfoDBExport& db, Char* name, Uint32& offset)
{
	Char*		newlist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (db.names)
	{
		while (curr < db.namesSize)
		{
			strsize = strlen(&db.names[curr]);

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

EOSError SoundInfoDBXMLParser::buildDB(void)
{
	EOSError							error = EOSErrorNone;
	XMLElementSoundInfoDB*				soundInfoDB;
	XMLElementSoundInfo*				soundInfo;
	SoundInfoExport*					soundInfoExp;
	Uint32								i;

	if (getRoot() && getRoot()->getType() == SoundInfoXMLElementTypeSoundInfoDB)
	{
		soundInfoDB = (XMLElementSoundInfoDB*) getRoot();

		if (soundInfoDB)
		{
			error = _addName(_exportDB, soundInfoDB->getName()->getString(), _exportDB.nameOffset);

			if (error == EOSErrorNone)
			{
				_exportDB.soundInfo = new SoundInfoExport[soundInfoDB->getNumSoundInfo()];

				if (_exportDB.soundInfo)
				{
					memset(_exportDB.soundInfo, 0, sizeof(SoundInfoExport) * soundInfoDB->getNumSoundInfo());
					_exportDB.numSoundInfo = soundInfoDB->getNumSoundInfo();

					for (i=0;i<soundInfoDB->getNumSoundInfo();i++)
					{
						soundInfo = soundInfoDB->getSoundInfoAtIndex(i);
						soundInfoExp = &_exportDB.soundInfo[i];

						error = _addName(_exportDB, soundInfo->getName()->getString(), soundInfoExp->nameOffset);

						if (soundInfo->getRefID())
							soundInfoExp->refID = soundInfo->getRefID()->getValue();
						else
							soundInfoExp->refID = 0;

						if (error == EOSErrorNone)
							error = _addName(_exportDB, soundInfo->getFilename()->getString(), soundInfoExp->filenameOffset);

						if (error == EOSErrorNone)
						{
							if (soundInfo->getStreaming())
							{
								if (soundInfo->getStreaming()->getValue())
									soundInfoExp->streaming = 1;
								else
									soundInfoExp->streaming = 0;
							}
							else
								soundInfoExp->streaming = 0;

							if (soundInfo->getBigEndian())
							{
								if (soundInfo->getBigEndian()->getValue())
									soundInfoExp->bigEndian = 1;
								else
									soundInfoExp->bigEndian = 0;
							}
							else
								soundInfoExp->bigEndian = 0;

							if (soundInfo->getSigned())
							{
								if (soundInfo->getSigned()->getValue())
									soundInfoExp->isSigned = 1;
								else
									soundInfoExp->isSigned = 0;
							}
							else
								soundInfoExp->isSigned = 0;

							if (soundInfo->getStereo())
							{
								if (soundInfo->getStereo()->getValue())
									soundInfoExp->stereo = 1;
								else
									soundInfoExp->stereo = 0;
							}
							else
								soundInfoExp->stereo = 0;

							if (soundInfo->getStreaming())
							{
								if (soundInfo->getStreaming()->getValue())
									soundInfoExp->streaming = 1;
								else
									soundInfoExp->streaming = 0;
							}
							else
								soundInfoExp->streaming = 0;

							if (soundInfo->getSampleRate())
								soundInfoExp->sampleRate = soundInfo->getSampleRate()->getValue();
							else
								soundInfoExp->sampleRate = 0;

							soundInfoExp->soundFormat = soundInfo->getSoundFormat()->getSoundFormat();
						}

						if (error != EOSErrorNone)
							break;
					}
				}
				else
					error = EOSErrorNoMemory;
			}
		}
		else
			error = EOSErrorResourceNotAvailable;
	}

	return error;
}

EOSError SoundInfoDBXMLParser::exportDB(Char* filename)
{
	EOSError				error = EOSErrorNone;
	SoundInfoDBHeader		header;
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
		totalsize = sizeof(SoundInfoDBHeader);
		totalsize += _exportDB.numSoundInfo * sizeof(SoundInfoExport);
		totalsize += _exportDB.namesSize;

		image = new Uint8[totalsize];

		if (image)
		{
			header.endian = 0x01020304;
			header.version = 0;
			header.nameOffset = _exportDB.nameOffset;
			header.numSoundInfo = _exportDB.numSoundInfo;
			header.soundInfo = 0;
			header.names = 0;

			offset += sizeof(SoundInfoDBHeader);

			//	Copy all of the SoundInfo
			header.soundInfo = offset;

			for (i=0;i<header.numSoundInfo;i++)
			{
				memcpy(&image[offset], &_exportDB.soundInfo[i], sizeof(SoundInfoExport));
				offset += sizeof(SoundInfoExport);
			}

			//	Copy the names
			header.names = offset;
			memcpy(&image[offset], _exportDB.names, _exportDB.namesSize);
			offset += _exportDB.namesSize;

			//	Now write the header, which is at the beginning of the file
			memcpy(image, &header, sizeof(SoundInfoDBHeader));

			file.writeUint8(image, totalsize);

			delete image;
		}
		else
			error = EOSErrorNoMemory;

		file.close();
	}

	return error;
}



