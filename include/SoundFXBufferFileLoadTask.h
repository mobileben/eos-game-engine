/******************************************************************************
 *
 * File: SoundFXFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * SoundFX File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __SOUNDFXFILELOADTASK_H__
#define __SOUNDFXFILELOADTASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "SoundFXBuffer.h"
 
class SoundFXBufferFileLoadTask : public FileLoadTask
{
private:
	SoundInfo		_soundInfo;
	SoundFXBuffer*	_soundFXBuffer;

#ifdef _USE_APPLE_AUDIOQUEUE
	AudioFileID					_audioFileID;
	AudioStreamBasicDescription	_audioStreamBasicDescription;
	Uint32						_maxPacketSize;
	Boolean						_ownsAudioBuffer;
	Uint32						_audioBufferSize;
	Uint8*						_audioBuffer;
#endif /* _USE_APPLE_AUDIOQUEUES */
	
public:
	SoundFXBufferFileLoadTask();
	~SoundFXBufferFileLoadTask();

	void				setSoundInfo(SoundInfo& soundInfo);

	virtual EOSError	prologue(void);
	virtual EOSError	epilogue(void);
	virtual Boolean		isComplete(EOSError& error);
};

#endif /* __SOUNDFXFILELOADTASK_H__ */
