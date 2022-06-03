/******************************************************************************
 *
 * File: SoundFX.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Sound FX Source
 * 
 *****************************************************************************/

#include "Platform.h"
#include "SoundFX.h"
#include "SoundFXBuffer.h"
#include "AudioManager.h"

AudioManager*	SoundFX::_audioMgr = NULL;

#ifdef _USE_APPLE_AUDIOQUEUE

#include <AudioToolbox/AudioToolbox.h>

static void AudioQueueHandleOutputBuffer(void* data, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer)
{
	SoundFX*	soundFX = (SoundFX*) data;
	UInt32		numBytesReadFromFile = 0;
	UInt32		numPackets = soundFX->getNumPacketsToRead();
	OSStatus	status;
	Uint32		loopCount = soundFX->getLoopingCount();
	Boolean		loopForever = (loopCount == SoundFX::LOOP_FOREVER);
	
	if (soundFX->isStreaming())
	{
		status = AudioFileReadPackets(soundFX->getSoundFXBuffer()->getAudioFileID(), false, &numBytesReadFromFile, soundFX->getAudioStreamPacketDescription(), soundFX->getCurrentPacket(), &numPackets, inBuffer->mAudioData);
		
		if (status == noErr)
		{
			if (numPackets > 0)
			{
				inBuffer->mAudioDataByteSize = numBytesReadFromFile;
				inBuffer->mPacketDescriptionCount = numPackets;
				
				status = AudioQueueEnqueueBuffer(soundFX->getAudioQueueRef(), inBuffer, (soundFX->getAudioStreamPacketDescription() ? numPackets : 0), soundFX->getAudioStreamPacketDescription());
				
				soundFX->setCurrentPacket(soundFX->getCurrentPacket() + numPackets);
			}
			else
			{
				if (loopForever || loopCount > 1)
				{
					if (loopForever == false)
						soundFX->setLoopingCount(loopCount - 1);
					
					soundFX->setCurrentPacket(0);
					AudioQueueHandleOutputBuffer(data, inAQ, inBuffer);
				}
				else
				{
					soundFX->setLoopingCount(0);
					AudioQueueFlush(soundFX->getAudioQueueRef());
					AudioQueueStop(soundFX->getAudioQueueRef(), false);
				}
			}
		}
		
	}
	else
	{
		Sint32	index = soundFX->getCurrentMemoryImageIndex();
		Sint32	xferSize;
		Sint32	totalSize = soundFX->getAudioMemoryImageSize();
		Uint8*	audioData = soundFX->getAudioMemoryImage();
		Uint8*	dst = (Uint8*) inBuffer->mAudioData;
		Boolean	go = true;
		
		if (index >= totalSize)
		{
			if (loopForever || loopCount > 1)
			{
				if (loopForever == false)
					soundFX->setLoopingCount(loopCount - 1);
				
				soundFX->setCurrentMemoryImageIndex(0);
				
				index = soundFX->getCurrentMemoryImageIndex();
			}
			else
			{
				soundFX->setLoopingCount(0);
				
				AudioQueueFlush(soundFX->getAudioQueueRef());
				AudioQueueStop(soundFX->getAudioQueueRef(), false);
				
				go = false;
			}
		}
		
		if (go)
		{
			xferSize = totalSize - index;
			
			if (xferSize > inBuffer->mAudioDataBytesCapacity)
				xferSize = inBuffer->mAudioDataBytesCapacity;

			if (xferSize < totalSize)
				memcpy((Uint8*) dst, &audioData[index], xferSize);	//	This implies we are using a queue size that will require multiple loads
			
			inBuffer->mAudioDataByteSize = xferSize;
			inBuffer->mPacketDescriptionCount = 0;
			
			status = AudioQueueEnqueueBuffer(soundFX->getAudioQueueRef(), inBuffer, 0, soundFX->getAudioStreamPacketDescription());
			
			soundFX->setCurrentMemoryImageIndex(index + xferSize);
		}
	}
}

static void AudioQueueIsRunningOutputBuffer(void* data, AudioQueueRef inAQ, AudioQueuePropertyID inID)
{
	SoundFX*	soundFX = (SoundFX*) data;

	switch (soundFX->getPlayState())
	{
		case SoundFX::PlayStateOff:
		case SoundFX::PlayStateQueued:
		case SoundFX::PlayStatePaused:
			soundFX->setPlayState(SoundFX::PlayStatePlaying);			
			break;
			
		default:
			soundFX->setPlayState(SoundFX::PlayStateOff);
			break;
	}
}

static void DeriveBufferSize (AudioStreamBasicDescription &ASBDesc,                            // 1
					   UInt32                      maxPacketSize,                       // 2
					   Float64                     seconds,                             // 3
					   UInt32                      *outBufferSize,                      // 4
					   UInt32                      *outNumPacketsToRead                 // 5
) {
    static const int maxBufferSize = 0x50000;                        // 6
    static const int minBufferSize = 0x4000;                         // 7
	
    if (ASBDesc.mFramesPerPacket != 0) {                             // 8
        Float64 numPacketsForTime =
		ASBDesc.mSampleRate / ASBDesc.mFramesPerPacket * seconds;
        *outBufferSize = numPacketsForTime * maxPacketSize;
    } else {                                                         // 9
        *outBufferSize =
		maxBufferSize > maxPacketSize ?
		maxBufferSize : maxPacketSize;
    }
	
    if (                                                             // 10
        *outBufferSize > maxBufferSize &&
        *outBufferSize > maxPacketSize
		)
        *outBufferSize = maxBufferSize;
    else {                                                           // 11
        if (*outBufferSize < minBufferSize)
            *outBufferSize = minBufferSize;
    }
	
    *outNumPacketsToRead = *outBufferSize / maxPacketSize;           // 12
}

#endif /* _USE_APPLE_AUDIOQUEUE */

SoundFX::SoundFX() : _name(NULL), _soundType(SoundTypeSFX)
{
	_init();
}

SoundFX::~SoundFX()
{
#ifdef _USE_APPLE_AUDIOQUEUE
	if (_packetDesc)
		delete _packetDesc;
#endif /* _USE_APPLE_AUDIOQUEUE */
}

void SoundFX::_init(void)
{
#ifdef _USE_APPLE_AUDIOQUEUE
	Uint32	i;
#endif /* _USE_APPLE_AUDIOQUEUE */
	
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

#ifdef _USE_APPLE_AUDIOQUEUE
	_playState = PlayStateOff;

	_usingAudioQueue = false;
	
	_queue = NULL;
	
	_actualBuffersUsed = 0;
	
	for (i=0;i<kAudioQueueNumBuffers;i++)
		_buffers[i] = NULL;
		
	_bufferSize = kAudioQueueBufferSize;
	_currPacket = 0;
	_memoryImageIndex = 0;
	_numPacketsToRead = 0;
	_packetDesc = NULL;
#endif /* _USE_APPLE_AUDIOQUEUE */
	
	_refID = InvalidObjectID;
	_hwID = InvalidHWSoundFXID;

	_soundFXBuffer = NULL;

	_looping = 0;
	_pauseSource = 0;

	_userLabel = 0;

	_used = false;
	_instantiated = false;
}

void SoundFX::setAsUsed(void)
{
	if (_used == false)
	{
		_init();

		_used = true;
	}
}

void SoundFX::setAsUnused(void)
{
	if (_used == true)
	{
		uninstantiate();

		_init();

		_used = false;
	}
}

void SoundFX::setName(const Char* name)
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

Char* SoundFX::getName(void)
{
	if (_name)
		return _name;
	else
	{
		if (_soundFXBuffer)
			return _soundFXBuffer->getName();
		else
			return NULL;
	}
}

Boolean SoundFX::isUsingSoundFXBuffer(const Char* name)
{
	Boolean	isUsing = false;
	
	if (_soundFXBuffer)
	{
		if (!strcmp(_soundFXBuffer->getName(), name))
			isUsing = true;
	}
	
	return isUsing;
}

#ifdef _USE_APPLE_AUDIOQUEUE

AudioFileID SoundFX::getAudioFileID(void) 
{ 
	return _soundFXBuffer->getAudioFileID();
}	

Boolean SoundFX::isStreaming(void)
{
	return _soundFXBuffer->isStreaming();
}

void SoundFX::setPlayState(PlayState state)
{
	_playState = state;
}

Uint8* SoundFX::getAudioMemoryImage(void)
{
	return _soundFXBuffer->getAudioQueueBuffer();
}

Uint32 SoundFX::getAudioMemoryImageSize(void)
{
	return _soundFXBuffer->getAudioQueueBufferSize();
}

Uint32 SoundFX::getNumActualAudioQueueBuferRefUsed(void)
{
	return _actualBuffersUsed;
}

AudioQueueBufferRef SoundFX::getAudioQueueBufferRefAtIndex(Uint32 index)
{
	return _buffers[index];
}

#endif /* _USE_APPLE_AUDIOQUEUE */

void SoundFX::uninstantiate(void)
{
	if (_instantiated)
	{
		detachSoundFXBuffer();
		
		_instantiated = false;

	}
}

void SoundFX::detachSoundFXBuffer(void)
{	
	_audioMgr->soundFXBufferDetached(this, _soundFXBuffer);

#ifdef _USE_APPLE_AUDIOQUEUE
	//	Detaching a sound buffer in the case of AudioQueue requires our queue to be destroyed
	if (_usingAudioQueue)
	{
		if (_queue)
		{
			Uint32	i;
			
			AudioQueueFlush(_queue);
			
			if (_playState != PlayStateOff)
				AudioQueueStop(_queue, YES);
			
			AudioQueueDispose(_queue, YES);
			
			if (_packetDesc)
			{
				delete _packetDesc;
				_packetDesc = NULL;
			}
						
			for (i=0;i<kAudioQueueNumBuffers;i++)
			{
				_buffers[i] = NULL;
			}

			_queue = NULL;
			_usingAudioQueue = false;

			//	Important to reset state, since the callback will change it to what it wants
			_pauseSource = 0;
			_playState = PlayStateOff;
		}
	}
	
#endif /* _USE_APPLE_AUDIOQUEUE */
	
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXID)
	{
		alDeleteSources(1, &_hwID);
		alSourcei(_hwID, AL_BUFFER, 0);
		_hwID = InvalidHWSoundFXID;		
	}
#endif /* _USE_OPENAL */
	
	_soundFXBuffer = NULL;
}

EOSError SoundFX::attachSoundFXBuffer(SoundFXBuffer* soundFXBuffer)
{
	EOSError error = EOSErrorNone;
	
	if (_used && _instantiated)
	{
		_soundFXBuffer = soundFXBuffer;

#ifdef _USE_APPLE_AUDIOQUEUE
		if (_soundFXBuffer->usingAudioQueueu())
		{
			OSStatus status;
			Uint32		i;
			Boolean		isFormatVBR;
			UInt32		cookieSize(sizeof(UInt32));
			Boolean		noProperty;
			Char*		magicCookie = NULL;

			//	Detach a previous queue, if on exists
			if (_queue)
				detachSoundFXBuffer();
						
			_usingAudioQueue = true;
			
			status = AudioQueueNewOutput(_soundFXBuffer->getAudioStreamBasicDescription(), AudioQueueHandleOutputBuffer, this, CFRunLoopGetCurrent(), kCFRunLoopCommonModes, 0, &_queue);
			
			if (status == noErr)
			{
				AudioQueueAddPropertyListener(_queue, kAudioQueueProperty_IsRunning, AudioQueueIsRunningOutputBuffer, this);
				
				DeriveBufferSize(*_soundFXBuffer->getAudioStreamBasicDescription(), _soundFXBuffer->getMaxPacketSize(), 0.5, &_bufferSize, &_numPacketsToRead);
				
				isFormatVBR = (_soundFXBuffer->getAudioStreamBasicDescription()->mBytesPerPacket == 0 || _soundFXBuffer->getAudioStreamBasicDescription()->mFramesPerPacket == 0);

				if (_packetDesc)
					delete _packetDesc;
				
				if (isFormatVBR)
					_packetDesc = new AudioStreamPacketDescription[_numPacketsToRead];
				else
					_packetDesc = NULL;
				
				noProperty = AudioFileGetProperty(getAudioFileID(), kAudioFilePropertyMagicCookieData, &cookieSize, NULL);
				
				if (noProperty && cookieSize)
				{
					magicCookie = new Char[cookieSize];
					
					if (magicCookie)
					{
						AudioFileGetProperty(getAudioFileID(), kAudioFilePropertyMagicCookieData, &cookieSize, magicCookie);
						
						AudioQueueSetProperty(_queue, kAudioQueueProperty_MagicCookie, magicCookie, cookieSize);
						
						delete magicCookie;
					}
					else
						error = EOSErrorNoMemory;
				}
				
				if (isStreaming())
				{
					_actualBuffersUsed = 0;
					
					for (i=0;i<kAudioQueueNumBuffers;i++)
					{
						status = AudioQueueAllocateBuffer(_queue, _bufferSize, &_buffers[i]);
						
						if (status == noErr)
						{
							_actualBuffersUsed++;
						}
						else
						{
							error = EOSErrorResourceNotAvailable;
							break;
						}
					}				
				}
				else
				{
					_actualBuffersUsed = 1;
					_bufferSize = _soundFXBuffer->getAudioQueueBufferSize();
					
					status = AudioQueueAllocateBuffer(_queue, _bufferSize, &_buffers[0]);
					memcpy((Uint8*) _buffers[0]->mAudioData, _soundFXBuffer->getAudioQueueBuffer(), _bufferSize);

					_buffers[1] = NULL;
					_buffers[2] = NULL;
				}
			}
		}
#endif /* _USE_APPLE_AUDIOQUEUE */
#ifdef _USE_OPENAL
		if (soundFXBuffer->getHWSoundFXBufferID() != InvalidHWSoundFXID)
		{
			ALenum	alError = AL_NO_ERROR;
			
			if (_hwID == InvalidHWSoundFXID)
			{
				alGenSources(1, &_hwID);
				
				alError = alGetError();
			}
			
			alSourcei(_hwID, AL_BUFFER, soundFXBuffer->getHWSoundFXBufferID());
		}
		else if (_hwID != InvalidHWSoundFXID) 
		{
			alDeleteSources(1, &_hwID);
			alSourcei(_hwID, AL_BUFFER, 0);
			_hwID = InvalidHWSoundFXID;					
		}
#endif /* _USE_OPENAL */
		
		_audioMgr->soundFXBufferAttached(this, _soundFXBuffer);
	}
	
	return error;
}

#ifdef _USE_APPLE_AUDIOQUEUE
void SoundFX::prime(void)
{
	Uint32	i;
	
	_currPacket = 0;
	_memoryImageIndex = 0;
	
	for (i=0;i<_actualBuffersUsed;i++)
	{
		AudioQueueHandleOutputBuffer(this, _queue, _buffers[i]);
	}					
}
#endif /* _USE_APPLE_AUDIOQUEUE */

EOSError SoundFX::create(void)
{
	EOSError 	error = EOSErrorNone;

	if (_used)
	{
	}
	else
		error = EOSErrorUnallocated;

	if (error == EOSErrorNone)
		_instantiated = true;

	return error;
}

Boolean SoundFX::isPlaying(void)
{
	Boolean	playing = false;

#if _USE_APPLE_AUDIOQUEUE
	if (_usingAudioQueue)
	{
		//	This is needed since when you are stopping the queue, it does not always call a routine to actually stop stop it. TEST AND VERIFY LATER!
		if (_playState == PlayStateOff || _playState == PlayStateStopping)
			playing = false;
		else
			playing = true;
	}
#endif /* _USE_APPLE_AUDIOQUEUE */
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXID)
	{
		ALint	val;
		
		alGetSourcei(_hwID, AL_SOURCE_STATE, &val);
			
		if (val == AL_PLAYING)
			playing = true;
	}
#endif /* _USE_OPENAL*/

	return playing;
}

Boolean SoundFX::isLooping(void)
{
	Boolean	looped = false;

#ifdef _USE_APPLE_AUDIOQUEUE
	if (_usingAudioQueue)
	{
		if (_looping > 1)
			looped = true;
	}
#endif /* _USE_APPLE_AUDIOQUEUE */
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXID)
	{
		ALint	val;
		
		alGetSourcei(_hwID, AL_LOOPING, &val);
		
		if (val == AL_TRUE)
			looped = true;
	}
#endif /* _USE_OPENAL*/

	return looped;
}

Boolean SoundFX::isPaused(Uint32 source)
{
	if (source && source != _pauseSource)
		return false;

#ifdef _USE_APPLE_AUDIOQUEUE
	if (_usingAudioQueue)
	{
		if (_playState == PlayStatePaused)
			return true;
	}
#endif /* _USE_APPLE_AUDIOQUEUE */
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXID)
	{
		ALint	val;
		
		alGetSourcei(_hwID, AL_SOURCE_STATE, &val);
		
		if (val == AL_PAUSED)
			return true;
	}
#endif /* _USE_OPENAL*/

	return false;
}

void SoundFX::play(Uint32 loop)
{
	_looping = loop;
	_pauseSource = 0;

	if (loop == 0 || isPlaying())
		return;
		
	if (_audioMgr)
	{
		if (_soundType == SoundTypeSFX)
		{
			if (_audioMgr->isSFXEnabled() == false)
				return;
		}
		else
		{
			if (_audioMgr->isMusicEnabled() == false)
				return;
#ifdef _PLATFORM_IPHONE
			else if (_audioMgr->otherAudioPlayingAtStart() == true)
				return;
#endif /* _PLATFORM_IPHONE */			
		}	
	}

#ifdef _USE_APPLE_AUDIOQUEUE
	if (_usingAudioQueue)
	{
		OSStatus status;

#ifdef _AUDIO_FORCE_STOP_BEFORE_PLAY
		_playState = PlayStateStopping;
		
		AudioQueueStop(_queue, YES);
#endif /* _AUDIO_FORCE_STOP_BEFORE_PLAY */
		
		_playState = PlayStateQueued;
		
		prime();
		
		status = AudioQueuePrime(_queue, 0, NULL);
		
		status = AudioQueueStart(_queue, NULL);
	}
#endif /* _USE_APPLE_AUDIOQUEUE */
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXID)
	{
		if (loop > 1)
			alSourcei(_hwID, AL_LOOPING, AL_TRUE);
		else
			alSourcei(_hwID, AL_LOOPING, AL_FALSE);

		alSourcePlay(_hwID);
	}
#endif /* _USE_OPENAL*/
}

void SoundFX::resume(Uint32 source)
{
	if (_audioMgr)
	{
		if (_soundType == SoundTypeSFX)
		{
			if (_audioMgr->isSFXEnabled() == false)
				return;
		}
		else
		{
			if (_audioMgr->isMusicEnabled() == false)
				return;
#ifdef _PLATFORM_IPHONE
			else if (_audioMgr->otherAudioPlayingAtStart() == true)
				return;
#endif /* _PLATFORM_IPHONE */
		}	
	}

	if (source != _pauseSource)
		return;

#ifdef _USE_APPLE_AUDIOQUEUE
	if (_usingAudioQueue)
	{
		OSStatus	status;
		
		_playState = PlayStatePlaying;
		status = AudioQueueStart(_queue, NULL);
	}
#endif /* _USE_APPLE_AUDIOQUEUE */
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXID)
	{
		if (_looping > 1)
			alSourcei(_hwID, AL_LOOPING, AL_TRUE);
		else
			alSourcei(_hwID, AL_LOOPING, AL_FALSE);
		
		alSourcePlay(_hwID);
	}
#endif /* _USE_OPENAL */

	_pauseSource = 0;
}

void SoundFX::pause(Uint32 source)
{
#ifdef _USE_APPLE_AUDIOQUEUE
	if (_usingAudioQueue)
	{
		if (isPlaying())
		{
			_playState = PlayStatePaused;
			_pauseSource = source;
			AudioQueuePause(_queue);
		}
	}
#endif /* _USE_APPLE_AUDIOQUEUE */
#ifdef _USE_OPENAL
	if (_hwID != InvalidHWSoundFXID)
	{
		alSourcePause(_hwID);
		_pauseSource = source;
	}
#endif /* _USE_OPENAL*/
}

void SoundFX::stop(void)
{
#ifdef _USE_APPLE_AUDIOQUEUE
	if (_usingAudioQueue)
	{
		if (isPlaying())
		{
			_playState = PlayStateStopping;
			_pauseSource = 0;
			AudioQueueStop(_queue, YES);
		}
	}
#endif /* _USE_APPLE_AUDIOQUEUE */
	if (_hwID != InvalidHWSoundFXID)
	{
#ifdef _USE_OPENAL
		alSourceStop(_hwID);
#endif /* _USE_OPENAL */

		_pauseSource = 0;
	}
}
