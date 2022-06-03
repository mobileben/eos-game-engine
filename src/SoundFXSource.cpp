/******************************************************************************
 *
 * File: SoundFXSource.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Sound FX Source
 * 
 *****************************************************************************/

#include "Platform.h"
#include "SoundFXSource.h"
#include "SoundFXBuffer.h"

SoundFXSource::SoundFXSource() : _name(NULL)
{
	_init();
}

SoundFXSource::~SoundFXSource()
{
}

void SoundFXSource::_init(void)
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	_refID = InvalidObjectID;
	_hwID = InvalidHWSoundFXSourceID;

	_soundFXBuffer = NULL;

	_used = false;
	_instantiated = false;
}

void SoundFXSource::setAsUsed(void)
{
	if (_used == false)
	{
		_init();

		_used = true;
	}
}

void SoundFXSource::setAsUnused(void)
{
	if (_used == true)
	{
		uninstantiate();

		_init();

		_used = true;
	}
}

void SoundFXSource::setName(Char* name)
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


void SoundFXSource::uninstantiate(void)
{
	if (_instantiated)
	{
		if (_hwID != InvalidHWSoundFXSourceID)
		{
			detachSoundFXBuffer();

#ifdef _USE_OPENAL			
			alDeleteSources(1, &_hwID);
#endif /* _USE_OPENGL */
			
			_hwID = InvalidHWSoundFXSourceID;
			_instantiated = false;
		}
	}
}

void SoundFXSource::detachSoundFXBuffer(void)
{
	_soundFXBuffer = NULL;

#ifdef _USE_OPENAL
		alSourcei(_hwID, AL_BUFFER, 0);
#endif /* _USE_OPENAL */
}

void SoundFXSource::attachSoundFXBuffer(SoundFXBuffer* soundFXBuffer)
{
	if (_used && _instantiated)
	{
		_soundFXBuffer = soundFXBuffer;

#ifdef _USE_OPENAL

		alSourcei(_hwID, AL_BUFFER, soundFXBuffer->getHWSoundFXBufferID());

#endif /* _USE_OPENAL */
	}
}

EOSError SoundFXSource::create(void)
{
	EOSError 	error = EOSErrorNone;
#ifdef _USE_OPENAL
	ALenum		alError = AL_NO_ERROR;
#endif /* _USE_OPENAL */
	if (_used)
	{
#ifdef _USE_OPENAL
		if (_hwID == InvalidHWSoundFXSourceID)
		{
			alGenSources(1, &_hwID);

			alError = alGetError();
		}
#endif /* _USE_OPENAL */
	}
	else
		error = EOSErrorUnallocated;

	if (error == EOSErrorNone)
		_instantiated = true;

	return error;
}

Boolean SoundFXSource::isPlaying(void)
{
	Boolean	playing = false;

#ifdef _USE_OPENAL
	ALint	val;

	if (_hwID != InvalidHWSoundFXSourceID)
	{
		alGetSourcei(_hwID, AL_SOURCE_STATE, &val);

		if (val == AL_PLAYING)
			val = true;
	}
#endif /* _USE_OPENAL*/

	return playing;
}

Boolean SoundFXSource::isLooping(void)
{
	Boolean	looped = false;

#ifdef _USE_OPENAL
	ALint	val;

	if (_hwID != InvalidHWSoundFXSourceID)
	{
		alGetSourcei(_hwID, AL_LOOPING, &val);

		if (val == AL_TRUE)
			val = true;
	}
#endif /* _USE_OPENAL*/

	return looped;
}

Boolean SoundFXSource::isPaused(void)
{
	Boolean	paused = false;

#ifdef _USE_OPENAL
	ALint	val;

	if (_hwID != InvalidHWSoundFXSourceID)
	{
		alGetSourcei(_hwID, AL_SOURCE_STATE, &val);

		if (val == AL_PAUSED)
			val = true;
	}
#endif /* _USE_OPENAL*/

	return false;
}

void SoundFXSource::play(Boolean loop)
{
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXSourceID)
	{
		if (loop)
			alSourcei(_hwID, AL_LOOPING, AL_TRUE);
		else
			alSourcei(_hwID, AL_LOOPING, AL_FALSE);

		alSourcePlay(_hwID);
	}
#endif /* _USE_OPENAL*/
}

void SoundFXSource::pause(void)
{
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXSourceID)
	{
		alSourcePause(_hwID);
	}
#endif /* _USE_OPENAL*/
}

void SoundFXSource::stop(void)
{
	if (_hwID != InvalidHWSoundFXSourceID)
	{
		alSourceStop(_hwID);
	}
}

