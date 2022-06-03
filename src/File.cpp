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

#include "File.h"

FileDescriptor::FileDescriptor()
{
	_streamType = FileStreamTypeSynchronous;
	_accessType = FileAccessTypeReadOnly;
	_shareFile = false;
	_filenameNeedsFree = false;
	_filename = NULL;
}

FileDescriptor::FileDescriptor(const FileDescriptor& p)
{
	_streamType = p._streamType;
	_accessType = p._accessType;
	_shareFile = p._shareFile;
	_filenameNeedsFree = p._filenameNeedsFree;

	if (p._filename)
	{
		if (_filenameNeedsFree)
		{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
			
			_filename = [[NSString alloc] initWithString:p._filename];
#else

			_filename = new UTF16[wcslen(p._filename) + 1];
			wcscpy_s(_filename, wcslen(p._filename) + 1, p._filename);

#endif /* _PLATFORM_MAC */
			
		}
		else
			_filename = p._filename;
	}
	else
		_filename = NULL;
}

FileDescriptor::~FileDescriptor()
{
	if (_filename)
	{
		if (_filenameNeedsFree)
		{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
			
			[_filename release];
			_filename = nil;
		
#else
			
			delete _filename;
			
#endif /* _PLATFORM_MAC */
		}
		
		_filename = NULL;
		_filenameNeedsFree = false;
	}
}

FileDescriptor& FileDescriptor::operator=(const FileDescriptor& desc)
{
	if (this != &desc)
	{
		_streamType = desc._streamType;
		_accessType = desc._accessType;
		_shareFile = desc._shareFile;
		_filenameNeedsFree = desc._filenameNeedsFree;

		if (desc._filename)
		{
			if (_filenameNeedsFree)
			{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
				
				_filename = [[NSString alloc] initWithString:desc._filename];
#else
				_filename = new UTF16[wcslen(desc._filename) + 1];
				wcscpy_s(_filename, wcslen(desc._filename) + 1, desc._filename);
#endif /* _PLATFORM_MAC */
			}
			else
				_filename = desc._filename;
		}
		else
			_filename = NULL;
	}

	return *this;
}

void FileDescriptor::setFilename(const Char* filename)
{
	AssertWDesc(filename != NULL, "filename NULL");

	if (filename)
	{
		if (_filename)
		{
			if (_filenameNeedsFree)
			{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

				[_filename release];
				_filename = nil;

#else

				delete _filename;

#endif /* _PLATFORM_MAC */
			}

			_filename = NULL;
		}

		_filenameNeedsFree = true;

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
			
		_filename = [[NSString alloc] initWithCString:filename encoding:NSUTF8StringEncoding];
#else

		_filename = new UTF16[strlen(filename) + 1];

		if (_filename)
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, filename, -1, _filename, (int) strlen(filename) + 1);
			
#endif /* _PLATFORM_MAC */

	}
}

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

void FileDescriptor::setFilename(NSString* filename, Boolean needsFree)
#else

void FileDescriptor::setFilename(UTF16* filename, Boolean needsFree)

#endif /* _PLATFORM_MAC */
{
	//	We copy this over just in case the user allocates the filename locallly within a function
	AssertWDesc(filename != NULL, "filename NULL");

	if (filename)
	{
		if (_filename)
		{
			if (_filenameNeedsFree)
			{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
				
				[_filename release];
				_filename = nil;
				
#else
				
				delete _filename;
				
#endif /* _PLATFORM_MAC */
			}

			_filename = NULL;
		}

		_filenameNeedsFree = needsFree;

		if (needsFree)
		{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
			
			_filename = [[NSString alloc] initWithString:filename];
#else

			_filename = new UTF16[wcslen(filename) + 1];

			if (_filename)
				wcscpy_s(_filename, wcslen(filename) + 1, filename);
			
#endif /* _PLATFORM_MAC */
		}
		else
			_filename = filename;
	}
}

File::File()
{
	_length = 0;
	_fileState = FileStateIllegal;
	_dataBuffer = NULL;
	_error = EOSErrorNone;

#ifdef _PLATFORM_PC

	_fileHandle = INVALID_HANDLE_VALUE;

	ZeroMemory(&_overlapped, sizeof(_overlapped));
	
#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	_fileHandle = nil;
	
#else

	#error _PLATFORM must be defined.

#endif /* _PLATFORM_PC */
}

File::~File()
{
	close();
}

EOSError File::setFileDescriptor(FileDescriptor& desc)
{
	_error = EOSErrorNone;

	if (_fileState == FileStateIllegal)
		_fileDescriptor = desc;
	else
		_error = EOSErrorAlreadyInitialized;

	return _error;
}

EOSError File::open(void)
{
	_error = EOSErrorNone;

	if (_fileState == FileStateIllegal)
	{
		AssertWDesc(_fileDescriptor.getFilename() != NULL, "filename is NULL.");

		if (_fileDescriptor.getFilename() != NULL)
		{
			_fileState = FileStateOpening;
	
#ifdef _PLATFORM_PC
			Sint32	access = 0;
			Sint32	share = 0;
			Sint32	disposition = OPEN_EXISTING;
	
			switch (_fileDescriptor.getFileAccessType())
			{
				case FileAccessTypeReadOnly:
				case FileAccessTypeBinaryReadOnly:
					access = GENERIC_READ;
					break;
	
				case FileAccessTypeWriteAppend:
				case FileAccessTypeBinaryWriteAppend:
					access = GENERIC_WRITE;
					break;

				case FileAccessTypeWriteOnly:
				case FileAccessTypeBinaryWriteOnly:
					access = GENERIC_WRITE;
					disposition = CREATE_ALWAYS;
					break;
	
				case FileAccessTypeReadWriteAppend:
				case FileAccessTypeBinaryReadWriteAppend:
					access = GENERIC_READ | GENERIC_WRITE;
					break;

				case FileAccessTypeReadWrite:
				case FileAccessTypeBinaryReadWrite:
					access = GENERIC_READ | GENERIC_WRITE;
					disposition = CREATE_ALWAYS;
					break;
	
				default:
					AssertWDesc(0 == 1, "Unsupported ACCESS mode.");
					break;
			}
	
			if (_fileDescriptor.isSharedFile() == true)
			{
				switch (_fileDescriptor.getFileAccessType())
				{
					case FileAccessTypeReadOnly:
					case FileAccessTypeBinaryReadOnly:
						share = FILE_SHARE_READ;
						break;
	
					case FileAccessTypeWriteOnly:
					case FileAccessTypeWriteAppend:
					case FileAccessTypeBinaryWriteOnly:
					case FileAccessTypeBinaryWriteAppend:
						share = FILE_SHARE_WRITE;
						break;
	
					case FileAccessTypeReadWrite:
					case FileAccessTypeReadWriteAppend:
					case FileAccessTypeBinaryReadWrite:
					case FileAccessTypeBinaryReadWriteAppend:
						share = FILE_SHARE_READ | FILE_SHARE_WRITE;
						break;
	
					default:
						AssertWDesc(0 == 1, "Unsupported ACCESS mode.");
						break;
				}
			}
			else
				share = 0;
	
			Sint32	flag = FILE_ATTRIBUTE_NORMAL;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				flag |= FILE_FLAG_OVERLAPPED;

			_fileHandle = CreateFile(_fileDescriptor.getFilename(), access, share,
							NULL, disposition, flag, NULL);
	
			AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "Could not create file handle.");
	
			if (_fileHandle != INVALID_HANDLE_VALUE)
			{
				_fileState = FileStateReady;
	
				_length = GetFileSize(_fileHandle, NULL);
			}
			else
				_error = EOSErrorCouldNotOpenResource;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

			Boolean			exists_as_needed = false;
			NSFileManager*	fileMgr;
#ifdef _PLATFORM_MAC			
			NSString*		bundleFilename = _fileDescriptor.getFilename();
#else
			NSString*		bundleFilename = [[NSBundle mainBundle] pathForResource:_fileDescriptor.getFilename() ofType:nil];
			NSArray*		paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
			NSString*		documentsDirectory = [paths objectAtIndex:0];
			NSString*		documentsFilename = [documentsDirectory stringByAppendingPathComponent:_fileDescriptor.getFilename()];
#endif /* _PLATFORM_MAC */
			NSString*		actualFilename = bundleFilename;
			
			fileMgr = [NSFileManager defaultManager];
			
			switch (_fileDescriptor.getFileAccessType())
			{
				case FileAccessTypeReadOnly:
				case FileAccessTypeBinaryReadOnly:
					if ([fileMgr isReadableFileAtPath:bundleFilename] == YES)
						exists_as_needed = true;
#ifdef _PLATFORM_IPHONE
					else if ([fileMgr isReadableFileAtPath:documentsFilename] == YES)
					{
						exists_as_needed = true;
						actualFilename = documentsFilename;
					}
#endif /* _PLATFORM_IPHONE */
					break;
					
				case FileAccessTypeWriteAppend:
				case FileAccessTypeBinaryWriteAppend:
				case FileAccessTypeWriteOnly:
				case FileAccessTypeBinaryWriteOnly:
					if ([fileMgr isWritableFileAtPath:bundleFilename] == YES)
						exists_as_needed = true;
#ifdef _PLATFORM_IPHONE
					else if ([fileMgr isWritableFileAtPath:documentsFilename] == YES)
					{
						exists_as_needed = true;
						actualFilename = documentsFilename;
					}
#endif /* _PLATFORM_IPHONE */
					break;
					
				case FileAccessTypeReadWriteAppend:
				case FileAccessTypeBinaryReadWriteAppend:
				case FileAccessTypeReadWrite:
				case FileAccessTypeBinaryReadWrite:
					if ([fileMgr isReadableFileAtPath:bundleFilename] == YES && [fileMgr isWritableFileAtPath:bundleFilename] == YES)
						exists_as_needed = true;
#ifdef _PLATFORM_IPHONE
					else if ([fileMgr isReadableFileAtPath:documentsFilename] == YES && [fileMgr isWritableFileAtPath:documentsFilename] == YES)
					{
						exists_as_needed = true;
						actualFilename = documentsFilename;
					}
#endif /* _PLATFORM_IPHONE */
					break;
					
				default:
					AssertWDesc(0 == 1, "Unsupported ACCESS mode.");
					break;
			}
			
			if (exists_as_needed == true)
			{
				//	We ignore share file details
				switch (_fileDescriptor.getFileAccessType())
				{
					case FileAccessTypeReadOnly:
					case FileAccessTypeBinaryReadOnly:
						_fileHandle = [NSFileHandle fileHandleForReadingAtPath:actualFilename];
						break;
						
					case FileAccessTypeWriteAppend:
					case FileAccessTypeBinaryWriteAppend:
					case FileAccessTypeWriteOnly:
					case FileAccessTypeBinaryWriteOnly:
						_fileHandle = [NSFileHandle fileHandleForWritingAtPath:actualFilename];
						break;
						
					case FileAccessTypeReadWriteAppend:
					case FileAccessTypeBinaryReadWriteAppend:
					case FileAccessTypeReadWrite:
					case FileAccessTypeBinaryReadWrite:
						_fileHandle = [NSFileHandle fileHandleForUpdatingAtPath:actualFilename];
						break;
						
					default:
						AssertWDesc(0 == 1, "Unsupported ACCESS mode.");
						break;
				}
			}
			else
			{
				switch (_fileDescriptor.getFileAccessType())
				{
					case FileAccessTypeWriteAppend:
					case FileAccessTypeBinaryWriteAppend:
					case FileAccessTypeWriteOnly:
					case FileAccessTypeBinaryWriteOnly:
					case FileAccessTypeReadWriteAppend:
					case FileAccessTypeBinaryReadWriteAppend:
					case FileAccessTypeReadWrite:
					case FileAccessTypeBinaryReadWrite:
#ifdef _PLATFORM_IPHONE						
						if ([fileMgr createFileAtPath:documentsFilename contents:nil attributes:nil] == YES)
						{
							_fileHandle = [NSFileHandle fileHandleForWritingAtPath:documentsFilename];
							actualFilename = documentsFilename;
						}
#else
						if ([fileMgr createFileAtPath:actualFilename contents:nil attributes:nil] == YES)
						{
							_fileHandle = [NSFileHandle fileHandleForWritingAtPath:actualFilename];
						}
#endif /* _PLATFORM_IPHONE */
						break;

					default:
						_error = EOSErrorCouldNotOpenResource;
						break;
				}
			}
			
			AssertWDesc(_fileHandle != nil, "Could not create file.");
			
			if (_fileHandle != nil)
			{
				NSDictionary*	attributes = [fileMgr fileAttributesAtPath:actualFilename traverseLink:YES];
				
				[_fileHandle retain];
				
				if (attributes != nil)
				{
					NSNumber*	fileSize;
					
					fileSize = [attributes objectForKey:NSFileSize];
					
					_fileState = FileStateReady;
					
					_length = [fileSize intValue];
				}
			}
			else
				_error = EOSErrorCouldNotOpenResource;
#else
	
	#error _PLATFORM must be defined.
	
#endif /* _PLATFORM_PC */
	
		}
		else
			_error = EOSErrorResourceHasNoName;
	}
	else
		_error = EOSErrorAlreadyInitialized;

	return _error;
}

EOSError File::open(FileDescriptor& desc)
{
	_error = EOSErrorNone;

	if (_fileState == FileStateIllegal)
	{
		setFileDescriptor(desc);
		_error = open();
	}
	else
		_error = EOSErrorAlreadyInitialized;

	return _error;
}

EOSError File::deleteFile(FileDescriptor& desc)
{
	EOSError	error = EOSErrorNone;
	
	if (exists(desc))
	{
#ifdef _PLATFORM_PC
		 if (DeleteFile(desc.getFilename()) == FALSE)
			 error = EOSErrorResourceRemovalFailure;
#elif defined (_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
		NSFileManager*	fileMgr = [NSFileManager defaultManager];
#ifdef _PLATFORM_MAC
		NSString*		filename = desc.getFilename();
#else
		NSArray*		paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString*		documentsDirectory = [paths objectAtIndex:0];
		NSString*		filename = [documentsDirectory stringByAppendingPathComponent:desc.getFilename()];
#endif /* _PLATFORM_MAC */

		//	Redundant test, but mainly needed for iPhone, since exists validates existence in bundle as well, and we don't
		//	want to delete items in bundle
		if ([fileMgr fileExistsAtPath:filename] == YES)
		{
			if ([fileMgr removeItemAtPath:filename error:nil] != YES)
				error = EOSErrorResourceRemovalFailure;
		}
		else
			error = EOSErrorResourceDoesNotExist;
		
#endif /* _PLATFORM_PC */
	}
	else
		error = EOSErrorResourceDoesNotExist;
	
	return error;
}

Boolean	File::exists(FileDescriptor& desc)
{
	Boolean present = false;

	if (desc.getFilename())
	{
#ifdef _PLATFORM_PC
		WIN32_FIND_DATA	data;

		if (FindFirstFile(desc.getFilename(), &data) != INVALID_HANDLE_VALUE)
			present = true;
#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
		NSFileManager*	fileMgr;

#ifdef _PLATFORM_MAC	
		NSString*		bundleFilename = desc.getFilename();
#else
		NSString*		bundleFilename = [[NSBundle mainBundle] pathForResource:desc.getFilename() ofType:nil];
		NSArray*		paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString*		documentsDirectory = [paths objectAtIndex:0];
		NSString*		documentsFilename = [documentsDirectory stringByAppendingPathComponent:desc.getFilename()];
#endif /* _PLATFORM_MAC */

		fileMgr = [NSFileManager defaultManager];

		if ([fileMgr fileExistsAtPath:bundleFilename] == YES)
			present = true;
#ifdef _PLATFORM_IPHONE
		else if ([fileMgr fileExistsAtPath:documentsFilename] == YES)
			present = true;
#endif /* _PLATFORM_IPHONE */
#endif /* _PLATFORM_PC */
	}

	return present;
}

EOSError File::close(void)
{
#ifdef _PLATFORM_PC

	if (_fileHandle != INVALID_HANDLE_VALUE)
	{
		_fileState = FileStateClosing;

		CloseHandle(_fileHandle);

		_fileHandle = INVALID_HANDLE_VALUE;

		_fileState = FileStateIllegal;

		_length = 0;

		return EOSErrorNone;
	}
	else
		return EOSErrorResourceNotOpened;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	if (_fileHandle != nil)
	{
		_fileState = FileStateClosing;
		
		[_fileHandle closeFile];
		
		[_fileHandle release];
		
		_fileHandle = nil;
		
		_fileState = FileStateIllegal;
		
		_length = 0;
		
		return EOSErrorNone;
	}
	else
		return EOSErrorResourceNotOpened;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::seek(Sint32 offset, FileSeekType origin)
{
#ifdef _PLATFORM_PC

	if (_fileHandle != INVALID_HANDLE_VALUE)
	{
		Sint32	move = 0;
		
		switch (origin)
		{
			case FileSeekTypeBegin:
				move = FILE_BEGIN;
				break;
				
			case FileSeekTypeCurrent:
				move = FILE_CURRENT;
				break;
				
			case FileSeekTypeEnd:
				move = FILE_END;
				break;
				
			default:
				AssertWDesc(0 == 1, "Illegal seek type.");
				break;
		}
		
		SetFilePointer(_fileHandle, offset, NULL, move);
		
		return EOSErrorNone;
	}
	else
		return EOSErrorResourceNotOpened;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	if (_fileHandle != nil)
	{
		unsigned long long seekPos;
		
		switch (origin)
		{
			case FileSeekTypeBegin:
				seekPos = offset;
				break;
				
			case FileSeekTypeCurrent:
				seekPos = [_fileHandle offsetInFile];
				seekPos += offset;
				break;
				
			case FileSeekTypeEnd:
				seekPos = _length - offset;
				break;
				
			default:
				AssertWDesc(0 == 1, "Illegal seek type.");
				break;
		}
		
		[_fileHandle seekToFileOffset:seekPos];
		
		return EOSErrorNone;
	}
	else
		return EOSErrorResourceNotOpened;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

Sint32 File::actualTransferred(void)
{
#ifdef _PLATFORM_PC

	return (Sint32) _actualTransfer;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	return (Sint32) _actualTransfer;
	
#else

	#error _PLATFORM must be defined.

	return 0;

#endif /* _PLATFORM_PC */
}

Boolean File::isOpened(void)
{
	if (_fileState != FileStateIllegal)
		return true;
	else
		return false;
}

EOSError File::readSint8(Sint8* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint8);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint8);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readSint8(Sint8* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;
	
#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
		
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readSint16(Sint16* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint16);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint16);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readSint16(Sint16* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readSint32(Sint32* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint32);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint32);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readSint32(Sint32* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Sint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readUint8(Uint8* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint8);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint8);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readUint8(Uint8* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readUint16(Uint16* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint16);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint16);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readUint16(Uint16* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readUint32(Uint32* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint32);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint32);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::readUint32(Uint32* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!ReadFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			NSData*	data;
			
			_error = EOSErrorNone;
			
			_fileState = FileStateReading;
			_desiredTransfer = sizeof(Uint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			data = [_fileHandle readDataOfLength:_desiredTransfer];
			
			if (data != nil)
			{
				_actualTransfer = [data length];
				
				if (_actualTransfer == _desiredTransfer)
				{
					[data getBytes:_dataBuffer length:_actualTransfer];
					
					_fileState = FileStateReady;
				}
				else
				{
					_error = EOSErrorResourceRead;
					_fileState = FileStateReady;
				}
			}
			else
			{
				_error = EOSErrorResourceRead;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}


EOSError File::writeChar(Char in)
{
#ifdef _PLATFORM_PC
	
	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Char);
			_actualTransfer = 0;
			_dataBuffer = &in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Char);
			_actualTransfer = 0;
			_dataBuffer = &in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;

			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeChar(const Char* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Char);
			_actualTransfer = 0;
			_dataBuffer = (void*) in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Char);
			_actualTransfer = 0;
			_dataBuffer = (void*) in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeChar(const Char* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Char) * length;
			_actualTransfer = 0;
			_dataBuffer = (void*) buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Char) * length;
			_actualTransfer = 0;
			_dataBuffer = (void*) buffer;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint8(Sint8 in)
{
#ifdef _PLATFORM_PC
	
	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint8);
			_actualTransfer = 0;
			_dataBuffer = &in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint8);
			_actualTransfer = 0;
			_dataBuffer = &in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint8(Sint8* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint8);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint8);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint8(Sint8* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint16(Sint16 in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint16);
			_actualTransfer = 0;
			_dataBuffer = &in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint16);
			_actualTransfer = 0;
			_dataBuffer = &in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint16(Sint16* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint16);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint16);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint16(Sint16* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint32(Sint32 in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint32);
			_actualTransfer = 0;
			_dataBuffer = &in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint32);
			_actualTransfer = 0;
			_dataBuffer = &in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint32(Sint32* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint32);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint32);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeSint32(Sint32* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Sint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint8(Uint8 in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint8);
			_actualTransfer = 0;
			_dataBuffer = &in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint8);
			_actualTransfer = 0;
			_dataBuffer = &in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint8(Uint8* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint8);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint8);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint8(Uint8* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint8) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint16(Uint16 in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint16);
			_actualTransfer = 0;
			_dataBuffer = &in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint16);
			_actualTransfer = 0;
			_dataBuffer = &in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint16(Uint16* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint16);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint16);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint16(Uint16* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint16) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint32(Uint32 in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint32);
			_actualTransfer = 0;
			_dataBuffer = &in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint32);
			_actualTransfer = 0;
			_dataBuffer = &in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint32(Uint32* in)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint32);
			_actualTransfer = 0;
			_dataBuffer = in;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint32);
			_actualTransfer = 0;
			_dataBuffer = in;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

EOSError File::writeUint32(Uint32* buffer, Uint32 length)
{
#ifdef _PLATFORM_PC

	OVERLAPPED*	overlap = NULL;

	AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined");

	if (_fileHandle)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;

			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;

			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				overlap = &_overlapped;

			if (!WriteFile(_fileHandle, _dataBuffer, _desiredTransfer, &_actualTransfer, overlap))
			{
				if (GetLastError() != ERROR_IO_PENDING)
				{
					_error = EOSErrorResourceWrite;
					_fileState = FileStateReady;
				}
			}
			else
				_fileState = FileStateReady;
		}
	}
	else
		_error = EOSErrorResourceNotOpened;

	return _error;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	AssertWDesc(_fileHandle != nil, "_fileHandle not defined");
	
	if (_fileHandle != nil)
	{
		if (_fileState == FileStateReady)
		{
			_error = EOSErrorNone;
			
			_fileState = FileStateWriting;
			_desiredTransfer = sizeof(Uint32) * length;
			_actualTransfer = 0;
			_dataBuffer = buffer;
			
			NSData*	data = [[NSData alloc] initWithBytesNoCopy:_dataBuffer length:_desiredTransfer];
			
			//	For now asynch not handled yet
			if (_fileDescriptor.getFileStreamType() == FileStreamTypeAsynchronous)
				;
			
			if (data != nil)
			{
				[_fileHandle writeData:data];
				
				_actualTransfer = [data length];
				_fileState = FileStateReady;
				
				[data release];
				data = nil;
			}
			else
			{
				_error = EOSErrorResourceWrite;
				_fileState = FileStateReady;
			}
		}
	}
	else
		_error = EOSErrorResourceNotOpened;
	
	return _error;
	
#else

	#error _PLATFORM must be defined.

	return EOSErrorUnsupported;

#endif /* _PLATFORM_PC */
}

Boolean File::isComplete(EOSError& error)
{
	Boolean	done = false;

	switch (_fileState)
	{
		case FileStateReading:
		case FileStateWriting:
#ifdef _PLATFORM_PC
			AssertWDesc(_fileHandle != INVALID_HANDLE_VALUE, "_fileHandle not defined.");

			if (_fileHandle != INVALID_HANDLE_VALUE)
			{
				if (GetOverlappedResult(_fileHandle, &_overlapped, &_actualTransfer, FALSE))
				{
					done = true;
				}
				else
				{
					if (GetLastError() != ERROR_IO_INCOMPLETE)
					{
						if (_fileState == FileStateReading)
							_error = EOSErrorResourceRead;
						else
							_error = EOSErrorResourceWrite;
	
						done = true;
					}
				}
			}
			else
			{
				_error = EOSErrorResourceNotOpened;
				done = true;
			}
			
#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

			AssertWDesc(_fileHandle != nil, "_fileHandle not defined.");
			
			if (_fileHandle != nil)
			{
				//	For now, async not supported
				done = true;
			}
			else
			{
				_error = EOSErrorResourceNotOpened;
				done = true;
			}

#else

	#error _PLATFORM must be defined.

#endif /* _PLATFORM_PC */
			break;

		case FileStateOpening:
			break;

		case FileStateClosing:
			break;

		default:
			done = true;
			break;
	}

	return done;
}

