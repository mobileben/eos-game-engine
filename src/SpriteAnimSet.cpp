/******************************************************************************
 *
 * File: SpriteAnimSet.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Sprite Anim Set Object
 * 
 *****************************************************************************/

#include "SpriteAnimSet.h"
#include "Endian.h"
#include "File.h"
#include "App.h"

SpriteAnimSet::SpriteAnimSet()
{
	_used = false;

	_id = 0xFFFFFFFF;
	_name = NULL;
	_numSequences = 0;
	_sequences = NULL;
	_numKeyFrames = 0;
	_keyFrames = NULL;

	_numHotspotsInFrame = 0;
	_numHotspotAttachFrames = 0;
	_hotspotAttach = NULL;

	_numHotspotsXformInFrame = 0;
	_numHotspotXformAttachFrames = 0;
	_hotspotXformAttach = NULL;

	_nameList = NULL;
	_nameData = NULL;

	_spriteSet = NULL;

	_memUsage = 0;

	_spriteAnimSetMgr = NULL;
}

SpriteAnimSet::~SpriteAnimSet()
{
	_spriteAnimSetMgr = NULL;

	destroy();
}

EOSError SpriteAnimSet::create(ObjectID objid, const Char* name, SpriteSet* set, Uint8* data, Uint32 datasize)
{
	EOSError				error = EOSErrorNone;
	SpriteAnimSetHeader*	header = (SpriteAnimSetHeader*) data;
	Endian					endian;
	SpriteAnimSequenceRAW*	sequences;
	SpriteAnimKeyFrameRAW*	keyframes;
	Point2D*				hotspots;
	HotSpotData*			hotspotsXform;	
	Uint32					i;

	AssertWDesc(data != NULL, "SpriteAnimSet::create() NULL data.");

	if (data)
	{
		if (header->endian == 0x04030201)
			endian.switchEndian();

		_id = objid;

		destroy();

		setName(name);

		_numSequences = endian.swapUint32(header->numSequences);
		_numKeyFrames = endian.swapUint32(header->numKeyFrames);

		sequences = (SpriteAnimSequenceRAW*) (data + endian.swapUint32(header->sequences));
		keyframes = (SpriteAnimKeyFrameRAW*) (data + endian.swapUint32(header->keyFrames));
		hotspots = (Point2D*) (data + endian.swapUint32(header->hotspotAttach));
		hotspotsXform = (HotSpotData*) (data + endian.swapUint32(header->hotspotXformAttach));

		_sequences = new SpriteAnimSequence[_numSequences];
		_keyFrames = new SpriteAnimKeyFrame[_numKeyFrames];

		_numHotspotsInFrame = endian.swapUint32(header->numHotspotsInFrame);
		_numHotspotAttachFrames = endian.swapUint32(header->numHotspotAttachFrames);
		_hotspotAttach = new Point2D[_numHotspotAttachFrames * _numHotspotsInFrame];

		_numHotspotsXformInFrame = endian.swapUint32(header->numHotspotsXformInFrame);
		_numHotspotXformAttachFrames = endian.swapUint32(header->numHotspotXformAttachFrames);

		if (_numHotspotXformAttachFrames)
		{
			_hotspotXformAttach = new HotSpotData[_numHotspotXformAttachFrames * _numHotspotsXformInFrame];
			AssertWDesc(_hotspotXformAttach != NULL, "SpriteAnimSet::create() Memory failed.");
		}

		AssertWDesc(_sequences != NULL, "SpriteAnimSet::create() Memory failed.");
		AssertWDesc(_keyFrames != NULL, "SpriteAnimSet::create() Memory failed.");
		AssertWDesc(_hotspotAttach != NULL, "SpriteAnimSet::create() Memory failed.");

		if (_sequences && _keyFrames && _hotspotAttach && (_numHotspotXformAttachFrames && _hotspotXformAttach))
		{
			_memUsage = sizeof(SpriteAnimSequence) * _numSequences + sizeof(SpriteAnimKeyFrame) * _numKeyFrames + sizeof(Point2D) * (_numHotspotAttachFrames * _numHotspotsInFrame) + sizeof(HotSpotData) * (_numHotspotXformAttachFrames * _numHotspotsXformInFrame);

			for (i=0;i<_numSequences;i++)
			{
				_sequences[i].animSet = this;

				_sequences[i].numFrames = endian.swapUint32(sequences[i].numFrames);
				_sequences[i].numKeyFrames = endian.swapUint32(sequences[i].numKeyFrames);
				_sequences[i].keyFrames = &_keyFrames[endian.swapUint32(sequences[i].firstKeyFrameIndex)];
			}

			for (i=0;i<_numKeyFrames;i++)
			{
				_keyFrames[i].duration = endian.swapUint32(keyframes[i].duration);
				_keyFrames[i].spriteNumber = endian.swapUint32(keyframes[i].spriteNumber);
				_keyFrames[i].hotspotRef.x = endian.swapFloat32(keyframes[i].hotspotRef.x);
				_keyFrames[i].hotspotRef.y = endian.swapFloat32(keyframes[i].hotspotRef.y);

				_keyFrames[i].hotspotAttach = (Point2D*) ((Uint8*) _hotspotAttach + endian.swapUint32(keyframes[i].hotspotAttach));
				_keyFrames[i].hotspotXformAttach = (HotSpotData*) ((Uint8*) _hotspotXformAttach + endian.swapUint32(keyframes[i].hotspotXformAttach));

#ifdef NOT_YET
				for (j=0;j<MaxSupportedHotSpotAttach;j++)
				{
					_keyFrames[i].hotspotAttach[j].x = endian.swapFloat32(keyframes[i].hotspotAttach[j].x);
					_keyFrames[i].hotspotAttach[j].y = endian.swapFloat32(keyframes[i].hotspotAttach[j].y);
				}
#endif
			}

			for (i=0;i<_numHotspotAttachFrames * _numHotspotsInFrame;i++)
			{
				_hotspotAttach[i].x = endian.swapFloat32(hotspots[i].x);
				_hotspotAttach[i].y = endian.swapFloat32(hotspots[i].y);
			}

			for (i=0;i<_numHotspotXformAttachFrames * _numHotspotsXformInFrame;i++)
			{
				_hotspotXformAttach[i].x = endian.swapFloat32(hotspotsXform[i].x);
				_hotspotXformAttach[i].y = endian.swapFloat32(hotspotsXform[i].y);
				_hotspotXformAttach[i].rotateZ = endian.swapFloat32(hotspotsXform[i].rotateZ);
			}

			_nameData = new Char[datasize - endian.swapUint32(header->nameData)];
			_nameList = new Char*[endian.swapUint32(header->numNameOffsets)];

			if (_nameData && _nameList)
			{
				Char*					srcnames;
				Uint32*					offsets;

				_memUsage += sizeof(Char) * (datasize - endian.swapUint32(header->nameData)) + sizeof(Char*) * (endian.swapUint32(header->numNameOffsets));

				srcnames = (Char*) (data + endian.swapUint32(header->nameData));
				offsets = (Uint32*) (data + endian.swapUint32(header->nameOffsets));

				memcpy(_nameData, srcnames, datasize - endian.swapUint32(header->nameData));

				for (i=0;i<endian.swapUint32(header->numNameOffsets);i++)
					_nameList[i] = &srcnames[endian.swapUint32(offsets[i])];
			}
			else
				error = EOSErrorNoMemory;

			if (error == EOSErrorNone)
			{
				_spriteSet = set;

				if (_spriteAnimSetMgr)
					_spriteAnimSetMgr->updateUsage();
			}
		}
		else
			error = EOSErrorNoMemory;
	}
	else
		error = EOSErrorNULL;

	AssertWDesc(error == EOSErrorNone, "SpriteAnimSet::create() error");

	return error;
}

EOSError SpriteAnimSet::createFromFile(const Char* filename, ObjectID objid, const Char* name, SpriteSet* set, Uint8* buffer, Uint32 maxBufferSize)
{
	EOSError	error = EOSErrorNone;
	FileDescriptor	fdesc;
	File			file;
	
	AssertWDesc(name != NULL, "SpriteAnimSet::createFromFile() NULL name");
	
	fdesc.setFileAccessType(FileAccessTypeReadOnly);
	
	fdesc.setFilename(filename);
	
	file.setFileDescriptor(fdesc);
	
	error = file.open();
	
	if (error == EOSErrorNone)
	{
		error = file.readUint8(buffer, file.length());
		
		if (error == EOSErrorNone)
		{
			error = create(objid, name, set, buffer, file.length());
		}
		
		file.close();
	}
	else
	{
		AssertWDesc(1 == 0, "Could not load\n");
	}
	
	return error;
}

void SpriteAnimSet::destroy(void)
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	if (_sequences)
	{
		delete [] _sequences;

		_sequences = NULL;
		_numSequences = 0;
	}

	if (_keyFrames)
	{
		delete [] _keyFrames;

		_keyFrames = NULL;
		_numKeyFrames = 0;
	}

	if (_hotspotAttach)
	{
		delete _hotspotAttach;

		_hotspotAttach = NULL;

		_numHotspotsInFrame = 0;
		_numHotspotAttachFrames = 0;
	}

	if (_hotspotXformAttach)
	{
		delete _hotspotXformAttach;

		_hotspotXformAttach = NULL;

		_numHotspotsXformInFrame = 0;
		_numHotspotXformAttachFrames = 0;
	}

	if (_nameData)
	{
		delete [] _nameData;
		_nameData = NULL;
	}

	if (_nameList)
	{
		delete [] _nameList;
		_nameList = NULL;
	}

	_spriteSet = NULL;
	_memUsage = 0;

	if (_spriteAnimSetMgr)
		_spriteAnimSetMgr->updateUsage();
}

void SpriteAnimSet::setSpriteSet(SpriteSet* set)
{
	_spriteSet = set;
}

void SpriteAnimSet::setUsed(Boolean used)
{
	_used = used;

	if (_spriteAnimSetMgr)
		_spriteAnimSetMgr->updateUsage();
}

TextureAtlasSubTexture* SpriteAnimSet::findTextureAtlasSubTexture(const Char* name)
{
	if (_spriteSet)
		return _spriteSet->findTextureAtlasSubTexture(name);
	else
		return NULL;
}

void SpriteAnimSet::setRefID(ObjectID objid)
{
	_id = objid;
}

void SpriteAnimSet::setName(const Char* name)
{
	if (name)
	{
		if (_name)
		{
			delete _name;
			_name = NULL;
		}

		_name = new Char[strlen(name) + 1];

		if (_name)
		{
			strcpy(_name, name);
		}
	}
}

