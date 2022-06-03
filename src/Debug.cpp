/******************************************************************************
 *
 * File: Debug.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Debug
 * 
 *****************************************************************************/

#include "Debug.h"
#include "App.h"

#ifdef _DEBUG

Boolean EOSAssert(Boolean test, const Char* desc, Uint32 line, const Char* filename)
{
	Boolean asserted = false;

	if (!test)
	{
#if defined(_EOS_TOOL) || defined(_EOS_CONSOLE_TOOL)
		assert(test);
#else
		if (_appRefPtr)
			_appRefPtr->assertHit(desc, line, filename);
#endif /* _EOS_TOOL || _EOS_CONSOLE_TOOL */

		asserted = true;
	}
	else
		asserted = false;

	return asserted;
}

Boolean EOSAssertOneShot(Boolean test, const Char* desc, Uint32 line, const Char* filename)
{
	Boolean asserted = false;

	if (!test)
	{
#if defined(_EOS_TOOL) || defined(_EOS_CONSOLE_TOOL)
		assert(test);
#endif /* _EOS_TOOL || _EOS_CONSOLE_TOOL */

		asserted = true;
	}
	else
		asserted = false;
	
	return asserted;
}

#endif /* _DEBUG */
