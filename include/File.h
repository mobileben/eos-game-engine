/******************************************************************************
 *
 * File: File.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * File. Basic file for the particular platform.
 * 
 *****************************************************************************/

#ifndef __FILE_H__
#define __FILE_H__

#include "Platform.h"
#include "Endian.h"
#include "EOSError.h"

typedef enum
{
	FileStreamTypeSynchronous = 0,	
	FileStreamTypeAsynchronous,
} FileStreamType;

typedef enum
{
	FileAccessTypeReadOnly = 0,
	FileAccessTypeWriteOnly,
	FileAccessTypeWriteAppend,
	FileAccessTypeReadWrite,
	FileAccessTypeReadWriteAppend,
	FileAccessTypeBinaryReadOnly,
	FileAccessTypeBinaryWriteOnly,
	FileAccessTypeBinaryWriteAppend,
	FileAccessTypeBinaryReadWrite,
	FileAccessTypeBinaryReadWriteAppend,
} FileAccessType;

typedef enum
{
	FileCreationTypeCreateAlways = 0,
	FileCreationTypeCreateNew,
	FileCreationTypeOpenAlways,
	FileCreationTypeOpenExisting,
	FileCreationTypeTruncateExisting,
} FileCreationType;

typedef enum
{
	FileSeekTypeBegin = 0,
	FileSeekTypeCurrent,
	FileSeekTypeEnd,
} FileSeekType;

class FileDescriptor : public EOSObject
{
private:
	FileStreamType	_streamType;
	FileAccessType	_accessType;
	Boolean			_shareFile;	//	For future expansion to allow multiple access to the file if the OS supports it	

	Boolean			_filenameNeedsFree;
	
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	NSString*		_filename;
	
#else
	
	UTF16*			_filename;

#endif /* _PLATFORM_MAC */
	
public:
	FileDescriptor();
	FileDescriptor(const FileDescriptor& p);

	~FileDescriptor();

	FileDescriptor& operator=(const FileDescriptor& desc);

	inline FileStreamType	getFileStreamType(void) { return _streamType; }
	inline FileAccessType	getFileAccessType(void) { return _accessType; }
	inline Boolean			isSharedFile(void) { return _shareFile; }

	void					setFilename(const Char* filename);

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	inline NSString*		getFilename(void) { return _filename; }
	void					setFilename(NSString* filename, Boolean needsFree = false);
	
#else
	
	inline UTF16*			getFilename(void) { return _filename; }
	void					setFilename(UTF16* filename, Boolean needsFree = false);

#endif /* _PLATFORM_MAC */
	
	inline void				setFileStreamType(FileStreamType stream) { _streamType = stream; }
	inline void				setFileAccessType(FileAccessType access) { _accessType = access; }
	inline void				setShareFile(Boolean share) { _shareFile = share; }
};

class File : public EOSFrameworkComponent
{
public:
	typedef enum
	{
		FileStateIllegal = 0,
		FileStateReady,
		FileStateOpening,
		FileStateClosing,
		FileStateReading,
		FileStateWriting,
	} FileState;

private:
	Sint32				_length;

	FileState			_fileState;

	FileDescriptor		_fileDescriptor;

	void*				_dataBuffer;

	EOSError			_error;

#ifdef _PLATFORM_PC

	HANDLE				_fileHandle;
	OVERLAPPED			_overlapped;
	DWORD				_actualTransfer;
	DWORD				_desiredTransfer;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	NSFileHandle*		_fileHandle;
	Uint32				_actualTransfer;
	Uint32				_desiredTransfer;
	
#else

	#error _PLATFORM must be defined.

#endif /* _PLATFORM_PC */

public:
	File();
	~File();

	EOSError			setFileDescriptor(FileDescriptor& desc);

	EOSError			open(FileDescriptor& desc);
	EOSError			open(void);  
	EOSError			close(void);

	static EOSError		deleteFile(FileDescriptor& desc);
	static Boolean		exists(FileDescriptor& desc);

	EOSError			seek(Sint32 offset, FileSeekType origin);

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	inline NSString*	getFilename(void) { return _fileDescriptor.getFilename(); }
	
#else
	
	const inline UTF16*	getFilename(void) { return _fileDescriptor.getFilename(); }

#endif /* _PLATFORM_MAC */
	
	inline Sint32		length(void) const { return _length; }
	Sint32				actualTransferred(void);

	Boolean				isOpened(void);

	EOSError			readSint8(Sint8* in);
	EOSError			readSint8(Sint8* buffer, Uint32 length);
	EOSError			readSint16(Sint16* in);
	EOSError			readSint16(Sint16* buffer, Uint32 length);
	EOSError			readSint32(Sint32* in);
	EOSError			readSint32(Sint32* buffer, Uint32 length);
	EOSError			readUint8(Uint8* in);
	EOSError			readUint8(Uint8* buffer, Uint32 length);
	EOSError			readUint16(Uint16* in);
	EOSError			readUint16(Uint16* buffer, Uint32 length);
	EOSError			readUint32(Uint32* in);
	EOSError			readUint32(Uint32* buffer, Uint32 length);

	EOSError			writeChar(const Char in);
	EOSError			writeChar(const Char* in);
	EOSError			writeChar(const Char* buffer, Uint32 length);
	EOSError			writeSint8(Sint8 in);
	EOSError			writeSint8(Sint8* in);
	EOSError			writeSint8(Sint8* buffer, Uint32 length);
	EOSError			writeSint16(Sint16 in);
	EOSError			writeSint16(Sint16* in);
	EOSError			writeSint16(Sint16* buffer, Uint32 length);
	EOSError			writeSint32(Sint32 in);
	EOSError			writeSint32(Sint32* in);
	EOSError			writeSint32(Sint32* buffer, Uint32 length);
	EOSError			writeUint8(Uint8 in);
	EOSError			writeUint8(Uint8* in);
	EOSError			writeUint8(Uint8* buffer, Uint32 length);
	EOSError			writeUint16(Uint16 in);
	EOSError			writeUint16(Uint16* in);
	EOSError			writeUint16(Uint16* buffer, Uint32 length);
	EOSError			writeUint32(Uint32 in);
	EOSError			writeUint32(Uint32* in);
	EOSError			writeUint32(Uint32* buffer, Uint32 length);

	Boolean				isComplete(EOSError& error);
};

#endif /* __FILE_H__ */

