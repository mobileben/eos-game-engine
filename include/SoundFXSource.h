/******************************************************************************
 *
 * File: SoundFXSource.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Sound FX Source
 * 
 *****************************************************************************/

#ifndef __SOUNDFXSOURCE_H__
#define __SOUNDFXSOURCE_H__

#include "Platform.h"
#include "EOSError.h"

#ifdef _USE_OPENAL

#include "al.h"
#include "alc.h"

typedef ALuint	HWSoundFXSourceID;

#else

typedef Uint32	HWSoundFXSourceID;

#endif /* _USE_OPENAL */


const Uint32			InvalidSoundFXSourceID = 0xFFFFFFFF;
const HWSoundFXSourceID	InvalidHWSoundFXSourceID = 0xFFFFFFFF;

class SoundFXBuffer;

class SoundFXSource : public EOSObject
{
private:
	Boolean				_used;
	Boolean				_instantiated;

	Char*				_name;

	ObjectID			_refID;
	HWSoundFXSourceID	_hwID;

	SoundFXBuffer*		_soundFXBuffer;

	void				_init(void);

public:
	SoundFXSource();
	~SoundFXSource();

	inline ObjectID				getRefID(void) { return _refID; }
	inline Char*				getName(void) { return _name; }
	inline HWSoundFXSourceID	getHWSoundFXSourceID(void) { return _hwID; }

	void						setAsUsed(void);
	void						setAsUnused(void);

	inline void					setRefID(ObjectID refID) { _refID = refID; }
	void 						setName(Char* name);

	inline Boolean				isUsed(void) { return _used; }

	void						uninstantiate(void);

	void						detachSoundFXBuffer(void);
	void						attachSoundFXBuffer(SoundFXBuffer* sound);

	EOSError					create(void);

	Boolean						isPlaying(void);
	Boolean						isLooping(void);
	Boolean						isPaused(void);

	void						play(Boolean loop);
	void						pause(void);
	void						stop(void);
};


#endif /* __SOUNDFXSOURCE_H__ */

