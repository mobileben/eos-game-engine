/******************************************************************************
 *
 * File: Debug.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Debug
 * 
 *****************************************************************************/

#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef _DEBUG

#include "Platform.h"

#include <assert.h>

extern Boolean EOSAssert(Boolean test, const Char* desc, Uint32 line, const Char* filename);
extern Boolean EOSAssertOneShot(Boolean test, const Char* desc, Uint32 line, const Char* filename);

#define Assert(x) \
	if (EOSAssert((x), NULL, __LINE__, __FILE__)) \
	{ \
	}

#define AssertWDesc(x, y) if (EOSAssert((x), (y), __LINE__, __FILE__)) \
	{ \
	}

#define AssertOneShot(x) \
{ \
	static Boolean	ignoreAlways = false; \
\
	if (!ignoreAlways) \
	{ \
		if (EOSAssertOneShot((x), NULL, __LINE__, __FILE__)) \
		{ \
		} \
	} \
}

#define AssertWDescOneShot(x, y) \
{ \
	static Boolean	ignoreAlways = false; \
\
	if (!ignoreAlways) \
	{ \
		if (EOSAssertOneShot((x), (y), __LINE__, __FILE__)) \
		{ \
			ignoreAlways = true; \
		} \
	} \
}

#else

//	Double check any new compiler truly optimizes these bits out
#define Assert(x)
#define AssertWDesc(x, y)
#define AssertOneShot(x)
#define AssertWDescOneShot(x, y)

#endif /* __DEBUG */

#endif /* __DEBUG_H__ */
