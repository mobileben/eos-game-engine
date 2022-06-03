/******************************************************************************
 *
 * File: LoadBufferObject.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Load Buffer Object
 * 
 *****************************************************************************/

#include "Platform.h"
#include "LoadBufferObject.h"

LoadBufferObject::LoadBufferObject() : _needsFree(false), _buffer(NULL), _bufferSize(0), _locked(false)
{
}

LoadBufferObject::~LoadBufferObject()
{
	if (_needsFree)
	{
		if (_buffer)
		{
			delete _buffer;
			_buffer = NULL;
		}

		_bufferSize = 0;
	}

	_locked = false;
	_needsFree = false;
}

EOSError LoadBufferObject::allocateLoadBuffer(Uint32 bufferSize)
{
	EOSError error = EOSErrorNone;

	if (_needsFree)
	{
		if (_buffer)
			delete _buffer;
	}

	_needsFree = false;
	_bufferSize = 0;

	_buffer = new Uint8[bufferSize];

	if (_buffer)
	{
		_needsFree = true;
		_bufferSize = bufferSize;
	}
	else
	{
		error = EOSErrorNoMemory;
	}

	_locked = false;

	return error;
}

EOSError LoadBufferObject::setLoadBuffer(Uint8* buffer, Uint32 bufferSize)
{
	EOSError error = EOSErrorNone;

	if (_needsFree)
	{
		if (_buffer)
			delete _buffer;
	}

	_needsFree = false;
	_buffer = buffer;
	_bufferSize = bufferSize;
	_locked = false;

	return error;
}

void LoadBufferObject::lock(void)
{
	AssertWDesc(_locked == false, "LoadBufferObject::lock() trying to lock an already locked buffer.");
	_locked = true;
}

void LoadBufferObject::unlock(void)
{
	AssertWDesc(_locked == true, "LoadBufferObject::unlock() trying to unlock an already unlocked buffer.");
	_locked = false;
}

