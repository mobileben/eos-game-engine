/******************************************************************************
 *
 * File: Platform.h
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

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

//	PlatformType lists the supported platform types
typedef enum
{
	PlatformTypeUndefined = 0,
	PlatformTypePC,
	PlatformTypeMac,
	PlatformTypeFlash,
	PlatformTypeJ2ME,
	PlatformTypeJava,
	PlatformTypeBREW,
	PlatformTypeIPhone,
	PlatformTypeIPodTouch,
	PlatformTypeWii,
	PlatformTypeLast,
} PlatformType;

#ifdef _PLATFORM_PC

#ifdef _DEBUG_MEMORY

#ifdef _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#endif /* __CRTDBG_MAP_ALLOC */

#endif /* _DEBUG_MEMORY */

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define _32BIT_ADDRESSING

#elif defined(_PLATFORM_MAC)

#include <Cocoa/Cocoa.h>
#include <wchar.h>

#define _32BIT_ADDRESSING

#elif defined(_PLATFORM_BREW)

#define _16BIT_ADDRESSING

#elif defined(_PLATFORM_IPHONE)

#include <wchar.h>

#define _32BIT_ADDRESSING

#elif defined(_PLATFORM_WII)

#define _32BIT_ADDRESSING

#else

#error _PLATFORM not defined.

#endif	/* _PLATFORM_PC */


#include "DTypes.h"
#include "Endian.h"
#include "Debug.h"

class EOSObject
{
private:
public:
	EOSObject() {}
	virtual ~EOSObject() {}
};

class	App;

class EOSRenderObject : EOSObject
{
public:
	EOSRenderObject();
	~EOSRenderObject();
};

#define EOS_ASSET_NAME_MAX_LENGTH	16

class EOSAssetObject : EOSObject
{
public:
	EOSAssetObject();
	~EOSAssetObject();
};

class EOSFrameworkComponent : EOSObject
{
protected:
	static App*		_app;

public:
	EOSFrameworkComponent() {}
	~EOSFrameworkComponent() {}
};

//	Platform contains basic information about the platform
//	Note that we opt to not create derived classes for specificc platform types
// 	This is to provide a degree of transparency to our platform
class Platform : public EOSFrameworkComponent
{
public:
	typedef enum
	{
		OSVersionStringLength = 32, 
	};

private:
	PlatformType	_type;
	Endian			_endian;

	//	ID platform OS Version
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	NSString*		_osVersionString;
	
#else
	
	UTF16			_osVersionString[OSVersionStringLength];
	
#endif /* _PLATFORM_MAC */
	
	Uint32			_osVersionMajor;
	Uint32			_osVersionMinor;
	Uint32			_osVersionSubMinor;

#ifdef _PLATFORM_PC

	HINSTANCE		_hInstance;
	HINSTANCE		_hPrevInstance;

#elif defined(_PLATFORM_MAC)
	
#elif defined(_PLATFORM_IPHONE)
	
#else	

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

public:
	Platform();
	~Platform();

	PlatformType		getPlatformType(void);

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	NSString*			getOSVersionString(void);
	
#else
	
	const UTF16*		getOSVersionString(void);
	
#endif /* _PLATFORM_MAC */
	
	void				getOSVersion(Uint32& major, Uint32& minor, Uint32 &subminor);

	Boolean				isBigEndian(void);
	Boolean				isLittleEndian(void);

	const Endian*		getEndian(void);

#ifdef _PLATFORM_PC

	inline HINSTANCE	getHInstance(void) const { return _hInstance; }
	inline HINSTANCE	getHPrevInstance(void) const { return _hPrevInstance; }
	void				setHInstance(HINSTANCE hinst) { _hInstance = hinst; }
	void				setHPrevInstance(HINSTANCE hinst) { _hPrevInstance = hinst; }

#elif defined(_PLATFORM_MAC)
	
#elif defined(_PLATFORM_IPHONE)
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

};

#include "MemoryStats.h"

#endif /* __PLATFORM_H__ */

