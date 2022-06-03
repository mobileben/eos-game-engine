/******************************************************************************
 *
 * File: FileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __FILELOADTASK_H__
#define __FILELOADTASK_H__

#include "Platform.h"
#include "Task.h"
#include "File.h"

class FileLoadTask : public Task
{
protected:
	File 			_file;

	Boolean			_needsFree;

	Uint8*			_buffer;
	Uint32			_bufferSize;

	Uint32			_actualSize;

	static Boolean	_useBlockXferSize;

	static Uint32	_blockXferSize;
	static Uint32	_blockXferPtr;
	static Uint32	_blockXferingSize;

public:
	FileLoadTask();
	~FileLoadTask();

	void					setFileDescriptor(FileDescriptor& desc);

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	inline NSString*		getFilename(void) { return _file.getFilename(); }
	
#else
	
	const inline UTF16*		getFilename(void) { return _file.getFilename(); }
	
#endif /* _PLATFORM_MAC */
	
	void					setLoadBuffer(void* buffer, Uint32 bufferSize, Boolean needsFree);
	static void				maxFileTransferSize(void);
	static void				setBlockTransferSize(Uint32 size);

	Uint32					getFileSize(void);

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
	virtual Boolean			isComplete(EOSError& error);
};

#endif /* __FILELOADTASK_H__ */

