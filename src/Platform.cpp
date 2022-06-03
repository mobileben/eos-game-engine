/******************************************************************************
 *
 * File: Platform.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Platform/device specific details. This encapsulates the platform itself
 * and is used in conjunction with the App class to define a running instance
 * of an Application on a target Platform. HW access is done through the 
 * CoreSys class rather than than the App class or through platform specific
 * APIs. 
 * 
 *****************************************************************************/

#include "Platform.h"

App* EOSFrameworkComponent::_app = NULL;

Platform::Platform()
{
#ifdef _PLATFORM_PC
	OSVERSIONINFO	info;

	ZeroMemory(&info, sizeof(OSVERSIONINFO));

	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&info);

	_osVersionMajor = info.dwMajorVersion;
	_osVersionMinor = info.dwMinorVersion;
	_osVersionSubMinor = info.dwBuildNumber;

	//	Note tha swprintf_s takes the length of the string avail. So we need to divide off by sizeof(UTF16) 
	swprintf_s((wchar_t*) _osVersionString, sizeof(_osVersionString) / sizeof(UTF16), L"%d.%d.%d", _osVersionMajor, _osVersionMinor, _osVersionSubMinor);

	_type = PlatformTypePC;

	_hInstance = NULL;
	_hPrevInstance = NULL;

#elif defined(_PLATFORM_MAC)
	SInt32	version;
	
	if (Gestalt(gestaltSystemVersion, &version) == noErr)
	{
		if (version < 0x1040)
		{
			_osVersionMajor = ((version & 0xF000) >> 12) * 10 + ((version & 0x0F00) >> 8);
			_osVersionMinor = (version & 0x00F0) >> 4;
			_osVersionSubMinor = (version & 0x000F);
		}
		else
		{
			if (Gestalt(gestaltSystemVersionMajor, &version) == noErr)
				_osVersionMajor = version;
			else
				_osVersionMajor = 0;
			
			if (Gestalt(gestaltSystemVersionMinor, &version) == noErr)
				_osVersionMinor = version;
			else
				_osVersionMinor = 0;
			
			if (Gestalt(gestaltSystemVersionBugFix, &version) == noErr)
				_osVersionSubMinor = version;
			else
				_osVersionSubMinor = 0;
		}
	}
	else
	{
		_osVersionMajor = _osVersionMinor = _osVersionSubMinor = 0;
	}
	
	_osVersionString = [[NSString alloc] initWithFormat:@"%d.%d.%d", _osVersionMajor, _osVersionMinor, _osVersionSubMinor];
	
	_type = PlatformTypeMac;
	
#elif defined(_PLATFORM_BREW)

#elif defined(_PLATFORM_IPHONE)

	NSArray*			array;
	NSString*			tmp;
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	_osVersionString = [[NSString alloc] initWithString: [[UIDevice currentDevice] systemVersion]];
	
	array = [_osVersionString componentsSeparatedByString:@"."];
	
	if ([array count] > 0)
	{
		tmp = [array objectAtIndex:0];
		
		if (tmp != nil)
		{
			_osVersionMajor = [tmp intValue];
		}
	}
	
	if ([array count] > 1)
	{
		tmp = [array objectAtIndex:1];
		
		if (tmp != nil)
		{
			_osVersionMinor = [tmp intValue];
		}
	}
	
	if ([array count] > 2)
	{
		tmp = [array objectAtIndex:2];
		
		if (tmp != nil)
		{
			_osVersionSubMinor = [tmp intValue];
		}
	}

	if ([[[UIDevice currentDevice] model] compare:@"iPod touch"] == NSOrderedSame)
		_type = PlatformTypeIPodTouch;
	else
		_type = PlatformTypeIPhone;
		
	[pool release];

#elif defined(_PLATFORM_WII)

#endif	/* _PLATFORM_PC */
}

Platform::~Platform()
{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	[_osVersionString release];
#endif /* _PLATFORM_MAC */
}

PlatformType Platform::getPlatformType(void)
{
	return _type;
}

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

NSString* Platform::getOSVersionString(void)

#else

const UTF16* Platform::getOSVersionString(void)

#endif /* _PLATFORM_MAC */

{
	return _osVersionString;
}

void Platform::getOSVersion(Uint32& major, Uint32& minor, Uint32 &subminor)
{
	major = _osVersionMajor;
	minor = _osVersionMinor;
	subminor = _osVersionSubMinor;
}

Boolean Platform::isBigEndian(void)
{
	if (_endian.getNativeEndian() == Endian::EndianTypeBig)
		return true;
	else
		return false;
}

Boolean Platform::isLittleEndian(void)
{
	if (_endian.getNativeEndian() == Endian::EndianTypeLittle)
		return true;
	else
		return false;
}

const Endian* Platform::getEndian(void)
{
	return &_endian;
}

