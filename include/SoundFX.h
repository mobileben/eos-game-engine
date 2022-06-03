/******************************************************************************
 *
 * File: SoundFX.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Sound FX 
 * 
 *****************************************************************************/

#ifndef __SOUNDFX_H__
#define __SOUNDFX_H__

#include "Platform.h"
#include "EOSError.h"

#ifdef _USE_OPENAL

#ifdef _PLATFORM_PC

#include "al.h"
#include "alc.h"

#else

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#endif /* _USE_OPENAL */

typedef ALuint	HWSoundFXID;

#else

typedef Uint32	HWSoundFXID;

#endif /* _USE_OPENAL */

#ifdef _USE_APPLE_AUDIOQUEUE

#include "AudioToolbox/AudioFile.h"
#include "AudioToolbox/AudioQueue.h"

static const int kAudioQueueNumBuffers = 3;
static const int kAudioQueueBufferSize = 100 * 1024;	//	This is temp, make this a settable value through AudioManager
	
#endif /* _USE_APPLE_AUDIOQUEUE */

const Uint32		InvalidSoundFXID = 0xFFFFFFFF;
const HWSoundFXID	InvalidHWSoundFXID = 0xFFFFFFFF;

class SoundFXBuffer;
class AudioManager;

class SoundFX : public EOSObject
{
public:
	static const Uint32				LOOP_FOREVER = 0xFFFFFFFF;
	static const Uint32				PLAY_ONCE = 1;
	
	typedef enum
	{
		PlayStateOff = 0,
		PlayStateQueued,
		PlayStatePlaying,
		PlayStatePaused,
		PlayStateStopping
	} PlayState;
	
	typedef enum
	{
		SoundTypeSFX = 0,
		SoundTypeMusic
	} SoundType;
	
private:
	static AudioManager*			_audioMgr;
	
	Boolean							_used;
	Boolean							_instantiated;

	SoundType						_soundType;
	
	Char*							_name;

	ObjectID						_refID;
	HWSoundFXID	_hwID;

	SoundFXBuffer*					_soundFXBuffer;

	Uint32							_userLabel;

	Uint32							_looping;
	Uint32							_pauseSource;

#ifdef _USE_APPLE_AUDIOQUEUE
	PlayState						_playState;
	Boolean							_usingAudioQueue;
	AudioQueueRef					_queue;
	Uint32							_actualBuffersUsed;
	AudioQueueBufferRef				_buffers[kAudioQueueNumBuffers];
	UInt32							_bufferSize;
	SInt32							_currPacket;
	UInt32							_numPacketsToRead;
	AudioStreamPacketDescription*	_packetDesc;
	Sint32							_memoryImageIndex;
#endif /* _USE_APPLE_AUDIOQUEUE */
	
	void							_init(void);

public:
	SoundFX();
	~SoundFX();

	static AudioManager*		getAudioManager(void) { return _audioMgr; }
	static void					setAudioManager(AudioManager* mgr) { _audioMgr = mgr; }
	
	inline ObjectID				getRefID(void) { return _refID; }
	Char*						getName(void);
	inline HWSoundFXID			getHWSoundFXID(void) { return _hwID; }

	void						setAsUsed(void);
	void						setAsUnused(void);

	inline void					setRefID(ObjectID refID) { _refID = refID; }
	void 						setName(const Char* name);

	inline Boolean				isUsed(void) { return _used; }

	void						uninstantiate(void);

	inline SoundFXBuffer*		getSoundFXBuffer(void) { return _soundFXBuffer; }
	
	inline void					setSoundType(SoundType type) { _soundType = type; }
	inline SoundType			getSoundType(void) { return _soundType; }
	
	Boolean						isUsingSoundFXBuffer(const Char* name);

	inline void					setUserLabel(Uint32 label) { _userLabel = label; }
	inline Uint32 				getUserLabel(void) { return _userLabel; }

#ifdef _USE_APPLE_AUDIOQUEUE
	
	Boolean						isStreaming(void);
	inline Boolean				isUsingAudioQueue(void) { return _usingAudioQueue; }
	
	inline Uint32				getNumPacketsToRead(void) { return _numPacketsToRead; }
	AudioFileID					getAudioFileID(void);
	inline Sint32				getCurrentPacket(void) { return _currPacket; }
	inline void					setCurrentPacket(Sint32 packet) { _currPacket = packet; }
	inline Sint32				getCurrentMemoryImageIndex(void) { return _memoryImageIndex; }
	void						setCurrentMemoryImageIndex(Sint32 index) { _memoryImageIndex = index; }
	inline AudioQueueRef		getAudioQueueRef(void) { return _queue; }
	inline AudioStreamPacketDescription* getAudioStreamPacketDescription(void) { return _packetDesc; }
	
	Uint32						getNumActualAudioQueueBuferRefUsed(void);
	AudioQueueBufferRef			getAudioQueueBufferRefAtIndex(Uint32 index);
	
	Uint8*						getAudioMemoryImage(void);
	Uint32						getAudioMemoryImageSize(void);
	void						prime(void);
	inline PlayState			getPlayState(void) { return _playState; }
	void						setPlayState(PlayState state);
	inline Uint32				getActualBuffersUsed(void) { return _actualBuffersUsed; }
	
#endif /* _USE_APPLE_AUDIOQUEUE */
	
	void						detachSoundFXBuffer(void);
	EOSError					attachSoundFXBuffer(SoundFXBuffer* sound);

	EOSError					create(void);

	Boolean						isPlaying(void);
	Boolean						isLooping(void);
	
	inline Uint32				getLoopingCount(void) { return _looping; }
	inline void					setLoopingCount(Uint32 looping) { _looping = looping; }
	
	Boolean						isPaused(Uint32 source = 0);

	void						play(Uint32 loop = PLAY_ONCE);
	void						pause(Uint32 source = 0);
	void						resume(Uint32 source = 0);
	void						stop(void);
};

typedef SoundFX MusicFX;

#endif /* __SOUNDFX_H__ */

