/******************************************************************************
 *
 * File: SoundInfoXML.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sound info XML
 * 
 *****************************************************************************/

#ifndef __SOUND_INFO_XML_H__
#define __SOUND_INFO_XML_H__

#include "Platform.h"
#include "SoundFXBuffer.h"
#include "toolkit/XMLElement.h"
#include "toolkit/XMLParser.h"
#include <libxml/xmlreader.h>

typedef struct
{
	Uint32				nameOffset;
	Uint32				numSoundInfo;
	SoundInfoExport*	soundInfo;
	Uint32				namesSize;
	Char*				names;	
} SoundInfoDBExport;

typedef enum
{
	SoundInfoXMLElementTypeSoundInfoDB = 0,
	SoundInfoXMLElementTypeSoundInfo,
	SoundInfoXMLElementTypeRefID,
	SoundInfoXMLElementTypeName,
	SoundInfoXMLElementTypeFilename,
	SoundInfoXMLElementTypeStreaming,
	SoundInfoXMLElementTypeBigEndian,
	SoundInfoXMLElementTypeSigned,
	SoundInfoXMLElementTypeStereo,
	SoundInfoXMLElementTypeSampleRate,
	SoundInfoXMLElementTypeSoundFormat,
	SoundInfoXMLElementTypeLast,
	SoundInfoXMLElementTypeIllegal = SoundInfoXMLElementTypeLast,
} SoundInfoXMLElementType;

class XMLElementRefID : public XMLElementSint32
{
public:
	XMLElementRefID() { setType(SoundInfoXMLElementTypeRefID); }
	~XMLElementRefID() {}
};

class XMLElementName : public XMLElementString
{
public:
	XMLElementName() { setType(SoundInfoXMLElementTypeName); }
	~XMLElementName() {}
};

class XMLElementFilename : public XMLElementString
{
public:
	XMLElementFilename() { setType(SoundInfoXMLElementTypeFilename); }
	~XMLElementFilename() {}
};

class XMLElementStreaming : public XMLElementBoolean
{
public:
	XMLElementStreaming() { setType(SoundInfoXMLElementTypeStreaming); }
	~XMLElementStreaming() {}
};

class XMLElementBigEndian : public XMLElementBoolean
{
public:
	XMLElementBigEndian() { setType(SoundInfoXMLElementTypeBigEndian); }
	~XMLElementBigEndian() {}
};

class XMLElementStereo : public XMLElementBoolean
{
public:
	XMLElementStereo() { setType(SoundInfoXMLElementTypeStereo); }
	~XMLElementStereo() {}
};

class XMLElementSigned : public XMLElementBoolean
{
public:
	XMLElementSigned() { setType(SoundInfoXMLElementTypeSigned); }
	~XMLElementSigned() {}
};

class XMLElementSampleRate : public XMLElementSint32
{
public:
	XMLElementSampleRate() { setType(SoundInfoXMLElementTypeSampleRate); }
	~XMLElementSampleRate() {}
};

class XMLElementSoundFormat : public XMLElementString
{
	protected:
		SoundFormat	_format;
public:
	XMLElementSoundFormat();
	~XMLElementSoundFormat();

	inline SoundFormat getSoundFormat(void) { return _format; }

	EOSError							setValue(const xmlChar* value);
	EOSError							end(const xmlChar* elementName);		//	We pass elementName for validation
};

class XMLElementSoundInfo : public XMLElementNode
{
protected:
	XMLElementRefID*		_refID;
	XMLElementName*			_name;
	XMLElementFilename*		_filename;
	XMLElementStreaming*	_streaming;
	XMLElementBigEndian*	_bigEndian;
	XMLElementSigned*		_signed;
	XMLElementStereo*		_stereo;
	XMLElementSampleRate*	_sampleRate;
	XMLElementSoundFormat*	_format;

public:
	XMLElementSoundInfo();
	~XMLElementSoundInfo();

	inline XMLElementRefID*			getRefID(void) { return _refID; }
	inline XMLElementName*			getName(void) { return _name; }
	inline XMLElementFilename*		getFilename(void) { return _filename; }
	inline XMLElementStreaming*		getStreaming(void) { return _streaming; }
	inline XMLElementBigEndian*		getBigEndian(void) { return _bigEndian; }
	inline XMLElementSigned*		getSigned(void) { return _signed; }
	inline XMLElementStereo*		getStereo(void) { return _stereo; }
	inline XMLElementSampleRate*	getSampleRate(void) { return _sampleRate; }
	inline XMLElementSoundFormat*	getSoundFormat(void) { return _format; }

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* element);
};

class XMLElementSoundInfoDB : public XMLElementNode
{
protected:
	XMLElementName*			_name;

	Uint32					_numSoundInfo;
	XMLElementSoundInfo**	_soundInfo;

public:
	XMLElementSoundInfoDB();
	~XMLElementSoundInfoDB();

	inline Uint32			getNumSoundInfo(void) { return _numSoundInfo; }
	inline XMLElementName*	getName(void) { return _name; }
	XMLElementSoundInfo*		getSoundInfoAtIndex(Uint32 index);

	EOSError addChild(XMLElement* child);
	EOSError end(const xmlChar* elementName);
};

class SoundInfoDBXMLParser :	public XMLParser
{
private:
	SoundInfoDBExport	_exportDB;

	EOSError			_addName(SoundInfoDBExport& db, Char* name, Uint32& offset);

public:
	SoundInfoDBXMLParser();
	~SoundInfoDBXMLParser();

	XMLElement*		_buildXMLElementFromName(const xmlChar* name);

	EOSError		buildDB(void);
	EOSError		exportDB(Char* filename);
};

#endif /* __SOUND_INFO_XML_H__ */
