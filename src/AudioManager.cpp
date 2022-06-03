/******************************************************************************
 *
 * File: AudioManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Audio Manager
 * 
 *****************************************************************************/

#include "Platform.h"
#include "AudioManager.h"
#include "File.h"
#include "SoundFXBufferFileLoadTask.h"
#include "TaskManager.h"

#ifdef _PLATFORM_IPHONE
#include "AudioToolbox/AudioToolbox.h"
#include "AudioToolbox/AudioServices.h"

static void AudioManagerInterruptListener(void* inClientData, UInt32 inInterruptionState)
{
	AudioManager*	mgr = (AudioManager*) inClientData;

	switch (inInterruptionState)
	{
		case kAudioSessionBeginInterruption:
			mgr->suspend();
			break;
			
		case kAudioSessionEndInterruption:
			mgr->resume();
			break;
	}
}

#endif /* _PLATFORM_IPHONE */

AudioManager::AudioManager() : _maxSoundFXBuffer(0), _numSoundFXBuffer(0), _soundFXBuffer(NULL), _maxSoundFX(0), _numSoundFX(0), _soundFX(NULL), 
								_maxMusicFX(0), _numMusicFX(0), _musicFX(NULL), _sfxEnabled(true), _musicEnabled(true),
								_soundInfoNeedsFree(false), _numSoundInfo(0), _soundInfo(NULL), _suspended(false), _soundFXBufferTotalMemUsage(0)
{
#ifdef _PLATFORM_IPHONE
	UInt32 other;
	UInt32 otherSize = sizeof(other);
#endif /* _PLATFORM_IPHONE */
	
#ifdef _USE_OPENAL
	_device = NULL;
#endif /* _USE_OPENAL */
	
	SoundFX::setAudioManager(this);
	
#ifdef _PLATFORM_IPHONE
	//	First grab an audio session that let's other audio play concurrent with ours!
	_category = kAudioSessionCategory_AmbientSound;
	
	AudioSessionInitialize(NULL, NULL, AudioManagerInterruptListener, this);
	AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(_category), &_category);
	AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &otherSize, &other);
	
	if (other == 0)
		_otherAudioPlayingAtStart = false;
	else
		_otherAudioPlayingAtStart = true;

	AudioSessionSetActive(true);
	
#endif /* _PLATFORM_IPHONE */
}

AudioManager::~AudioManager()
{
	deallocateMusicFXPool();
	deallocateSoundFXPool();
	deallocateSoundFXBufferPool();

	destroySoundInfoDatabase();

	_soundInfoNeedsFree = false;
	_soundInfo = NULL;
	_numSoundInfo = 0;

	destroy();
}

void AudioManager::init(void)
{
#ifdef _USE_OPENAL
	EOSError	error = EOSErrorNone;
	
	_device = alcOpenDevice(NULL);

	if (_device)
	{
		_context = alcCreateContext(_device, NULL);

		if (_context)
			alcMakeContextCurrent(_context);
		else
			error = EOSErrorResourceNotAvailable;
	}
	else
		error = EOSErrorResourceNotAvailable;
#endif /* _USE_OPENAL */
}

void AudioManager::destroy(void)
{
#ifdef _USE_OPENAL
	if (_context)
		alcDestroyContext(_context);

	_context = NULL;

	if (_device)
		alcCloseDevice(_device);

	_device = NULL;

#endif /* _USE_OPENAL */
}

void AudioManager::deallocateSoundFXBufferPool(void)
{
	Uint32	i;

	if (_soundFXBuffer)
	{
		for (i=0;i<_maxSoundFXBuffer;i++)
		{
			if (_soundFXBuffer[i].isUsed())
			{
				_soundFXBuffer[i].setAsUnused();
			}
		}

		delete [] _soundFXBuffer;
		_soundFXBuffer = NULL;
	}

	_maxSoundFXBuffer = 0;
	_numSoundFXBuffer = 0;

	_soundFXBufferTotalMemUsage = 0;
}

EOSError AudioManager::allocateSoundFXBufferPool(Uint32 sfxmax)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	AssertWDesc(sfxmax != 0, "sfxmax must be != 0");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateSoundFXBufferPool();

	_soundFXBuffer = new SoundFXBuffer[sfxmax];

	AssertWDesc(_soundFXBuffer != NULL, "_soundFXBuffer is NULL");

	if (_soundFXBuffer != NULL)
	{
		for (i=0;i<sfxmax;i++)
		{
			_soundFXBuffer[i].setAudioManager(this);
		}

		_maxSoundFXBuffer = sfxmax;
		_numSoundFXBuffer = 0;
	}
	else
	{
		deallocateSoundFXBufferPool();

		error = EOSErrorNoMemory;
	}

	return error;
}

void AudioManager::deallocateSoundFXPool(void)
{
	Uint32	i;
	
	if (_soundFX)
	{
		for (i=0;i<_maxSoundFX;i++)
		{
			if (_soundFX[i].isUsed())
			{
				_soundFX[i].setAsUnused();
			}
		}
		
		delete [] _soundFX;
		_soundFX = NULL;
	}

	_maxSoundFX = 0;
	_numSoundFX = 0;
}

EOSError AudioManager::allocateSoundFXPool(Uint32 sourcemax)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	AssertWDesc(sourcemax != 0, "sourcemax must be != 0");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateSoundFXPool();

	_soundFX = new SoundFX[sourcemax];

	AssertWDesc(_soundFX != NULL, "_soundFX is NULL");

	if (_soundFX != NULL)
	{
		for (i=0;i<sourcemax;i++)
			_soundFX[i].setSoundType(SoundFX::SoundTypeSFX);
		
		_maxSoundFX = sourcemax;
		_numSoundFX = 0;
	}
	else
	{
		deallocateSoundFXPool();

		error = EOSErrorNoMemory;
	}

	return error;
}


void AudioManager::deallocateMusicFXPool(void)
{
	Uint32	i;
	
	if (_musicFX)
	{
		for (i=0;i<_maxMusicFX;i++)
		{
			if (_musicFX[i].isUsed())
			{
				_musicFX[i].setAsUnused();
			}
		}
		
		delete [] _musicFX;
		_musicFX = NULL;
	}
	
	_maxMusicFX = 0;
	_numMusicFX = 0;
}

EOSError AudioManager::allocateMusicFXPool(Uint32 sourcemax)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;
	
	AssertWDesc(sourcemax != 0, "sourcemax must be != 0");
	
	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateMusicFXPool();
	
	_musicFX = new MusicFX[sourcemax];
	
	AssertWDesc(_musicFX != NULL, "_musicFX is NULL");
	
	if (_musicFX != NULL)
	{
		for (i=0;i<sourcemax;i++)
			_musicFX[i].setSoundType(SoundFX::SoundTypeMusic);
		
		_maxMusicFX = sourcemax;
		_numMusicFX = 0;
	}
	else
	{
		deallocateMusicFXPool();
		
		error = EOSErrorNoMemory;
	}
	
	return error;
}

void AudioManager::destroySoundInfoDatabase(void)
{
	Uint32	i;

	if (_soundInfoNeedsFree)
	{
		if (_soundInfo)
		{
			for (i=0;i<_numSoundInfo;i++)
			{
				if (_soundInfo[i].name)
					delete _soundInfo[i].name;

				if (_soundInfo[i].filename)
					delete _soundInfo[i].filename;
			}

			delete _soundInfo;
		}
	}

	_soundInfoNeedsFree = false;
	_numSoundInfo = 0;
	_soundInfo = NULL;
}

EOSError AudioManager::createSoundInfoDatabase(const Char* filename, Uint8* buffer, Uint32 buffersize)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	FileDescriptor		fdesc;
	File				file;
	Endian				endian;
	SoundInfoDBHeader*	header;
	SoundInfoRAW*		raws;
	Char*				names;
	Char*				str;

	AssertWDesc(filename != NULL, "AudioManager::createSoundInfoDatabase() NULL name");

	destroySoundInfoDatabase();

	fdesc.setFileAccessType(FileAccessTypeReadOnly);

	fdesc.setFilename(filename);

	file.setFileDescriptor(fdesc);

	error = file.open();

	if (error == EOSErrorNone)
	{
		if (file.length() < (Sint32) buffersize)
		{
			_soundInfoNeedsFree = true;

			error = file.readUint8(buffer, file.length());

			if (error == EOSErrorNone)
			{
				header = (SoundInfoDBHeader*) buffer;

				if (header->endian == 0x04030201)
					endian.switchEndian();

				_numSoundInfo = endian.swapUint32(header->numSoundInfo);;

				raws = (SoundInfoRAW*) (buffer + endian.swapUint32(header->soundInfo));
				names = (Char*) (buffer + endian.swapUint32(header->names));

				_soundInfo = new SoundInfo[_numSoundInfo];

				memset(_soundInfo, 0, sizeof(SoundInfo) * _numSoundInfo);

				for (i=0;i<_numSoundInfo;i++)
				{
					_soundInfo[i].refID = endian.swapUint32(raws[i].refID);

					str = &names[endian.swapUint32(raws[i].nameOffset)];

					_soundInfo[i].name = new Char[strlen(str) + 1];

					if (_soundInfo[i].name)
					{
						strcpy(_soundInfo[i].name, str);
					}
					else
						error = EOSErrorNoMemory;

					str = &names[endian.swapUint32(raws[i].filenameOffset)];

					_soundInfo[i].filename = new Char[strlen(str) + 1];

					if (_soundInfo[i].filename)
					{
						strcpy(_soundInfo[i].filename, str);
					}
					else
						error = EOSErrorNoMemory;

					//	These are bytes
					if (raws[i].streaming)
						_soundInfo[i].streaming = true;
					else
						_soundInfo[i].streaming = false;

					//	These are bytes
					if (raws[i].bigEndian)
						_soundInfo[i].bigEndian = true;
					else
						_soundInfo[i].bigEndian = false;

					//	These are bytes
					if (raws[i].isSigned)
						_soundInfo[i].isSigned = true;
					else
						_soundInfo[i].isSigned = false;

					//	These are bytes
					if (raws[i].stereo)
						_soundInfo[i].stereo = true;
					else
						_soundInfo[i].stereo = false;

					_soundInfo[i].sampleRate = endian.swapUint32(raws[i].sampleRate);
					_soundInfo[i].soundFormat = endian.swapUint32(raws[i].soundFormat);

#ifdef _PLATFORM_IPHONE
					switch (_soundInfo[i].soundFormat)
					{
						case SoundFormatMP3:
						case SoundFormatCAF:
						case SoundFormatAAC:
							_soundInfo[i].useAudioQueue = true;	
							break;

						default:
							_soundInfo[i].useAudioQueue = false;	
							break;
					}
#else
					_soundInfo[i].useAudioQueue = false;	
#endif /* _PLATFORM_IPHONE */

					if (error != EOSErrorNone)
						break;
				}
			}
			else
				error = EOSErrorResourceRead;
		}
		else
			error = EOSErrorBufferSize;
	}

	return error;
}

SoundInfo* AudioManager::findSoundInfoByRefID(ObjectID objid)
{
	Uint32			i;
	SoundInfo*	info = NULL;

	for (i=0;i<_numSoundInfo;i++)
	{
		if (objid == _soundInfo[i].refID)
		{
			info = &_soundInfo[i];
			break;
		}
	}

	return info;
}

SoundInfo* AudioManager::findSoundInfoByRefName(const Char* name)
{
	Uint32			i;
	SoundInfo*	info = NULL;

	if (name)
	{
		for (i=0;i<_numSoundInfo;i++)
		{
			if (_soundInfo[i].name)
			{
				if (!strcmp(_soundInfo[i].name, name))
				{
					info = &_soundInfo[i];
					break;
				}
			}
		}
	}

	return info;
}

void AudioManager::queueSoundFXBufferLoad(TaskManager* loader, Uint8* buffer, Uint32 buffersize, const Char* name)
{
	SoundInfo*					info = findSoundInfoByRefName(name);
	FileDescriptor				fdesc;

	AssertWDesc(info != NULL, "AudioManager::queueSoundFXBufferLoad() no SoundInfo found");

	if (info)
	{
		SoundFXBufferFileLoadTask*	sfxtask = new SoundFXBufferFileLoadTask;

		fdesc.setFilename(info->filename);

		sfxtask->setSoundInfo(*info);
		sfxtask->setFileDescriptor(fdesc);
		sfxtask->setLoadBuffer(buffer, buffersize, false);

		loader->addTask(sfxtask);
	}
}

SoundFXBuffer* AudioManager::getFreeSoundFXBuffer(void)
{
	SoundFXBuffer*	soundFXBuffer = NULL;
	Uint32		i;

	for (i=0;i<_maxSoundFXBuffer;i++)
	{
		if (_soundFXBuffer[i].isUsed() == false)
		{
			soundFXBuffer = &_soundFXBuffer[i];
			soundFXBuffer->setAsUsed();
			break;
		}
	}

	return soundFXBuffer;
}

SoundFXBuffer* AudioManager::findSoundFXBufferFromRefID(ObjectID refID)
{
	SoundFXBuffer*	soundFXBuffer = NULL;
	Uint32		i;

	if (refID == InvalidObjectID)
		return soundFXBuffer;

	for (i=0;i<_maxSoundFXBuffer;i++)
	{
		if (_soundFXBuffer[i].isUsed() && _soundFXBuffer[i].getRefID() == refID)
		{
			soundFXBuffer = &_soundFXBuffer[i];
			break;
		}
	}

	return soundFXBuffer;
}

SoundFXBuffer* AudioManager::findSoundFXBufferFromName(const Char* name)
{
	SoundFXBuffer*	soundFXBuffer = NULL;
	Uint32		i;

	if (name)
	{
		for (i=0;i<_maxSoundFXBuffer;i++)
		{
			if (_soundFXBuffer[i].isUsed() && _soundFXBuffer[i].getName())
			{
				if (!strcmp(_soundFXBuffer[i].getName(), name))
				{
					soundFXBuffer = &_soundFXBuffer[i];
					break;
				}
			}
		}
	}

	return soundFXBuffer;
}

SoundFX* AudioManager::getFreeSoundFX(void)
{
	SoundFX*	soundFX = NULL;
	Uint32		i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() == false)
		{
			soundFX = &_soundFX[i];
			soundFX->setAsUsed();
			break;
		}
	}

	return soundFX;
}

SoundFX* AudioManager::findSoundFXFromRefID(ObjectID refID)
{
	SoundFX*	soundFX = NULL;
	Uint32		i;

	if (refID == InvalidObjectID)
		return soundFX;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() && _soundFX[i].getRefID() == refID)
		{
			soundFX = &_soundFX[i];
			break;
		}
	}

	return soundFX;
}

SoundFX* AudioManager::findSoundFXFromName(const Char* name)
{
	SoundFX*	soundFX = NULL;
	Uint32		i;

	if (name)
	{
		for (i=0;i<_maxSoundFX;i++)
		{
			if (_soundFX[i].isUsed() && _soundFX[i].getName())
			{
				if (!strcmp(_soundFX[i].getName(), name))
				{
					soundFX = &_soundFX[i];
					break;
				}
			}
		}
	}

	return soundFX;
}

SoundFX* AudioManager::createSoundFX(const Char* name, const Char* bufferFXName)
{
	SoundFX*		fx = NULL;
	SoundFXBuffer*	bufferFX;

	bufferFX = findSoundFXBufferFromName(bufferFXName);
	
	if (bufferFX)
	{
		fx = getFreeSoundFX();
		
		if (fx)
		{
			fx->create();
			fx->attachSoundFXBuffer(bufferFX);
		}		
	}
	
	return fx;
}

SoundFX* AudioManager::getFreeMusicFX(void)
{
	MusicFX*	musicFX = NULL;
	Uint32		i;
	
	for (i=0;i<_maxMusicFX;i++)
	{
		if (_musicFX[i].isUsed() == false)
		{
			musicFX = &_musicFX[i];
			musicFX->setAsUsed();
			break;
		}
	}
	
	return musicFX;
}

MusicFX* AudioManager::findMusicFXFromRefID(ObjectID refID)
{
	MusicFX*	musicFX = NULL;
	Uint32		i;
	
	if (refID == InvalidObjectID)
		return musicFX;
	
	for (i=0;i<_maxMusicFX;i++)
	{
		if (_musicFX[i].isUsed() && _musicFX[i].getRefID() == refID)
		{
			musicFX = &_musicFX[i];
			break;
		}
	}
	
	return musicFX;
}

MusicFX* AudioManager::findMusicFXFromName(const Char* name)
{
	MusicFX*	musicFX = NULL;
	Uint32		i;
	
	if (name)
	{
		for (i=0;i<_maxMusicFX;i++)
		{
			if (_musicFX[i].isUsed() && _musicFX[i].getName())
			{
				if (!strcmp(_musicFX[i].getName(), name))
				{
					musicFX = &_musicFX[i];
					break;
				}
			}
		}
	}
	
	return musicFX;
}

MusicFX* AudioManager::createMusicFX(const Char* name, const Char* bufferFXName)
{
	MusicFX*		fx = NULL;
	SoundFXBuffer*	bufferFX;
	
	bufferFX = findSoundFXBufferFromName(bufferFXName);
	
	if (bufferFX)
	{
		fx = getFreeMusicFX();
		
		if (fx)
		{
			fx->create();
			fx->attachSoundFXBuffer(bufferFX);
		}		
	}
	
	return fx;
}

void AudioManager::changeSoundFXSoundFXBuffer(SoundFX* fx, const Char* bufferFXName)
{
	SoundFXBuffer*	bufferFX;
	
	bufferFX = findSoundFXBufferFromName(bufferFXName);

	if (bufferFX)
	{
		if (fx->isUsed())
		{
			fx->detachSoundFXBuffer();
			fx->attachSoundFXBuffer(bufferFX);
		}
	}
}

void AudioManager::changeMusicFXSoundFXBuffer(MusicFX* fx, const Char* bufferFXName)
{
	SoundFXBuffer*	bufferFX;
	
	bufferFX = findSoundFXBufferFromName(bufferFXName);
	
	if (bufferFX)
	{
		if (fx->isUsed())
		{
			fx->detachSoundFXBuffer();
			fx->attachSoundFXBuffer(bufferFX);
		}
	}
}

void AudioManager::setSFXEnabled(Boolean enabled)
{
	Uint32	i;
	
	if (enabled == false && _sfxEnabled == true)
	{
		for (i=0;i<_maxSoundFX;i++)
		{
			if (_soundFX[i].isUsed())
				_soundFX[i].stop();
		}
	}
	
	_sfxEnabled = enabled;	
}

void AudioManager::setMusicEnabled(Boolean enabled)
{
	Uint32	i;
	
	if (enabled == false && _musicEnabled == true)
	{
		for (i=0;i<_maxMusicFX;i++)
		{
			if (_musicFX[i].isUsed())
				_musicFX[i].stop();
		}
	}
	
	_musicEnabled = enabled;	
}

void AudioManager::destroySoundFX(SoundFX& fx, Boolean destroySoundFXBuffer)
{
	if (fx.isUsed())
	{
		SoundFXBuffer*	fxBuffer = fx.getSoundFXBuffer();
		
		fx.setAsUnused();
		
		if (destroySoundFXBuffer)
		{
			if (fxBuffer->isUsed())
				fxBuffer->setAsUnused();
		}
	}
}

void AudioManager::destroyMusicFX(SoundFX& fx, Boolean destroySoundFXBuffer)
{
	if (fx.isUsed())
	{
		SoundFXBuffer*	fxBuffer = fx.getSoundFXBuffer();
		
		fx.setAsUnused();
		
		if (destroySoundFXBuffer)
		{
			if (fxBuffer->isUsed())
				fxBuffer->setAsUnused();
		}
	}
}

void AudioManager::destroySoundFXBuffer(SoundFXBuffer& fxBuffer)
{
	Uint32	i;
	
	if (fxBuffer.isUsed())
	{
		for (i=0;i<_maxSoundFX;i++)
		{
			if (_soundFX[i].getSoundFXBuffer() == &fxBuffer)
			{
				//	Need to detach
				_soundFX[i].detachSoundFXBuffer();
			}
		}
		
		for (i=0;i<_maxMusicFX;i++)
		{
			if (_musicFX[i].getSoundFXBuffer() == &fxBuffer)
			{
				//	Need to detach
				_musicFX[i].detachSoundFXBuffer();
			}
		}
		
		fxBuffer.setAsUnused();
	}
}

Uint32 AudioManager::numSoundFXReferencingSoundFXBuffer(SoundFX* fx, SoundFXBuffer* bufferFX)
{
	Uint32	num = 0;
	Uint32	i;
	
	if (bufferFX)
	{
		for (i=0;i<_maxSoundFX;i++)
		{
			if (_soundFX[i].isUsed())
			{
				if (_soundFX[i].getSoundFXBuffer() == bufferFX)
					num++;
			}
		}
	}
	
	return num;
}

void AudioManager::soundFXBufferAttached(SoundFX* soundFX, SoundFXBuffer* soundFXBuffer)
{
	//	This is only valid for non-streaming sounds
	
#ifdef _USE_APPLE_AUDIOQUEUE
#endif _USE_APPLE_AUDIOQUEUE */
}

void AudioManager::soundFXBufferDetached(SoundFX* soundFX, SoundFXBuffer* soundFXBuffer)
{
}

void AudioManager::suspend(void)
{
	Uint32	i;

	if (_suspended == false)
	{
		for (i=0;i<_maxSoundFX;i++)
		{
			if (_soundFX[i].isUsed())
			{
				if (_soundFX[i].isPlaying() && _soundFX[i].isPaused() == false)
					_soundFX[i].pause(AudioManagerPauseSource);
			}
		}
		
		for (i=0;i<_maxMusicFX;i++)
		{
			if (_musicFX[i].isUsed())
			{
				if (_musicFX[i].isPlaying() && _musicFX[i].isPaused() == false)
					_musicFX[i].pause(AudioManagerPauseSource);
			}
		}
		
#ifdef _PLATFORM_IPHONE
		AudioSessionSetActive(NO);
#endif /* _PLATFORM_IPHONE */
		
#ifdef _USE_OPENAL
		alcMakeContextCurrent(NULL);
		alcSuspendContext(_context);
#endif /* _USE_OPENAL */
		
		_suspended = true;
	}
}

void AudioManager::resume(void)
{
	Uint32	i;
	
	if (_suspended)
	{
		
#ifdef _PLATFORM_IPHONE
		AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(_category), &_category);
		AudioSessionSetActive(true);
#endif /* _PLATFORM_IPHONE */
		
#ifdef _USE_OPENAL
		alcMakeContextCurrent(_context);
		alcProcessContext(_context);
#endif /* _USE_OPENAL */
		
		for (i=0;i<_maxSoundFX;i++)
		{
			if (_soundFX[i].isUsed())
			{
				if (_soundFX[i].isPaused(AudioManagerPauseSource))
					_soundFX[i].resume(AudioManagerPauseSource);
			}
		}
		
		for (i=0;i<_maxMusicFX;i++)
		{
			if (_musicFX[i].isUsed())
			{
				if (_musicFX[i].isPaused(AudioManagerPauseSource))
					_musicFX[i].resume(AudioManagerPauseSource);
			}
		}
		
		_suspended = false;
	}
}

void AudioManager::pauseAllSFX(Uint32 source)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed())
		{
			if (_soundFX[i].isPlaying() && _soundFX[i].isPaused() == false)
				_soundFX[i].pause(source);
		}
	}
}

void AudioManager::resumeAllSFX(Uint32 source)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed())
		{
			if (_soundFX[i].isPaused(source))
				_soundFX[i].resume(source);
		}
	}
}

void AudioManager::pauseAllSFX(Uint32 source, Uint32 label)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() && _soundFX[i].getUserLabel() == label)
		{
			if (_soundFX[i].isPlaying() && _soundFX[i].isPaused() == false)
				_soundFX[i].pause(source);
		}
	}
}

void AudioManager::resumeAllSFX(Uint32 source, Uint32 label)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() && _soundFX[i].getUserLabel() == label)
		{
			if (_soundFX[i].isPaused(source))
				_soundFX[i].resume(source);
		}
	}
}

void AudioManager::pauseAllLoopedSFX(Uint32 source)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() && _soundFX[i].isLooping())
		{
			if (_soundFX[i].isPlaying() && _soundFX[i].isPaused() == false)
				_soundFX[i].pause(source);
		}
	}
}

void AudioManager::resumeAllLoopedSFX(Uint32 source)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() && _soundFX[i].isLooping())
		{
			if (_soundFX[i].isPaused(source))
				_soundFX[i].resume(source);
		}
	}
}

void AudioManager::pauseAllLoopedSFX(Uint32 source, Uint32 label)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() && _soundFX[i].isLooping() && _soundFX[i].getUserLabel() == label)
		{
			if (_soundFX[i].isPlaying() && _soundFX[i].isPaused() == false)
				_soundFX[i].pause(source);
		}
	}
}

void AudioManager::resumeAllLoopedSFX(Uint32 source, Uint32 label)
{
	Uint32	i;

	for (i=0;i<_maxSoundFX;i++)
	{
		if (_soundFX[i].isUsed() && _soundFX[i].isLooping() && _soundFX[i].getUserLabel() == label)
		{
			if (_soundFX[i].isPaused(source))
				_soundFX[i].resume(source);
		}
	}
}

void AudioManager::updateUsage(void)
{
	Uint32	i;

	_numSoundFXBuffer = 0;
	_soundFXBufferTotalMemUsage = 0;

	if (_soundFXBuffer)
	{
		for (i=0;i<_maxSoundFXBuffer;i++)
		{
			if (_soundFXBuffer[i].isUsed())
			{
				_numSoundFXBuffer++;
				_soundFXBufferTotalMemUsage += _soundFXBuffer[i].getMemoryUsage();
			}
		}
	}
}

Uint32 AudioManager::getNumUsedSoundFXBuffers(void)
{
	return _numSoundFXBuffer;
}

Uint32 AudioManager::getSoundFXBuffersTotalMemoryUsage(void)
{
	return _soundFXBufferTotalMemUsage;
}

#ifdef _PLATFORM_IPHONE

void AudioManager::enableExternalAppsAudio(void)
{
#ifdef _USE_OPENAL
	alcMakeContextCurrent(NULL);
#endif /* _USE_OPENAL */
	UInt32	currCategory;
	UInt32	currCategorySize = sizeof(currCategory);

	_category = kAudioSessionCategory_AmbientSound;
	
	AudioSessionGetProperty(kAudioSessionProperty_AudioCategory, &currCategorySize, &currCategory);
	
	if (_category != currCategory)		
		AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(_category), &_category);

#ifdef _USE_OPENAL
	alcMakeContextCurrent(_context);
	alcProcessContext(_context);
#endif /* _USE_OPENAL */
}

void AudioManager::disableExternalAppsAudio(void)
{
#ifdef _USE_OPENAL
	alcMakeContextCurrent(NULL);
#endif /* _USE_OPENAL */
	UInt32	currCategory;
	UInt32	currCategorySize = sizeof(currCategory);
	
	_category = kAudioSessionCategory_SoloAmbientSound;

	AudioSessionGetProperty(kAudioSessionProperty_AudioCategory, &currCategorySize, &currCategory);

	if (_category != currCategory)
		AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(_category), &_category);
	
	//	We change this, because this will block audio from playing
	_otherAudioPlayingAtStart = false;
	
#ifdef _USE_OPENAL
	alcMakeContextCurrent(_context);
	alcProcessContext(_context);
#endif /* _USE_OPENAL */
}

Boolean AudioManager::otherAudioPlayingAtStart(void)
{
	return _otherAudioPlayingAtStart;
}

#endif /* _PLATFORM_IPHONE */
