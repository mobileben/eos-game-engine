/******************************************************************************
 *
 * File: App.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * App details. App defines the base class for the App. This also helps to
 * embed app specific details such as versioning, app name, etc.  
 * 
 *****************************************************************************/

#ifndef __APP_H__
#define __APP_H__

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

#ifndef _EOS_CONSOLE_TOOL
#import "EOSController.h"
#import "OpenGLView.h"
#endif /* _EOS_CONSOLE_TOOL */

#endif /* _PLATFORM_MAC */

#include "Platform.h"
#include "EOSError.h"
#include "EOSEventManager.h"
#include "CoreSys.h"
#include "Heartbeat.h"
#include "Gfx.h"
#include "SpriteSetManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "TextureAtlasManager.h"
#include "SpriteAnimSetManager.h"
#include "Renderer.h"
#include "FontManager.h"
#include "DebugFont.h"
#include "VibrationManager.h"

#ifdef _SUPPORT_OPEN_FEINT

#include "EOSOFDelegate.h"
#include "EOSOFChallengeDelegate.h"
#include "EOSOFNotificationDelegate.h"

#endif /* _SUPPORT_OPEN_FEINT */

class App : public EOSFrameworkComponent
{
public:
	typedef enum
	{
		BuildNameLength = 64,
		BuildDateLength = 64,
		BuildTimeLength = 64,
	};

private:
	static const int		MaxAssertLineNumStrLength = 16;

	Platform				_platform;
	CoreSys		   			_coreSys;  
	Heartbeat				_heartbeat;
	TextureManager			_textureMgr;
	TextureAtlasManager		_texAtlasMgr;
	SpriteSetManager		_spriteSetMgr;
	SpriteAnimSetManager	_spriteAnimSetMgr;
	Gfx						_gfx;
	AudioManager			_audioMgr;
	Renderer				_renderer;
	EOSEventManager			_eventMgr;
	VibrationManager		_vibrateMgr;
	FontManager				_fontMgr;

	DebugFont				_debugFont;

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	NSString*		_buildName;
#ifndef _EOS_CONSOLE_TOOL
	EOSController*	_controller;
	OpenGLView*		_view;
#endif /* _EOS_CONSOLE_TOOL */
	
#else
	
	UTF16			_buildName[BuildNameLength];

#endif /* _PLATFORM_MAC */
	
	Uint32			_buildNumber;

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	NSString*		_buildDate;
	NSString*		_buildTime;
	
#else
	
	UTF16			_buildDate[BuildDateLength];
	UTF16			_buildTime[BuildTimeLength];

#endif /* _PLATFORM_MAC */
	
	Uint32			_buildVersionMajor;
	Uint32			_buildVersionMinor;
	Uint32			_buildVersionSubMinor;

	Date			_launchDate;
	Time			_launchTime;

#ifdef _PLATFORM_IPHONE
	Boolean			_isOS30OrHigher;
	Boolean			_isOS32OrHigher;
	Boolean			_isOS40OrHigher;
	Boolean			_isHiRes;
	Boolean			_isIPad;
#endif /* _PLATFORM_PHONE */

	Boolean			_assertHit;

	Char*			_assertDesc;
	Char			_assertLineNumber[MaxAssertLineNumStrLength];
	Char*			_assertFilename;

public:
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	App(NSString* name, Uint32 major, Uint32 minor, Uint32 subminor, Uint32 build, const Char* date, const Char* time);
#else	
	App(const UTF16* name, Uint32 major, Uint32 minor, Uint32 subminor, Uint32 build, const Char* date, const Char* time);
#endif /* _PLATFORM_MAC */
	
	~App();

	inline Platform*				getPlatform(void) { return &_platform; }
	inline CoreSys*					getCoreSys(void) { return &_coreSys; }
	inline Heartbeat*				getHeartbeat(void) { return &_heartbeat; }
	inline TextureManager*			getTextureManager(void) { return &_textureMgr; }
	inline TextureAtlasManager*		getTextureAtlasManager(void) { return &_texAtlasMgr; }
	inline SpriteSetManager*		getSpriteSetManager(void) { return &_spriteSetMgr; }
	inline SpriteAnimSetManager*	getSpriteAnimSetManager(void) { return &_spriteAnimSetMgr; }
	inline Gfx*						getGfx(void) { return &_gfx; }
	inline AudioManager*			getAudioManager(void) { return &_audioMgr; }
	inline Renderer*				getRenderer(void) { return &_renderer; }
	inline EOSEventManager*			getEOSEventManager(void) { return &_eventMgr; }
	inline FontManager*				getFontManager(void) { return &_fontMgr; }
	inline VibrationManager*		getVibrationManager(void) { return &_vibrateMgr; }
	
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	inline NSString*		getBuildName(void) { return _buildName; }
	
	inline NSString*		getBuildDate(void) { return _buildDate; }
	inline NSString*		getBuildTime(void) { return _buildTime; }

#else
	
	inline const UTF16*		getBuildName(void) { return _buildName; }

	inline const UTF16*		getBuildDate(void) { return _buildDate; }
	inline const UTF16*		getBuildTime(void) { return _buildTime; }

#endif /* _PLATFORM_MAC */
	
	inline Uint32			getBuildNumber(void) { return _buildNumber; }
	inline void				getBuildVersion(Uint32& major, Uint32& minor, Uint32& subminor) { major = _buildVersionMajor; minor = _buildVersionMinor; subminor = _buildVersionSubMinor; }

	inline void				getLaunchDate(Date& date) { date = _launchDate; }
	inline void				getLaunchTime(Time& time) { time = _launchTime; }

#if defined(_PLATFORM_MAC)
	
#ifndef _EOS_CONSOLE_TOOL
	void					initializeEOSFramework(EOSController* controller);
	
	virtual	void			initVideo(OpenGLView* view) =0;

#endif /* _EOS_CONSOLE_TOOL */

#ifndef _EOS_CONSOLE_TOOL
	inline OpenGLView*		getOpenGLView(void) { return _view; }
	inline void				setOpenGLView(OpenGLView* view) { _view = view; }
	inline EOSController*	getEOSController(void) { return _controller; }
	
	void					notifyEOSControllerOfExit(void);
#endif /* _EOS_CONSOLE_TOOL */
	
#elif defined(_PLATFORM_IPHONE)

	void					initializeEOSFramework(EOSController* controller);
	
	virtual	void			initVideo(OpenGLView* view) =0;
	
	inline OpenGLView*		getOpenGLView(void) { return _view; }
	void					setOpenGLView(OpenGLView* view);
	inline EOSController*	getEOSController(void) { return _controller; }
	
	Boolean					isHighResolution(void);
	void					useHighResolution(Boolean hires);
	
	void					notifyEOSControllerOfExit(void);
	
	virtual void			didReceiveMemoryWarning(void) {}
	virtual void			applicationWillResignActive(void) {}
	virtual void			applicationDidBecomeActive(void) {}
	virtual void			applicationWillTerminate(void) {}
	virtual void			applicationDidFinishLaunching(void) {}
	virtual BOOL			applicationShouldAutorotateToInterfaceOrientation(UIInterfaceOrientation orientation) { return NO; }
	virtual void			applicationWillRotateToInterfaceOrientation(UIInterfaceOrientation orientation, NSTimeInterval duration) {}
	virtual void			applicationDidRotateFromInterfaceOrientation(UIInterfaceOrientation orientation) {}

	virtual BOOL			applicationHandleOpenURL(NSURL* url) { return NO; }
	
#else
	
	void					initializeEOSFramework(void);

	virtual	void			initVideo(void) =0;

#endif /* _PLATFORM_MAC */
	
#ifdef _SUPPORT_OPEN_FEINT
	
	virtual void dashboardWillAppear(void) {}
	virtual void dashboardDidAppear(void) {}
	virtual void dashboardWillDisappear(void) {}
	virtual void dashboardDidDisappear(void) {}
	virtual void userLoggedIn(NSString* userID) {}
	virtual Boolean showCustomOpenFeintApprovalScreen(void) { return false; }
	virtual Boolean isOpenFeintNotificationAllowed(OFNotificationData* notificationData) { return false; }
	virtual void handleDisallowedNotification(OFNotificationData* notificationData) {}
	virtual void notificationWillShow(OFNotificationData* notificationData) {}
	virtual void userLaunchedChallenge(OFChallengeToUser* challengeToLaunch, NSData* challengeData) {}
	virtual void userRestartedChallenge(void) {}
	
	virtual void localUserHighScoreSuccess(NSString* leaderboardID, Sint32 score, Sint32 rank) {}
	virtual void localUserHighScoreFailure(NSString* leaderboardID) {}
	
	virtual void achievementUnlocksSuccess(void) {}
	virtual void achievementUnlocksFailure(void) {}

#endif /* _SUPPORT_OPEN_FEINT */

	virtual	void			init(void) =0;
	virtual	void			exit(void);
	virtual	void			notify(EOSEvent event) =0;
	virtual	void			update(MicroSeconds delta_t) =0;
	virtual	void			render(void) =0;

	void					assertHit(const Char* desc, Uint32 line, const Char* filename);
	Boolean					updateAssertHitCheck(void);
	Boolean					renderAssertHitCheck(void);

	//	Special rendering type routines
	void					initDebugFont(void);
	void 					drawDebugString(Sint32 x, Sint32 y, const Char* str, Boolean linewrap = false);

#ifdef _PLATFORM_IPHONE

	inline Boolean			isOS30OrHigher(void) { return _isOS30OrHigher; }
	inline Boolean			isOS32OrHigher(void) { return _isOS32OrHigher; }
	inline Boolean			isOS40OrHigher(void) { return _isOS40OrHigher; }
	inline Boolean			isIPad(void) { return _isIPad; }

#ifdef _SUPPORT_ACCELEROMETER
	void					turnOnAccelerometer(Float32 rate);
	void					turnOffAccelerometer(void);
#endif /* _SUPPORT_ACCELEROMETER */
	
	void					notifyOnOrientationChangeOn(void);
	void					notifyOnOrientationChangeOff(void);
#endif

	//	"Factory" type routines used for easier access to get object types
	inline Texture*			findTextureFromRefID(ObjectID refID) { return _textureMgr.findTextureFromRefID(refID); }
	inline Texture*			findTextureFromName(const Char* name) { return _textureMgr.findTextureFromName(name); }

	inline TextureAtlas*	findTextureAtlasFromRefID(ObjectID refID) { return _texAtlasMgr.findTextureAtlasFromRefID(refID); }
	inline TextureAtlas*	findTextureAtlasFromName(const Char* name) { return _texAtlasMgr.findTextureAtlasFromName(name); }

	inline SpriteSet*		findSpriteSetFromRefID(ObjectID refID) { return _spriteSetMgr.findSpriteSetFromRefID(refID); }
	inline SpriteSet*		findSpriteSetFromName(const Char* name) { return _spriteSetMgr.findSpriteSetFromName(name); }

	inline SpriteAnimSet*	findSpriteAnimSetFromRefID(ObjectID refID) { return _spriteAnimSetMgr.findSpriteAnimSetFromRefID(refID); }
	inline SpriteAnimSet*	findSpriteAnimSetFromName(const Char* name) { return _spriteAnimSetMgr.findSpriteAnimSetFromName(name); }
};

//	Must be defined in the app, just to be safe for easier passing of app reference through the different platforms
extern App*	_appRefPtr;

#endif /* __APP_H__ */

