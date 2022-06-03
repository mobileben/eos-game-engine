/******************************************************************************
 *
 * File: DTypes.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Platform/device specific details
 * 
 *****************************************************************************/

#ifndef __DTYPES_H__
#define __DTYPES_H__

#include "Platform.h"

typedef long long int		Sint64;
typedef unsigned long long	Uint64;

typedef unsigned int		Uint32;
typedef signed int			Sint32;
typedef unsigned short		Uint16;
typedef signed short		Sint16;
typedef unsigned char		Uint8;
typedef signed char			Sint8;

#if !defined(_PLATFORM_MAC) && !defined(_PLATFORM_IPHONE)

typedef	bool				Boolean;

#endif /* _PLATFORM_MAC */

typedef float				Float32;
typedef double 				Double64;

#ifdef _PLATFORM_PC

typedef char				Char;
typedef Uint8				UChar;

typedef char				ANSIChar;
typedef char				UTF8;
typedef wchar_t				UTF16;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

typedef char				Char;
#ifdef _NOT_YET		// conflict with libxml on iPhone/Mac
typedef Uint8				UChar;
#endif /* _NOT_YET */

typedef char				ANSIChar;
typedef char				UTF8;
typedef unichar				UTF16;

#else

	#error _PLATFORM not defined.
	 
#endif /* _PLATFORM_PC */

//	Define addressing modes, must be defined or else

#ifdef _64BIT_ADDRESSING

typedef Uint64		Address;
typedef Uint64		AddressOffset;
typedef Uint64		ObjectID;

const Uint64 InvalidObjectID = 0xFFFFFFFFFFFFFFFF;
const Uint64 UndefinedObjectID = 0xFFFFFFFFFFFFFFFE; 

#elif defined(_32BIT_ADDRESSING)

typedef Uint32		Address;
typedef Uint32		AddressOffset;
typedef Uint32		ObjectID;

const Uint32 InvalidObjectID = 0xFFFFFFFF;
const Uint32 UndefinedObjectID = 0xFFFFFFFE;

#elif defined(_16BIT_ADDRESSING)

typedef Uint16		Address;
typedef Uint16		AddressOffset;
typedef Uint16		ObjectID;

const Uint16 InvalidObjectID = 0xFFFF;
const Uint16 UndefinedObjectID = 0xFFFE;

#else

#error BIT_ADDRESSING must be assigned.

#endif /* _64BIT_ADDRESSING */

typedef struct
{
	Uint16	year;
	Uint8	month;
	Uint8	day;
} Date;

typedef struct
{
	Uint8	sec;
	Uint8	min;
	Uint8	hour;
	Uint8	pad;
} Time;

const unsigned int	SECONDS_PRECISION = 8;

typedef Uint32 MicroSeconds;
typedef Uint32 MilliSeconds;
typedef Uint32 Seconds;

inline Seconds IntSecondsToSeconds(Uint32 value) { return value << SECONDS_PRECISION; }
inline Uint32 SecondsToIntSecond(Seconds value) { return value >> SECONDS_PRECISION; }

inline MicroSeconds SecondsToMicroSeconds(Seconds sec) { return (MicroSeconds) (((Sint64) sec * 1000000) >> SECONDS_PRECISION); }
inline Seconds MicroSecondsToSeconds(MicroSeconds microsec) { return (Seconds) (((Sint64) microsec << SECONDS_PRECISION) / 1000000); }

const unsigned int FRAMES_PER_SECOND = 30; 
const unsigned int ANIM_PRECISION = 8;  

typedef Uint32	FixedFrame;


#endif /* __DTYPES_H__ */

