/******************************************************************************
 *
 * File: SoundFXBufferFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * SoundFXBuffer File Load Task Base Class
 * 
 *****************************************************************************/

#include "Platform.h"
#include "App.h"
#include "TaskManager.h"
#include "AudioManager.h"
#include "SoundFXBufferFileLoadTask.h"

SoundFXBufferFileLoadTask::SoundFXBufferFileLoadTask()
{
	_soundFXBuffer = NULL;

	memset(&_soundInfo, 0, sizeof(SoundInfo));
	
#ifdef _USE_APPLE_AUDIOQUEUE
	_audioFileID = NULL;
	_ownsAudioBuffer = false;
	_audioBufferSize = 0;
	_audioBuffer = NULL;
	_maxPacketSize = 0;
#endif /* _USE_APPLE_AUDIOQUEUE */
}

SoundFXBufferFileLoadTask::~SoundFXBufferFileLoadTask()
{
	if (_soundInfo.name)
	{
		delete _soundInfo.name;
		_soundInfo.name = NULL;
	}
	
	if (_soundInfo.filename)
	{
		delete _soundInfo.filename;
		_soundInfo.filename = NULL;
	}

#ifdef _USE_APPLE_AUDIOQUEUE
	if (_audioBuffer && _ownsAudioBuffer)
		delete _audioBuffer;
#endif /* _USE_APPLE_AUDIOQUEUE */
}

void SoundFXBufferFileLoadTask::setSoundInfo(SoundInfo& soundInfo)
{
	if (_soundInfo.name)
	{
		delete _soundInfo.name;
		_soundInfo.name = NULL;
	}

	if (_soundInfo.filename)
	{
		delete _soundInfo.filename;
		_soundInfo.filename = NULL;
	}

	_soundInfo = soundInfo;

	_soundInfo.name = NULL;

	if (soundInfo.name)
	{
		_soundInfo.name = new Char[strlen(soundInfo.name) + 1];

		if (_soundInfo.name)
			strcpy(_soundInfo.name, soundInfo.name);
	}

	_soundInfo.filename = NULL;

	if (soundInfo.filename)
	{
		_soundInfo.filename = new Char[strlen(soundInfo.filename) + 1];

		if (_soundInfo.filename)
			strcpy(_soundInfo.filename, soundInfo.filename);
	}
}

EOSError SoundFXBufferFileLoadTask::prologue(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _USE_APPLE_AUDIOQUEUE
	if (_soundInfo.useAudioQueue)
	{
		NSString*		bundleFilename = [[NSBundle mainBundle] pathForResource:getFilename() ofType:nil];
		CFURLRef		urlRef = (CFURLRef)[[NSURL alloc] initFileURLWithPath:bundleFilename];
		OSStatus		status;
		
		status = AudioFileOpenURL(urlRef, kAudioFileReadPermission, 0, &_audioFileID);
		
		if (status == noErr)
		{
			UInt32	data = sizeof(_audioStreamBasicDescription);
			
			AudioFileGetProperty(_audioFileID, kAudioFilePropertyDataFormat, &data, &_audioStreamBasicDescription);
			
			data = sizeof(_maxPacketSize);
			
			AudioFileGetProperty(_audioFileID, kAudioFilePropertyPacketSizeUpperBound, &data, &_maxPacketSize);
		}
		else
			error = EOSErrorResourceNotAvailable;
		
		CFRelease(urlRef);
		
		if (_soundInfo.streaming)
		{
		}
		else
		{
		}
	}
	else
#endif /* _USE_APPLE_AUDIOQUEUE */
		error = FileLoadTask::prologue();
	
	return error;
}

EOSError SoundFXBufferFileLoadTask::epilogue(void)
{
	AudioManager*	audioMgr;
	EOSError 		error = EOSErrorNone;

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			audioMgr = getTaskManager()->getApp()->getAudioManager();

			if (_soundInfo.name)
				_soundFXBuffer = audioMgr->findSoundFXBufferFromName(_soundInfo.name);
			else
				_soundFXBuffer = audioMgr->findSoundFXBufferFromRefID(_soundInfo.refID);

			if (_soundFXBuffer == NULL)
				_soundFXBuffer = audioMgr->getFreeSoundFXBuffer();

			AssertWDesc(_soundFXBuffer != NULL, "Could not allocate SoundFXBuffer.");

			if (_soundFXBuffer)
			{
#ifdef _USE_APPLE_AUDIOQUEUE				
				if (_soundInfo.useAudioQueue)
				{
					_soundFXBuffer->setStreaming(_soundInfo.streaming);
					_soundFXBuffer->setUseAudioQueue(_soundInfo.useAudioQueue);
					_soundFXBuffer->setAudioFileID(_audioFileID);
					_soundFXBuffer->setMaxPacketSize(_maxPacketSize);
					_soundFXBuffer->setAudioQueueBuffer(_audioBuffer, _audioBufferSize, true);
					_ownsAudioBuffer = false;	//	We now no longer own this buffer
					_soundFXBuffer->setAudioStreamBasicDescription(_audioStreamBasicDescription);
				}
				else
#endif /* _USE_APPLE_AUDIOQUEUE */
				{
#ifdef _USE_OPENAL
					switch (_soundInfo.soundFormat)
					{
						case SoundFormatWAV:
							AssertWDesc(1 == 0, "SoundFormatWAV not supported yet.");
							break;

						case SoundFormatPCM_MONO8:
						case SoundFormatPCM_MONO16:
						case SoundFormatPCM_STEREO8:
						case SoundFormatPCM_STEREO16:
							error = _soundFXBuffer->createFromPCM(_buffer, _soundFXBuffer->convertSoundFormatToALFormat((SoundFormat) _soundInfo.soundFormat), _soundInfo.sampleRate, getFileSize());
							break;
					}
#endif /* _USE_OPENAL */
				}
				
				if (error == EOSErrorNone)
				{
					_soundFXBuffer->setRefID(_soundInfo.refID);
					_soundFXBuffer->setName(_soundInfo.name);
				}
			}
			else
				error = EOSErrorResourceNotAvailable;
		}
		else
			error = EOSErrorNULL;
	}
	else
		error = EOSErrorNULL;

	return error;
}

Boolean SoundFXBufferFileLoadTask::isComplete(EOSError& error)
{
	Boolean	completed = false;
	
#ifdef _USE_APPLE_AUDIOQUEUE
	if (_soundInfo.useAudioQueue)
	{
		error = EOSErrorNone;
		
		if (_soundInfo.streaming)
		{
		}
		else
		{
			UInt32	dataSize = sizeof(UInt64);
			UInt64	bufSize;
			
			if (AudioFileGetProperty(_audioFileID, kAudioFilePropertyAudioDataByteCount, &dataSize, &bufSize) == noErr)
			{
				_audioBufferSize = (Uint32) bufSize;
				
				if (_audioBuffer && _ownsAudioBuffer)
					delete _audioBuffer;
				
				_ownsAudioBuffer = true;
				_audioBuffer = new Uint8[_audioBufferSize];
				
				if (_audioBuffer)
				{
					UInt32	numBytes = _audioBufferSize;
					
					if (AudioFileReadBytes(_audioFileID, false, 0, &numBytes, _audioBuffer) == noErr)
					{
						if (numBytes != _audioBufferSize)
							error = EOSErrorResourceRead;
					}
					else
						error = EOSErrorResourceRead;
				}
				else
				{
					_ownsAudioBuffer = false;
					error = EOSErrorNoMemory;
				}
			}
			else
				error = EOSErrorResourceNotAvailable;
			
			AudioFileClose(_audioFileID);
			_audioFileID = NULL;
		}
		
		completed = true;
	}
	else
#endif /* _USE_APPLE_AUDIOQUEUE */
		completed = FileLoadTask::isComplete(error);
	
	return completed;
}


