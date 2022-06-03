/******************************************************************************
 *
 * File: FileLoadTask.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * File Load Task Base Class
 * 
 *****************************************************************************/

#include "FileLoadTask.h"

Boolean FileLoadTask::_useBlockXferSize = false;
Uint32 FileLoadTask::_blockXferPtr = 0;
Uint32 FileLoadTask::_blockXferingSize = 0;
Uint32 FileLoadTask::_blockXferSize = 0;

FileLoadTask::FileLoadTask()
{
	_needsFree = false;
	_buffer = NULL;
	_bufferSize = 0;
	_actualSize = 0;
}

FileLoadTask::~FileLoadTask()
{
	if (_buffer)
	{
		if (_needsFree)
			delete _buffer;

		_buffer = NULL;
		_bufferSize = 0;
		_actualSize = 0;
		_needsFree = false;
	}
}

void FileLoadTask::setFileDescriptor(FileDescriptor& desc)
{
	_file.setFileDescriptor(desc);
}

void FileLoadTask::setLoadBuffer(void* buffer, Uint32 bufferSize, Boolean needsFree)
{
	AssertWDesc(buffer != NULL, "buffer is NULL");

	_buffer = (Uint8*) buffer;
	_bufferSize = bufferSize;
	_needsFree = needsFree;
}

void FileLoadTask::maxFileTransferSize(void)
{
	_useBlockXferSize = false;
	_blockXferSize = 0;
}

void FileLoadTask::setBlockTransferSize(Uint32 size)
{
	if (size == 0)
		_useBlockXferSize = false;
	else
	{
		_useBlockXferSize = true;
		_blockXferSize = size;
	}
}

Uint32 FileLoadTask::getFileSize(void)
{
	Uint32	len = 0;

	if (_file.isOpened())
		len = _actualSize = _file.length();
	else
		len = _actualSize;

	return len;
}

EOSError FileLoadTask::prologue(void)
{
	EOSError	error = EOSErrorNone;

	AssertWDesc(_buffer != NULL, "_buffer is NULL.");

	if (_buffer && _bufferSize)
	{
		error = _file.open();

		if (error == EOSErrorNone)
		{
			_actualSize = _file.length();

			if (_bufferSize >= _actualSize)
			{
				if (_useBlockXferSize)
				{
					_blockXferPtr = 0;

					if (_actualSize <= _blockXferSize)
						_blockXferingSize = _actualSize;
					else
						_blockXferingSize = _blockXferSize;

					error = _file.readUint8(&_buffer[_blockXferPtr], _blockXferingSize);
				}
				else
					error = _file.readUint8(_buffer, _actualSize);
			}
			else
				error = EOSErrorOutOfBounds;
		}
	}
	else
		error = EOSErrorNoDefinedBuffer;

	return error;
}

EOSError FileLoadTask::epilogue(void)
{
	EOSError 	error = EOSErrorNone;

	return error;
}

Boolean FileLoadTask::isComplete(EOSError& error)
{
	Boolean	completed = false;

	error = EOSErrorNone;

	if (_file.isOpened())
	{
		if (_file.isComplete(error))
		{
			//	Do check here for file sizes
			if (_useBlockXferSize)
			{
				if ((_blockXferPtr + _blockXferingSize) >= _actualSize)
				{
					//	We are done
					_file.close();

					completed = true;
				}
				else
				{
					_blockXferPtr += _blockXferingSize;

					if ((_blockXferPtr + _blockXferingSize) >= _actualSize)
						_blockXferingSize = _actualSize - _blockXferPtr;
					else
						_blockXferingSize = _blockXferSize;

					error = _file.readUint8(&_buffer[_blockXferPtr], _blockXferingSize);
				}
			}
			else
			{
				_file.close();
	
				completed = true;
			}
		}
	}
	else
	{
		completed = true;
		error = EOSErrorResourceNotOpened;
	}

	return completed;
}

