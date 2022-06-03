/******************************************************************************
 *
 * File: SoundFXBuffer.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Sound FX
 * 
 *****************************************************************************/

#include "Platform.h"
#include "SoundFXBuffer.h"
#include "AudioManager.h"

AudioManager*	SoundFXBuffer::_audioMgr = NULL;

SoundFXBuffer::SoundFXBuffer() : _name(NULL)
{
	_init();
}

SoundFXBuffer::~SoundFXBuffer()
{
}

void SoundFXBuffer::_init(void)
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	_refID = InvalidObjectID;
	_hwID = InvalidHWSoundFXBufferID;

	_used = false;
	_instantiated = false;
	_streaming = false;
	
#ifdef _USE_APPLE_AUDIOQUEUE
	
	_useAudioQueue = false;
	_needsFree = false;
	
	memset(&_streamBasicDesc, 0, sizeof(_streamBasicDesc));
	
	_audioFileID = NULL;
	_audioQueueBuffer = NULL;
	_audioQueueBufferSize = 0;
	_maxPacketSize = 0;
	
#endif /* _USE_APPLE_AUDIOQUEUE */
	
#ifdef _USE_OPENAL
	_alFormat = 0;
	_alFreq = 0;
#endif /* _USE_OPENAL */

	_memUsage = 0;
}

void SoundFXBuffer::setAsUsed(void)
{
	if (_used == false)
	{
		_init();

		_used = true;

		if (_audioMgr)
			_audioMgr->updateUsage();
	}
}

void SoundFXBuffer::setAsUnused(void)
{
	if (_used == true)
	{
		uninstantiate();

		_init();

		_used = false;

		if (_audioMgr)
			_audioMgr->updateUsage();
	}
}

void SoundFXBuffer::setName(const Char* name)
{
	if (_name)
		delete _name;

	_name = NULL;

	if (name)
	{
		_name = new Char[strlen(name) + 1];

		if (_name)
		{
			strcpy(_name, name);
		}
	}
}

void SoundFXBuffer::setStreaming(Boolean streaming)
{
	_streaming = streaming;
}

Boolean SoundFXBuffer::isStreaming(void)
{
	return _streaming;
}


#ifdef _USE_APPLE_AUDIOQUEUE

void SoundFXBuffer::setUseAudioQueue(Boolean use)
{
	_useAudioQueue = use;
}

Boolean SoundFXBuffer::usingAudioQueueu(void)
{
	return _useAudioQueue;
}

void SoundFXBuffer::setAudioFileID(AudioFileID fid)
{
	_audioFileID = fid;
}

AudioFileID SoundFXBuffer::getAudioFileID(void)
{
	return _audioFileID;
}

void SoundFXBuffer::setMaxPacketSize(Uint32 size)
{
	_maxPacketSize = size;
}

Uint32 SoundFXBuffer::getMaxPacketSize(void)
{
	return _maxPacketSize;
}

void SoundFXBuffer::setAudioStreamBasicDescription(AudioStreamBasicDescription& desc)
{
	_streamBasicDesc = desc;
}

AudioStreamBasicDescription* SoundFXBuffer::getAudioStreamBasicDescription(void)
{
	return &_streamBasicDesc;
}

void SoundFXBuffer::setAudioQueueBuffer(Uint8* buffer, Uint32 bufferSize, Boolean needsToFree)
{
	if (_audioQueueBuffer && _needsFree)
		delete _audioQueueBuffer;
	
	_needsFree = needsToFree;
	_audioQueueBuffer = buffer;
	_audioQueueBufferSize = bufferSize;
	
	_memUsage = bufferSize;
}

Uint8* SoundFXBuffer::getAudioQueueBuffer(void)
{
	return _audioQueueBuffer;
}

Uint32 SoundFXBuffer::getAudioQueueBufferSize(void)
{
	return _audioQueueBufferSize;
}

void SoundFXBuffer::buildAudioQueueBuffer(Uint8* buffer, Uint32 bufferSize)
{
	if (_audioQueueBuffer && _needsFree)
		delete _audioQueueBuffer;

	_memUsage = 0;

	_audioQueueBuffer = new Uint8[bufferSize];

	AssertWDesc(_audioQueueBuffer != NULL, "SoundFXBuffer::buildAudioQueueBuffer() no memory");

	if (_audioQueueBuffer)
	{
		_memUsage = bufferSize;

		memcpy(_audioQueueBuffer, buffer, bufferSize);
		_audioQueueBufferSize = bufferSize;
		_needsFree = true;
	}
	else
	{
		_needsFree = false;
		_audioQueueBuffer = NULL;
		_audioQueueBufferSize = 0;
	}
}

#endif /* _USE_APPLE_AUDIOQUEUE */

void SoundFXBuffer::uninstantiate(void)
{
	if (_instantiated)
	{
		_memUsage = 0;

#ifdef _USE_APPLE_AUDIOQUEUE
		if (_audioQueueBuffer && _needsFree)
			delete _audioQueueBuffer;

		_needsFree = false;
		_audioQueueBuffer = NULL;
		_audioQueueBufferSize = 0;

		if (_audioFileID != NULL)
		{
			AudioFileClose(_audioFileID);
			_audioFileID = NULL;
		}
		
#endif /* _USE_APPLE_AUDIOQUEUE */
		
		if (_hwID != InvalidHWSoundFXBufferID)
		{
#ifdef _USE_OPENAL			
			ALenum		alError = AL_NO_ERROR;

			alDeleteBuffers(1, &_hwID);

			alError = alGetError();
#endif /* _USE_OPENGL */
			
			_hwID = InvalidHWSoundFXBufferID;
			_instantiated = false;
		}

		if (_audioMgr)
			_audioMgr->updateUsage();
	}
}

#ifdef _USE_OPENAL

ALenum SoundFXBuffer::convertSoundFormatToALFormat(SoundFormat format)
{
	ALenum	alFormat = AL_FORMAT_MONO8;

	switch (format)
	{
		case SoundFormatPCM_MONO8:
			alFormat = AL_FORMAT_MONO8;
			break;

		case SoundFormatPCM_MONO16:
			alFormat = AL_FORMAT_MONO16;
			break;

		case SoundFormatPCM_STEREO8:
			alFormat = AL_FORMAT_STEREO8;
			break;

		case SoundFormatPCM_STEREO16:
			alFormat = AL_FORMAT_STEREO16;
			break;

		default:
			AssertWDesc(1 == 0, "SoundFXBuffer::convertSoundFormatToALFormat() Format not supported.");
			break;
	}

	return alFormat;
}

EOSError SoundFXBuffer::createFromPCM(ALvoid* buffer, ALenum format, ALsizei freq, ALsizei bufferSize)
{
	EOSError	error = EOSErrorNone;
	ALenum		alError = AL_NO_ERROR;

	if (_used == true)
	{
		alGenBuffers(1, &_hwID);
	}

	alError = alGetError();

	if (alError == AL_NO_ERROR)
	{
		_alFormat = format;
		_alFreq = freq;

		_memUsage = bufferSize;

		alBufferData(_hwID, format, buffer, bufferSize, freq);

		alError = alGetError();
	}

	switch (alError)
	{
		case AL_INVALID_NAME:
			error = EOSErrorResourceDoesNotExist;
			break;

		case AL_INVALID_ENUM:
		case AL_INVALID_VALUE:
			error = EOSErrorIllegalArguments;
			break;

		case AL_INVALID_OPERATION:
			error = EOSErrorNoMemory;
			break;

		case AL_OUT_OF_MEMORY:
			error = EOSErrorNoMemory;
			break;
	}

	if (error == EOSErrorNone)
		_instantiated = true;

	return error;
}

#endif /* _USE_OPENAL */

