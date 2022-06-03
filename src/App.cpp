/******************************************************************************
 *
 * File: App.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * App details. App defines the base class for the App. This also helps to
 * embed app specific details such as versioning, app name, etc.  
 * 
 *****************************************************************************/

#include "App.h"

App*			_appRefPtr = NULL;

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

App::App(NSString* name, Uint32 major, Uint32 minor, Uint32 subminor, Uint32 build, const Char* date, const Char* time)

#else

App::App(const UTF16* name, Uint32 major, Uint32 minor, Uint32 subminor, Uint32 build, const Char* date, const Char* time)

#endif /* _PLATFORM_MAC */

{
	Uint32	i;

	_app = this;

	AssertWDesc(name != NULL, "App::App NULL name");
	AssertWDesc(date != NULL, "App::App NULL date");
	AssertWDesc(time != NULL, "App::App NULL time");

	_buildVersionMajor = major;
	_buildVersionMinor = minor;
	_buildVersionSubMinor = subminor;

	_buildNumber = build;

#ifdef _PLATFORM_PC

	wcscpy_s(_buildName, BuildNameLength, name);
	swprintf_s(_buildDate, BuildDateLength, L"%s", date);
	swprintf_s(_buildTime, BuildTimeLength, L"%s", time);

	//	Derive date of launch
	SYSTEMTIME	systime;

	GetLocalTime(&systime);

	_launchTime.sec = (Uint8) systime.wSecond;
	_launchTime.min = (Uint8) systime.wMinute;
	_launchTime.hour = (Uint8) systime.wHour;
	_launchTime.pad = 0;

	_launchDate.year = (Uint16) systime.wYear;
	_launchDate.month = (Uint8) systime.wMonth;
	_launchDate.day = (Uint8) systime.wDay;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	//	Since this is in the constructor, an NSAutorelease pool may not exist, so create one now.
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	
	_buildName = [[NSString alloc] initWithString:name];
	
	_buildDate = [[NSString alloc] initWithCString:date encoding:NSASCIIStringEncoding];
	
	_buildTime = [[NSString alloc] initWithCString:time encoding:NSASCIIStringEncoding];
	
	//	Derive date of launch
	NSDate*	currdate = [[NSDate alloc] init];
	NSDateFormatter* dateFormat = [[NSDateFormatter alloc]  init];
	[dateFormat setDateFormat:@"%S"];
	
	_launchTime.sec = [[dateFormat stringFromDate:currdate] intValue];
	
	[dateFormat setDateFormat:@"%M"];
	_launchTime.min = [[dateFormat stringFromDate:currdate] intValue];
		
	[dateFormat setDateFormat:@"%H"];
	_launchTime.hour = [[dateFormat stringFromDate:currdate] intValue];
	
	_launchTime.pad = 0;

	[dateFormat setDateFormat:@"%Y"];
	_launchDate.year = [[dateFormat stringFromDate:currdate] intValue];
	
	[dateFormat setDateFormat:@"%m"];
	_launchDate.month = [[dateFormat stringFromDate:currdate] intValue];

	[dateFormat setDateFormat:@"%d"];
	_launchDate.day = [[dateFormat stringFromDate:currdate] intValue];
	
	[dateFormat release];
	[currdate release];
	
	[pool drain];
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

#if defined(_PLATFORM_MAC)
	_controller = nil;
	_view = nil;
#endif /* _PLATFORM_MAC */

#ifdef _PLATFORM_IPHONE
	_isOS30OrHigher = false;
	_isOS32OrHigher = false;
	_isOS40OrHigher = false;
	_isHiRes = false;
	_isIPad = false;

	NSString *os30 = @"3.0";
	NSString *currSysVer = [[UIDevice currentDevice] systemVersion];

	if ([currSysVer compare:os30 options:NSNumericSearch] != NSOrderedAscending)
	{
		_isOS30OrHigher = true;
	}
	
	NSString *os32 = @"3.2";
	
	if ([currSysVer compare:os32 options:NSNumericSearch] != NSOrderedAscending)
	{
		_isOS32OrHigher = true;
	}
	
	NSString *os40 = @"4.0";
	
	if ([currSysVer compare:os40 options:NSNumericSearch] != NSOrderedAscending)
	{
		_isOS40OrHigher = true;
	}
	
	NSString*	model = [[UIDevice currentDevice] model];
	
	if ([model compare:@"iPad"] == NSOrderedSame || [model compare:@"iPad Simulator"] == NSOrderedSame)
	{
		_isIPad = true;
	}
#endif

	_assertHit = false;

	_assertDesc = NULL;

	for (i=0;i<MaxAssertLineNumStrLength;i++)
		_assertLineNumber[i] = 0;

	_assertFilename = NULL;

	_gfx.bindRenderer(&_renderer);
	_renderer.bindGfx(&_gfx);
}

App::~App()
{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	[_buildName release];
	[_buildDate release];
	[_buildTime release];
	
#endif /* _PLATFORM_MAC */

	if (_assertDesc)
		delete _assertDesc;

	if (_assertFilename)
		delete _assertFilename;
}

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

void App::initializeEOSFramework(EOSController* controller)

#else

void App::initializeEOSFramework(void)

#endif
{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	_controller = controller;
	
#else
	
	initVideo();

#endif /* _PLATFORM_MAC */

	_renderer.init();
	_audioMgr.init();
	_eventMgr.initialize();
}

#ifdef _PLATFORM_IPHONE
#ifdef _SUPPORT_ACCELEROMETER
void App::turnOnAccelerometer(Float32 rate)
{
	[_controller turnOnAccelerometer:(Float32) rate];
}

void App::turnOffAccelerometer(void)
{
	[_controller turnOffAccelerometer];
}
#endif /* _SUPPORT_ACCELEROMETER */

void App::notifyOnOrientationChangeOn(void)
{
	[_controller notifyOnOrientationChangeOn];
}

void App::notifyOnOrientationChangeOff(void)
{
	[_controller notifyOnOrientationChangeOff];
}

#endif

void App::exit(void)
{
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
#ifdef _DEBUG
	NSLog(@"exit");
#endif
	notifyEOSControllerOfExit();
#endif /* _PLATFORM_MAC */
}

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

void App::notifyEOSControllerOfExit(void)
{
#ifdef _DEBUG
	NSLog(@"notifyEOSControllerOfExit");
#endif

#ifdef _NOT_YET
	if (_controller != nil)
		[_controller notifyExit];
#endif
}

#endif /* _PLATFORM_MAC */

void App::assertHit(const Char* desc, Uint32 line, const Char* filename)
{
	if (_assertHit == false)
	{
		if (_assertDesc)
			delete _assertDesc;
	
		if (_assertFilename)
			delete _assertFilename;

		if (desc)
		{
			_assertDesc = new Char[strlen(desc) + 1];
		
			if (_assertDesc)
				strcpy(_assertDesc, desc);
		}
		else
			_assertDesc = NULL;

		if (filename)
		{
			_assertFilename = new Char[strlen(filename) + 1];
		
			if (_assertFilename)
				strcpy(_assertFilename, filename);
		}
		else
			_assertFilename = NULL;

		sprintf(_assertLineNumber, "Line %d", line);

		_assertHit = true;
	}
}

Boolean App::updateAssertHitCheck(void)
{
	if (_assertHit)
	{
		return true;
	}
	else
		return false;
}

Boolean App::renderAssertHitCheck(void)
{
	if (_assertHit)
	{
		Uint32	x = 16;
		Uint32	y = 16;
		Uint32	lines;

		getGfx()->beginFrame();

		getGfx()->clear(Gfx::ClearMaskColorBuffer);

		if (_assertDesc)
		{
			lines = _debugFont.drawString(x, y, _assertDesc, true);
			y += lines * 12;
		}

		if (_assertFilename)
		{
			lines = _debugFont.drawString(x, y, _assertFilename, true);
			y += lines * 12;
		}

		lines = _debugFont.drawString(x, y, _assertLineNumber, true);
		y += lines * 12;

		getGfx()->getBoundRenderer()->renderMode2D();

		getGfx()->endFrame();

		getGfx()->swapBuffer();

		return true;
	}
	else
		return false;
}

void App::initDebugFont(void)
{
	_debugFont.init();
}

void App::drawDebugString(Sint32 x, Sint32 y, const Char* str, Boolean linewrap)
{
	_debugFont.drawString(x, y, str, linewrap);
}

#ifdef _PLATFORM_IPHONE


void App::setOpenGLView(OpenGLView* view)
{
	_view = view;
	
	if (isOS40OrHigher())
	{
		if (_isHiRes)
			_view.contentScaleFactor = 1.0F;
		else
			_view.contentScaleFactor = 1.0F;
	}
}

Boolean App::isHighResolution(void)
{
	return _isHiRes;
}

void App::useHighResolution(Boolean hires)
{
	_isHiRes = hires;
	
	if (_view)
	{
		if (isOS40OrHigher())
		{
#if TARGET_IPHONE_SIMULATOR == 0

			if (hires)
				_view.contentScaleFactor = 1.0F;
			else
				_view.contentScaleFactor = 1.0F;
			
#endif
		}
	}
}

#endif /* _PLATFORM_IPHONE */
