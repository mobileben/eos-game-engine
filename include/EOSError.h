/******************************************************************************
 *
 * File: EOSError.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Error values.
 * 
 *****************************************************************************/

#ifndef __EOSERROR_H__
#define __EOSERROR_H__

typedef enum
{
	EOSErrorNone = 0,

	EOSErrorNULL = 1,

	EOSErrorUnsupported = 2,

	EOSErrorAlreadyInitialized = 3,

	EOSErrorUnallocated = 4,

	EOSErrorNotInitialized = 5,

	EOSErrorResourceCreationFailure = 6,

	EOSErrorNoMemory = 7,

	EOSErrorOutOfBounds = 8,

	EOSErrorCouldNotOpenResource = 9,

	EOSErrorResourceNotOpened = 10,

	EOSErrorResourceOpen = 11,

	EOSErrorResourceClose = 12,

	EOSErrorResourceRead = 13,

	EOSErrorResourceWrite = 14,

	EOSErrorResourceDataMismatch = 15,

	EOSErrorResourceHasNoName = 16,

	EOSErrorNoDefinedBuffer = 17,

	EOSErrorResourceDoesNotExist = 18,

	EOSErrorResourceNotAvailable = 19,

	EOSErrorIllegalArguments = 20,

	EOSErrorIllegalGrammar = 21,

	EOSErrorBufferSize = 22,

	EOSErrorIllegalValue = 23,
	
	EOSErrorResourceRemovalFailure = 24,

	EOSErrorUnknown = 25,

} EOSError;

#endif /* __EOSERROR_H__ */

