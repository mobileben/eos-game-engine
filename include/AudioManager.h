/******************************************************************************
 *
 * File: AudioManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Audio Manager
 * 
 *****************************************************************************/

#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__

#include "Platform.h"
#include "SoundFXBuffer.h"
#include "SoundFX.h"
#include "EOSError.h"

#ifdef _USE_OPENAL

#ifdef _PLATFORM_PC

#include "al.h"
#include "alc.h"

#else

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#endif /* _PLATFORM_PC */

#endif /* _USE_OPENAL */

class TaskManager;

class AudioManager : public EOSFrameworkComponent
{
public:
	static const int AudioManagerPauseSource = 1;
	static const int UserFirstPauseSource = 2;

private:
	Uint32			_maxSoundFXBuffer;
	Uint32			_numSoundFXBuffer;
	SoundFXBuffer*	_soundFXBuffer;

	Uint32			_maxSoundFX;
	Uint32			_numSoundFX;
	SoundFX*		_soundFX;

	Uint32			_maxMusicFX;
	Uint32			_numMusicFX;
	MusicFX*		_musicFX;

	Boolean			_soundInfoNeedsFree;
	Uint32			_numSoundInfo;
	SoundInfo*		_soundInfo;

	Boolean			_sfxEnabled;
	Boolean			_musicEnabled;
	
	Boolean			_suspended;

	Uint32			_soundFXBufferTotalMemUsage;

#ifdef _PLATFORM_IPHONE
	Boolean			_otherAudioPlayingAtStart;
	UInt32			_category;
#endif /* _PLATFORM_IPHONE */
	
#ifdef _USE_OPENAL

	ALCdevice*		_device;
	ALCcontext*		_context;

#endif /* _USE_OPENAL*/

	void			destroySoundInfoDatabase(void);

public:
	AudioManager();
	~AudioManager();

	EOSError		allocateSoundFXBufferPool(Uint32 sfxmax);
	void			deallocateSoundFXBufferPool(void);

	EOSError		allocateSoundFXPool(Uint32 sourcemax);
	void			deallocateSoundFXPool(void);
	
	EOSError		allocateMusicFXPool(Uint32 sourcemax);
	void			deallocateMusicFXPool(void);

	EOSError		createSoundInfoDatabase(const Char* filename, Uint8* buffer, Uint32 buffersize);

	SoundInfo*		findSoundInfoByRefID(ObjectID objid);
	SoundInfo*		findSoundInfoByRefName(const Char* name);

	SoundFXBuffer*	getFreeSoundFXBuffer(void);
	SoundFXBuffer*	findSoundFXBufferFromRefID(ObjectID refid);
	SoundFXBuffer*	findSoundFXBufferFromName(const Char* name);

	SoundFX*		getFreeSoundFX(void);
	SoundFX*		findSoundFXFromRefID(ObjectID refid);
	SoundFX*		findSoundFXFromName(const Char* name);
	
	SoundFX*		createSoundFX(const Char* name, const Char* bufferFXName);
	void			changeSoundFXSoundFXBuffer(SoundFX* fx, const Char* bufferFXName);
	
	MusicFX*		getFreeMusicFX(void);
	MusicFX*		findMusicFXFromRefID(ObjectID refid);
	MusicFX*		findMusicFXFromName(const Char* name);
	
	MusicFX*		createMusicFX(const Char* name, const Char* bufferFXName);
	void			changeMusicFXSoundFXBuffer(MusicFX* fx, const Char* bufferFXName);
	
	void			destroyMusicFX(MusicFX& fx, Boolean destroySoundFXBuffer);
	void			destroySoundFX(SoundFX& fx, Boolean destroySoundFXBuffer);
	void			destroySoundFXBuffer(SoundFXBuffer& fxBuffer);
	
	Uint32			numSoundFXReferencingSoundFXBuffer(SoundFX* fx, SoundFXBuffer* bufferFX);
	
	void			soundFXBufferAttached(SoundFX* soundFX, SoundFXBuffer* soundFXBuffer);
	void			soundFXBufferDetached(SoundFX* soundFX, SoundFXBuffer* soundFXBuffer);

	void			queueSoundFXBufferLoad(TaskManager* loader, Uint8* buffer, Uint32 buffersize, const Char* name);

#ifdef _PLATFORM_IPHONE
	
	Boolean			otherAudioPlayingAtStart(void);
	void			enableExternalAppsAudio(void);
	void			disableExternalAppsAudio(void);
	
#endif /* _PLATFORM_IPHONE */
	
	void			setMasterVolume(Uint32);
	Uint32			getMasterVolume(void);

	inline Boolean	isSFXEnabled(void) { return _sfxEnabled; }
	void			setSFXEnabled(Boolean enabled);

	inline Boolean	isMusicEnabled(void) { return _musicEnabled; }
	void			setMusicEnabled(Boolean enabled);
	
	void			init(void);
	void			destroy(void);

	void			suspend(void);
	void			resume(void);

	void			pauseAllSFX(Uint32 source);
	void			resumeAllSFX(Uint32 source);
	void			pauseAllSFX(Uint32 source, Uint32 label);
	void			resumeAllSFX(Uint32 source, Uint32 label);

	void			pauseAllLoopedSFX(Uint32 source);
	void			resumeAllLoopedSFX(Uint32 source);
	void			pauseAllLoopedSFX(Uint32 source, Uint32 label);
	void			resumeAllLoopedSFX(Uint32 source, Uint32 label);

	void			update(MicroSeconds delta_t);

	void			updateUsage(void);

	Uint32			getNumUsedSoundFXBuffers(void);
	Uint32			getSoundFXBuffersTotalMemoryUsage(void);
};

#endif /* __AUDIO_MANAGER_H__ */

