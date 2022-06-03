/******************************************************************************
 *
 * File: SpriteAnimDataSet.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Parts for encapsulating and loading Sprite Anim Data sets
 * 
 *****************************************************************************/

#include "toolkit/SpriteAnimDataSet.h"
#include "SpriteAnimSet.h"
#include "Endian.h"
#include "File.h"

SpriteAnimDataSet::SpriteAnimDataSet() : _exportNames(NULL), _exportNamesOffsets(0), _exportNamesSize(0)
{
	_frame_to_time = 1.0F / 30.0F;

	_spriteSetID = 0;

	_version = 0;
	_comments = false;

	_numExportHotSpotAttach = MaxSupportedHotSpotAttach;
	_numExportHotSpotXformAttach = MaxSupportedHotSpotXformAttach;
}

SpriteAnimDataSet::~SpriteAnimDataSet()
{
	vector<Point2DContainer>::iterator hs;

	hs = _hotspotFrames.begin();

	while (hs != _hotspotFrames.begin())
	{
		if (hs->point2D)
			delete hs->point2D;

		hs++;
	}

	if (_exportNames)
	{
		delete _exportNames;
		_exportNames = NULL;
	}

	if (_exportNamesOffsets)
	{
		delete _exportNamesOffsets;
		_exportNamesOffsets = NULL;
	}

	_exportNamesSize = 0;
}

Uint32 SpriteAnimDataSet::getNumAnimSequences(void)
{
	return (Uint32) _animSequences.size();
}

vector<SpriteAnimSequenceRAW>::iterator SpriteAnimDataSet::getAnimSequenceAt(Uint32 index)
{
	return _animSequences.begin() + index;
}

Uint32 SpriteAnimDataSet::getNumKeyFrames(void)
{
	return (Uint32) _keyFrames.size();
}

vector<SpriteAnimKeyFrameFlashRAW>::iterator SpriteAnimDataSet::getKeyFrameAt(Uint32 index)
{
	return _keyFrames.begin() + index;
}

Uint32 SpriteAnimDataSet::getNumSequenceNames(void)
{
	return (Uint32) _sequenceNames.size();
}

vector<string>::iterator SpriteAnimDataSet::getSequenceNameAt(Uint32 index)
{
	return _sequenceNames.begin() + index;
}

void SpriteAnimDataSet::swapHeaderIfNecessary(FlashSpriteAnimDataHeader* header)
{
	if (header->endian == 0x04030201)
	{
		_endian.switchEndian();
	}

	header->endian = _endian.swapUint32(header->endian);
	header->version = _endian.swapUint32(header->version);
	header->sampledFrameRate = _endian.swapUint32(header->sampledFrameRate);
	header->targetFrameRate = _endian.swapUint32(header->targetFrameRate);
	header->numHotspotsInFrame = _endian.swapUint32(header->numHotspotsInFrame);
	header->numHotspotsXformInFrame = _endian.swapUint32(header->numHotspotsXformInFrame);
	header->numAnimSequences = _endian.swapUint32(header->numAnimSequences);
	header->animSequences = _endian.swapUint32(header->animSequences);
	header->numKeyFrames = _endian.swapUint32(header->numKeyFrames);
	header->keyFrames = _endian.swapUint32(header->keyFrames);
	header->numNameOffsets = _endian.swapUint32(header->numNameOffsets);
	header->nameOffsets = _endian.swapUint32(header->nameOffsets);
	header->nameData = _endian.swapUint32(header->nameData);
}

void SpriteAnimDataSet::swapAnimSequenceIfNecessary(SpriteAnimSequenceRAW* seq)
{
	//	swapHeaderIfNecessary sets the endian control
	seq->numFrames = _endian.swapUint32(seq->numFrames);
	seq->numKeyFrames = _endian.swapUint32(seq->numKeyFrames);
	seq->firstKeyFrameIndex = _endian.swapUint32(seq->firstKeyFrameIndex);
}

void SpriteAnimDataSet::swapAnimKeyFrameIfNecessary(SpriteAnimKeyFrameFlashRAW* frame)
{
	Uint32	i;

	//	swapHeaderIfNecessary sets the endian control
	frame->frameNumStart = _endian.swapUint32(frame->frameNumStart);
	frame->frameNumEnd = _endian.swapUint32(frame->frameNumEnd);
	frame->spriteNumber = _endian.swapUint32(frame->spriteNumber);

	frame->hotspotRef.x = _endian.swapFloat32(frame->hotspotRef.x);
	frame->hotspotRef.y = _endian.swapFloat32(frame->hotspotRef.y);

	for (i=0;i<_numExportHotSpotAttach;i++)
	{
		frame->hotspotAttach[i].x = _endian.swapFloat32(frame->hotspotAttach[i].x);
		frame->hotspotAttach[i].y = _endian.swapFloat32(frame->hotspotAttach[i].y);
	}

	for (i=0;i<_numExportHotSpotXformAttach;i++)
	{
		frame->hotspotXformAttach[i].x = _endian.swapFloat32(frame->hotspotXformAttach[i].x);
		frame->hotspotXformAttach[i].y = _endian.swapFloat32(frame->hotspotXformAttach[i].y);

		frame->hotspotXformAttach[i].control = _endian.swapUint32(frame->hotspotXformAttach[i].control);
		frame->hotspotXformAttach[i].scaleX = _endian.swapFloat32(frame->hotspotXformAttach[i].scaleX);
		frame->hotspotXformAttach[i].scaleY = _endian.swapFloat32(frame->hotspotXformAttach[i].scaleY);

		frame->hotspotXformAttach[i].rotateZ = _endian.swapFloat32(frame->hotspotXformAttach[i].rotateZ);

//		if (frame->hotspotXformAttach[i].control & CONTROL_FLIP_H)
//			frame->hotspotXformAttach[i].rotateZ = -frame->hotspotXformAttach[i].rotateZ;

		printf("RotateZ %f %f %f\n", frame->hotspotXformAttach[i].scaleX , frame->hotspotXformAttach[i].scaleY , frame->hotspotXformAttach[i].rotateZ );
	}
}

void SpriteAnimDataSet::swapNameOffsetIfNecessary(Uint32* offset)
{
	//	swapHeaderIfNecessary sets the endian control
	*offset = _endian.swapUint32(*offset);
}

EOSError SpriteAnimDataSet::loadAnimSequences(Uint32 num, Uint8* buffer,Uint32 size)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	SpriteAnimSequenceRAW*	sequences = (SpriteAnimSequenceRAW*) buffer;

	for (i=0;i<num;i++)
	{
		swapAnimSequenceIfNecessary(&sequences[i]);

		_animSequences.push_back(sequences[i]);
	}

	return error;
}

EOSError SpriteAnimDataSet::loadAnimKeyFrames(Uint32 num, Uint8* buffer,Uint32 size)
{
	EOSError				error = EOSErrorNone;
	Uint32					i;
	SpriteAnimKeyFrameFlashRAW*	keyframes = (SpriteAnimKeyFrameFlashRAW*) buffer;

	for (i=0;i<num;i++)
	{
		swapAnimKeyFrameIfNecessary(&keyframes[i]);

		_keyFrames.push_back(keyframes[i]);
	}

	return error;
}

EOSError SpriteAnimDataSet::loadNames(Uint32 num, Uint8* buffer,Uint32 size, Uint8* databuffer,Uint32 datasize)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	Uint32*				offsets = (Uint32*) buffer;
	string				name;
	Uint32				end;

	for (i=0;i<num;i++)
	{
		swapNameOffsetIfNecessary(&offsets[i]);
	}

	for (i=0;i<num;i++)
	{
		if (i < (num - 1))
			end = offsets[i+1];
		else
			end = datasize;

		name = string((const char*) (databuffer+offsets[i]), end - offsets[i]);

		_sequenceNames.push_back(name);
	}

	return error;
}

EOSError SpriteAnimDataSet::parseAnimData(Uint8* data, Uint32 size)
{
	EOSError					error = EOSErrorNone;
	FlashSpriteAnimDataHeader*	header = (FlashSpriteAnimDataHeader*) data;

	swapHeaderIfNecessary(header);

	_frame_to_time = (1.0F / header->sampledFrameRate) *  (header->targetFrameRate / header->sampledFrameRate);

	_numExportHotSpotAttach = header->numHotspotsInFrame;
	_numExportHotSpotXformAttach = header->numHotspotsXformInFrame;

	error = loadAnimSequences(header->numAnimSequences, data + header->animSequences, header->keyFrames - header->animSequences);

	if (error == EOSErrorNone)
		error = loadAnimKeyFrames(header->numKeyFrames, data + header->keyFrames, header->nameOffsets - header->keyFrames);

	if (error == EOSErrorNone)
		error = loadNames(header->numNameOffsets, data + header->nameOffsets, header->nameData - header->nameOffsets, data + header->nameData, size - header->nameData);

	return error;
}

EOSError SpriteAnimDataSet::addExportName(Char* name, Char** nameList, Uint32& size, Uint32& offset)
{
	Char*		newlist;
	EOSError 	error = EOSErrorNone;
	size_t		strsize;
	Boolean		found = false;
	size_t		curr = 0;

	//	Try and see if it exists already
	if (nameList)
	{
		while (curr < size)
		{
			strsize = strlen(&(*nameList)[curr]);

			if (!strcmp(name, &(*nameList)[curr]))
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

		newlist = new Char[size + strsize + 1];

		if (newlist)
		{
			if ((*nameList))
			{
				memcpy(newlist, (*nameList), size);
				delete (*nameList);
			}

			strcpy(&newlist[size], name);
			(*nameList) = newlist;
			offset = size;
			size += (Uint32) strsize + 1;
		}
		else
			error = EOSErrorNoMemory;
	}

	return error;
}

Uint32 SpriteAnimDataSet::addHotspotFrame(Point2D* hotspots)
{
	vector<Point2DContainer>::iterator	hs;
	Boolean									found = false;
	Sint32									index = 0;
	Uint32									i;

	hs = _hotspotFrames.begin();

	while (hs != _hotspotFrames.end())
	{
		found = true;

		for (i=0;i<_numExportHotSpotAttach;i++)
		{
			if (hotspots[i].x != hs->point2D[i].x || hotspots[i].y != hs->point2D[i].y)
			{
				found = false;
				break;
			}
		}

		if (found)
			break;

		index++;
		hs++;
	}

	if (!found)
	{
		Point2DContainer	container;

		index = (Sint32) _hotspotFrames.size();

		container.point2D = new Point2D[_numExportHotSpotAttach];

		if (container.point2D)
		{
			for (i=0;i<_numExportHotSpotAttach;i++)
			{
				container.point2D[i] = hotspots[i];
			}
		}
		else
		{
			fprintf(stderr, "Memory allocation error\n");
		}

		_hotspotFrames.push_back(container);
	}

	return (sizeof(Point2D) * _numExportHotSpotAttach * index);
}

Uint32 SpriteAnimDataSet::addHotspotXformFrame(HotSpotDataRAW* hotspots)
{
	vector<HotSpotDataContainer>::iterator	hs;
	Boolean									found = false;
	Sint32									index = 0;
	Uint32									i;

	hs = _hotspotXformFrames.begin();

	while (hs != _hotspotXformFrames.end())
	{
		found = true;

		for (i=0;i<_numExportHotSpotXformAttach;i++)
		{
			if (hotspots[i].x != hs->data[i].x || hotspots[i].y != hs->data[i].y || hotspots[i].rotateZ != hs->data[i].rotateZ)
			{
				found = false;
				break;
			}
		}

		if (found)
			break;

		index++;
		hs++;
	}

	if (!found)
	{
		HotSpotDataContainer	container;

		index = (Sint32) _hotspotXformFrames.size();

		container.data = new HotSpotData[_numExportHotSpotXformAttach];

		if (container.data)
		{
			for (i=0;i<_numExportHotSpotXformAttach;i++)
			{
				container.data[i].x = hotspots[i].x;
				container.data[i].y = hotspots[i].y;
				container.data[i].rotateZ = hotspots[i].rotateZ;
			}
		}
		else
		{
			fprintf(stderr, "Memory allocation error\n");
		}

		_hotspotXformFrames.push_back(container);
	}

	return (sizeof(HotSpotData) * _numExportHotSpotXformAttach * index);
}

EOSError SpriteAnimDataSet::loadDataSet(Char* filename)
{
	EOSError		error = EOSErrorNone;
	FileDescriptor	fd;
	File			file;
	Uint8*			buffer;
	Uint32			size;
	wstring			filenameUTF16(filename, filename + strlen(filename));

	if (filenameUTF16.c_str())
	{
		fd.setFilename((UTF16*) filenameUTF16.c_str());
		fd.setFileAccessType(FileAccessTypeReadOnly);

		file.setFileDescriptor(fd);

		error = file.open();

		if (error == EOSErrorNone)
		{
			size = file.length();

			buffer = new Uint8[size];

			if (buffer)
			{
				error = file.readUint8(buffer, size);

				if (error == EOSErrorNone)
					error = parseAnimData(buffer, size);

				delete [] buffer;
			}
			else
			{
				error = EOSErrorNoMemory;
				fprintf(stderr, "Memory allocation error.\n");
			}

			file.close();
		}
		else
		{
			error = EOSErrorResourceNotOpened;
			fprintf(stderr, "Could not open file %s\n", filename);
		}
	}
	else
	{
		error = EOSErrorNoMemory;
		fprintf(stderr, "Memory allocation error.\n");
	}

	return error;
}

EOSError SpriteAnimDataSet::exportDataSetAsText(Char* filename, Char* outprefix)
{
	EOSError										error = EOSErrorNone;
	vector<SpriteAnimSequenceRAW>::iterator			sequence;
	vector<SpriteAnimKeyFrameFlashRAW>::iterator	keyframe;
	FileDescriptor									fd;
	File											file;
	char											str[256];
	Uint32											cnt;
	wstring											filenameUTF16(filename, filename + strlen(filename));
	Uint32											i;

	fd.setFilename((UTF16*) filenameUTF16.c_str());
	fd.setFileAccessType(FileAccessTypeWriteOnly);

	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		sprintf_s(str, sizeof(str), "const SpriteAnimSequenceRAW\t%s_anim_seq[%d] =\n{\n", outprefix, _animSequences.size()); 
		file.writeChar(str, (Uint32) strlen(str));

		sequence = _animSequences.begin();
		cnt = 0;

		while (sequence != _animSequences.end())
		{
			if (_comments)
				sprintf_s(str, sizeof(str), "\t{\t// %s\n", _sequenceNames[cnt].c_str());
			else
				sprintf_s(str, sizeof(str), "\t{\n");

			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", sequence->numFrames);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// numFrames");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", sequence->numKeyFrames);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// numKeyFrames");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", sequence->firstKeyFrameIndex);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// firstKeyFrameIndex");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t},\n");
			file.writeChar(str, (Uint32) strlen(str));

			sequence++;
			cnt++;
		}

		sprintf_s(str, sizeof(str), "};\n");
		file.writeChar(str, (Uint32) strlen(str));

		sprintf_s(str, sizeof(str), "\nconst SpriteAnimKeyFrameRAW\t%s_anim_key_frames[%d] =\n{\n", outprefix, _keyFrames.size()); 
		file.writeChar(str, (Uint32) strlen(str));

		keyframe = _keyFrames.begin();
		cnt = 0;

		while (keyframe != _keyFrames.end())
		{
			if (_comments)
				sprintf_s(str, sizeof(str), "\t{\t// %d\n", cnt);
			else
				sprintf_s(str, sizeof(str), "\t{\n");

			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", (Uint32) (1000000.0 * (keyframe->frameNumEnd - keyframe->frameNumStart + 1.0) * _frame_to_time));
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// duration");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t%d,", keyframe->spriteNumber);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// spriteNumber");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			sprintf_s(str, sizeof(str), "\t\t{ %f, %f },", keyframe->hotspotRef.x, keyframe->hotspotRef.y);
			file.writeChar(str, (Uint32) strlen(str));

			if (_comments)
			{
				sprintf_s(str, sizeof(str), "\t// hotspotRef");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\n");
			file.writeChar(str, (Uint32) strlen(str));

			for (i=0;i<_numExportHotSpotAttach;i++)
			{
				sprintf_s(str, sizeof(str), "\t\t{ %f, %f },", keyframe->hotspotAttach[i].x, keyframe->hotspotAttach[i].y);
				file.writeChar(str, (Uint32) strlen(str));

				if (_comments)
				{
					sprintf_s(str, sizeof(str), "\t// hotspotAttach[%d]", i);
					file.writeChar(str, (Uint32) strlen(str));
				}

				sprintf_s(str, sizeof(str), "\n");
				file.writeChar(str, (Uint32) strlen(str));
			}

			sprintf_s(str, sizeof(str), "\t},\n");
			file.writeChar(str, (Uint32) strlen(str));

			keyframe++;
			cnt++;
		}

		sprintf_s(str, sizeof(str), "};\n");
		file.writeChar(str, (Uint32) strlen(str));
	}
	else
	{
		error = EOSErrorResourceOpen;
		fprintf(stderr, "Could not open file %s\n", filename);
	}

	return error;
}

EOSError SpriteAnimDataSet::exportDataSetAsBIN(Char* filename)
{
	EOSError							error = EOSErrorNone;
	vector<SpriteAnimSequenceRAW>::iterator		sequence;
	vector<SpriteAnimKeyFrameFlashRAW>::iterator	keyframe;
	vector<Point2DContainer>::iterator		hs;
	vector<HotSpotDataContainer>::iterator		hsXform;
	FileDescriptor							fd;
	File									file;
	wstring									filenameUTF16(filename, filename + strlen(filename));
	Uint32									cnt;

	fd.setFilename((UTF16*) filenameUTF16.c_str());
	fd.setFileAccessType(FileAccessTypeWriteOnly);

	file.setFileDescriptor(fd);

	error = file.open();

	if (error == EOSErrorNone)
	{
		Uint32					offset;
		Uint32*					offsets = NULL;
		Uint32 					nameOffset = 0;
		Uint32					dataOffset = 0;
		SpriteAnimSequenceRAW*	sequences = NULL;
		SpriteAnimKeyFrameRAW*	keyframes = NULL;
		Point2D*				hotspots = NULL;
		HotSpotData*			hotspotsXform = NULL;
		SpriteAnimSetHeader		header;
		vector<string>::iterator	name;
		char					tmpName[EOS_ASSET_NAME_MAX_LENGTH] = "TEST";

		header.endian = 0x01020304;
		header.version = _version;

		header.spriteSetID = _spriteSetID;

		error = addExportName((Char*) tmpName, &_exportNames, _exportNamesSize, offset);

		header.nameOffset = offset;

		error = addExportName((Char*) tmpName, &_exportNames, _exportNamesSize, offset);

		header.spriteSetNameOffset = offset;

		dataOffset = sizeof(SpriteAnimSetHeader);

		sequences = new SpriteAnimSequenceRAW[_animSequences.size()];
		keyframes = new SpriteAnimKeyFrameRAW[_keyFrames.size()];
		offsets = new Uint32[_sequenceNames.size()];

		header.numSequences = (Uint32) _animSequences.size();
		header.sequences = dataOffset;

		if (sequences && keyframes && offsets)
		{
			sequence = _animSequences.begin();
			cnt = 0;

			while (sequence != _animSequences.end())
			{
				sequences[cnt].numFrames = sequence->numFrames;
				sequences[cnt].numKeyFrames = sequence->numKeyFrames;
				sequences[cnt].firstKeyFrameIndex = sequence->firstKeyFrameIndex;

				dataOffset += sizeof(SpriteAnimSequenceRAW);

				sequence++;
				cnt++;
			}

			header.numKeyFrames = (Uint32) _keyFrames.size();
			header.keyFrames = dataOffset;

			keyframe = _keyFrames.begin();
			cnt = 0;

			while (keyframe != _keyFrames.end())
			{
				keyframes[cnt].duration = (Uint32) (1000000.0 * ((Float32) keyframe->frameNumEnd - (Float32) keyframe->frameNumStart + 1.0F) * _frame_to_time);
				keyframes[cnt].spriteNumber = keyframe->spriteNumber;

				//	If needed, add more hotspot ref info here
				keyframes[cnt].hotspotRef.x = keyframe->hotspotRef.x;
				keyframes[cnt].hotspotRef.y = keyframe->hotspotRef.y;

				keyframes[cnt].hotspotAttach = addHotspotFrame(keyframe->hotspotAttach);

				keyframes[cnt].hotspotXformAttach = addHotspotXformFrame(keyframe->hotspotXformAttach);

				dataOffset += sizeof(SpriteAnimKeyFrameRAW);

				keyframe++;
				cnt++;
			}

			header.numHotspotsInFrame = _numExportHotSpotAttach;
			header.numHotspotAttachFrames = (Uint32) _hotspotFrames.size();
			header.hotspotAttach = dataOffset;

			hotspots = new Point2D[_numExportHotSpotAttach * _hotspotFrames.size()];
			cnt = 0;

			if (hotspots)
			{
				hs = _hotspotFrames.begin();

				while (hs != _hotspotFrames.end())
				{
					memcpy(&hotspots[cnt * _numExportHotSpotAttach], hs->point2D, _numExportHotSpotAttach * sizeof(Point2D));

					dataOffset += _numExportHotSpotAttach * sizeof(Point2D);

					cnt++;
					hs++;
				}
			}

			header.numHotspotsXformInFrame = _numExportHotSpotXformAttach;
			header.numHotspotXformAttachFrames = (Uint32) _hotspotXformFrames.size();
			header.hotspotXformAttach = dataOffset;

			hotspotsXform = new HotSpotData[_numExportHotSpotXformAttach * _hotspotXformFrames.size()];
			cnt = 0;

			if (hotspotsXform)
			{
				hsXform = _hotspotXformFrames.begin();

				while (hsXform != _hotspotXformFrames.end())
				{
					memcpy(&hotspotsXform[cnt * _numExportHotSpotXformAttach], hsXform->data, _numExportHotSpotXformAttach * sizeof(HotSpotData));

					dataOffset += _numExportHotSpotXformAttach * sizeof(HotSpotData);

					cnt++;
					hsXform++;
				}
			}

			header.numNameOffsets = (Uint32) _sequenceNames.size() + 2;	//	Two is for the name and sprite set name

			header.nameOffsets = dataOffset;
			dataOffset += sizeof(Uint32) * header.numNameOffsets;

			header.nameData = dataOffset;

			name = _sequenceNames.begin();
			cnt = 0;

			while (name != _sequenceNames.end())
			{
				error = addExportName((Char*) name->c_str(), &_exportNames, _exportNamesSize, offset);
				offsets[cnt] = offset;

				if (error != EOSErrorNone)
					break;

				name++;
				cnt++;
			}

			//	Now write out all the data
			file.writeUint8((Uint8*) &header, sizeof(SpriteAnimSetHeader));
			file.writeUint8((Uint8*) sequences, (Uint32) sizeof(SpriteAnimSequenceRAW) * _animSequences.size());
			file.writeUint8((Uint8*) keyframes, (Uint32) sizeof(SpriteAnimKeyFrameRAW) * _keyFrames.size());
			file.writeUint8((Uint8*) hotspots, (Uint32) sizeof(Point2D) * _hotspotFrames.size() * _numExportHotSpotAttach);
			file.writeUint8((Uint8*) hotspotsXform, (Uint32) sizeof(HotSpotData) * _hotspotXformFrames.size() * _numExportHotSpotXformAttach);
			file.writeUint8((Uint8*) offsets, header.numNameOffsets * sizeof(Uint32));
			file.writeUint8((Uint8*) _exportNames, _exportNamesSize);

		}
		else
		{
			error = EOSErrorNoMemory;
			fprintf(stderr, "Memory allocation error\n");
		}

		file.close();

		if (sequences)
			delete sequences;

		if (keyframes)
			delete keyframes;

		if (hotspots)
			delete hotspots;

		if (offsets)
			delete offsets;
	}
	else
	{
		error = EOSErrorResourceOpen;
		fprintf(stderr, "Could not open file %s\n", filename);
	}

	return error;
}

