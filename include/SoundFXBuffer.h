/******************************************************************************
 *
 * File: SoundFXBuffer.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Sound FX
 * 
 *****************************************************************************/

#ifndef __SOUNDFXBUFFER_H__
#define __SOUNDFXBUFFER_H__

#include "Platform.h"
#include "EOSError.h"

#ifdef _USE_OPENAL

#ifdef _PLATFORM_PC

#include "al.h"
#include "alc.h"

#else

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#endif /* _PLATFORM_PC */

typedef ALuint	HWSoundFXBufferID;

#else

typedef Uint32	HWSoundFXBufferID;

#endif /* _USE_OPENAL */

#ifdef _USE_APPLE_AUDIOQUEUE

#include "AudioToolbox/AudioFile.h"
#include "AudioToolbox/AudioQueue.h"

#endif /* _USE_APPLE_AUDIOQUEUE */

const Uint32		InvalidSoundFXBufferID = 0xFFFFFFFF;
const HWSoundFXBufferID	InvalidHWSoundFXBufferID = 0xFFFFFFFF;

typedef enum
{
	SoundFormatPCM_MONO8 = 0,
	SoundFormatPCM_MONO16,
	SoundFormatPCM_STEREO8,
	SoundFormatPCM_STEREO16,
	SoundFormatWAV,
	SoundFormatCAF,
	SoundFormatMP3,
	SoundFormatAAC,
	SoundFormatLast,
	SoundFormatIllegal = SoundFormatLast
} SoundFormat;

typedef struct
{
	Uint32	refID;
	Uint32	nameOffset;
	Uint32	filenameOffset;
	Uint8	streaming;
	Uint8	bigEndian;
	Uint8	isSigned;
	Uint8	stereo;
	Uint32	sampleRate;
	Uint32	soundFormat;
} SoundInfoExport;

typedef SoundInfoExport SoundInfoRAW;

typedef struct
{
	ObjectID	refID;
	Char*		name;
	Char*		filename;	
	Boolean		streaming;
	Boolean		useAudioQueue;
	Boolean		stereo;
	Boolean		bigEndian;
	Boolean		isSigned;
	Uint32		sampleRate;
	Uint32		soundFormat;
} SoundInfo;

typedef struct
{
	Uint32	endian;
	Uint32	version;
	Uint32	nameOffset;
	Uint32	numSoundInfo;
	Uint32	soundInfo;
	Uint32	names;
} SoundInfoDBHeader;

#ifdef _USE_APPLE_AUDIOQUEUE

typedef struct
{
	Boolean						streaming;
	AudioFileID					fileID;
	AudioStreamBasicDescription	desc;
	Uint32						maxPacketSize;
	Uint8*						buffer;
	Uint32						bufferSize;
} AudioQueueFXBuffer;

#endif /* _USE_APPLE_AUDIOQUEUE */

class AudioManager;

class SoundFXBuffer : public EOSObject
{
private:
	static AudioManager*	_audioMgr;
	
	Boolean					_used;
	Boolean					_instantiated;

	Char*					_name;
	ObjectID				_refID;

	HWSoundFXBufferID		_hwID;

	Boolean					_streaming;

#ifdef _USE_OPENAL
	ALenum					_alFormat;
	ALsizei					_alFreq;
#endif /* _USE_OPENAL */

#ifdef _USE_APPLE_AUDIOQUEUE
	
	Boolean						_useAudioQueue;
	AudioFileID					_audioFileID;
	AudioStreamBasicDescription	_streamBasicDesc;
	Uint32						_maxPacketSize;
	Boolean						_needsFree;
	Uint8*						_audioQueueBuffer;
	Uint32						_audioQueueBufferSize;	
	
#endif /* _USE_APPLE_AUDIOQUEUE */

	Uint32				_memUsage;

	void				_init(void);

public:
	SoundFXBuffer();
	~SoundFXBuffer();
	
	static AudioManager*		getAudioManager(void) { return _audioMgr; }
	static void					setAudioManager(AudioManager* mgr) { _audioMgr = mgr; }

	inline ObjectID		getRefID(void) { return _refID; }
	inline Char*		getName(void) { return _name; }
	inline HWSoundFXBufferID	getHWSoundFXBufferID(void) { return _hwID; }

	void				setAsUsed(void);
	void				setAsUnused(void);

	void				setStreaming(Boolean streaming);
	Boolean				isStreaming(void);

#ifdef _USE_APPLE_AUDIOQUEUE
	
	void				setUseAudioQueue(Boolean use);
	Boolean				usingAudioQueueu(void);
	
	void				setAudioFileID(AudioFileID fid);
	AudioFileID			getAudioFileID(void);
	
	void				setMaxPacketSize(Uint32 size);
	Uint32				getMaxPacketSize(void);
	
	void				setAudioStreamBasicDescription(AudioStreamBasicDescription& desc);
	AudioStreamBasicDescription*	getAudioStreamBasicDescription(void);
	
	void				setAudioQueueBuffer(Uint8* buffer, Uint32 bufferSize, Boolean needsToFree);
	Uint8*				getAudioQueueBuffer(void);
	Uint32				getAudioQueueBufferSize(void);
	
	void				buildAudioQueueBuffer(Uint8* buffer, Uint32 bufferSize);
	
#endif /* _USE_APPLE_AUDIOQUEUE */
	
	inline void			setRefID(ObjectID refID) { _refID = refID; }
	void 				setName(const Char* name);

	inline Boolean		isUsed(void) { return _used; }

	void				uninstantiate(void);

#ifdef _USE_OPENAL
	ALenum				convertSoundFormatToALFormat(SoundFormat format);
	EOSError			createFromWAV(ALvoid* buffer, ALenum format, ALsizei freq, ALsizei bufferSize);
	EOSError			createFromPCM(ALvoid* buffer, ALenum format, ALsizei freq, ALsizei bufferSize);
#endif /* _USE_OPENAL */

	inline Uint32		getMemoryUsage(void) { return _memUsage; }
};

#endif /* __SOUNDFXBUFFER_H__ */

