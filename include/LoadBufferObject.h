/******************************************************************************
 *
 * File: LoadBufferObject.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Load Buffer Object
 * 
 *****************************************************************************/

#ifndef __LOADBUFFEROBJECT_H__
#define __LOADBUFFEROBJECT_H__

#include "Platform.h"
#include "EOSError.h"

class LoadBufferObject : public EOSObject
{
protected:
	Boolean		_needsFree;

	Uint8*		_buffer;
	Uint32		_bufferSize;

	Boolean		_locked;

public:
	LoadBufferObject();
	~LoadBufferObject();

	EOSError		allocateLoadBuffer(Uint32 bufferSize);
	EOSError		setLoadBuffer(Uint8* buffer, Uint32 bufferSize);

	inline Uint8*	getLoadBuffer(void) { return _buffer; }
	inline Uint32	getLoadBufferSize(void) { return _bufferSize; }

	void			lock(void);
	void			unlock(void);
	inline Boolean	isLocked(void) { return _locked; }
};

#endif /* __LOADBUFFEROBJECT_H__ */
