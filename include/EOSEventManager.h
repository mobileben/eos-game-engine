/******************************************************************************
 *
 * File: EOSEventManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Event Manager
 * 
 *****************************************************************************/

#ifndef __EOSEVENT_MANAGER_H__
#define __EOSEVENT_MANAGER_H__

#include "Platform.h"
#include "EOSEvent.h"
#include "EOSError.h"

#define EOS_EVENT_MANAGER_MAX_LISTENERS				8
#define EOS_EVENT_MANAGER_MAX_OS_BUFFER				32
#define EOS_EVENT_MANAGER_MAX_KEYBOARD_BUFFER		32
#define EOS_EVENT_MANAGER_MAX_MOUSE_BUFFER			32
#define EOS_EVENT_MANAGER_MAX_TOUCHSCREEN_BUFFER	32
#ifdef _SUPPORT_ACCELEROMETER
#define EOS_EVENT_MANAGER_MAX_ACCELEROMETER_BUFFER	32
#endif /* _SUPPORT_ACCELEROMETER */

typedef struct
{
	Boolean				used;
	EOSEventType		event;
	EOSEventNotifier	notifer;
	void*				object;
} EOSEventListener;

typedef struct
{
	EOSEventType	type;
	Uint32			inputIndex;
	Uint32			readIndex;
	Uint32 			max;
	MicroSeconds	lastInputTime;
	EOSEvent*		pool; 
} EOSEventBuffer;

#define EOS_EVENT_MOUSE_BUTTON_STATUS_LEFT		1
#define EOS_EVENT_MOUSE_BUTTON_STATUS_MIDDLE	2
#define EOS_EVENT_MOUSE_BUTTON_STATUS_RIGHT		4

class EOSEventManager : public EOSFrameworkComponent
{
private:
	Uint32				_maxListeners;
	EOSEventListener	_listeners[EOS_EVENT_MANAGER_MAX_LISTENERS];
	EOSEventListener*	_activeListeners[EOS_EVENT_MANAGER_MAX_LISTENERS];

	EOSEventBuffer	_osBuffer;

#if defined(_PLATFORM_PC) || defined(_PLATFORM_MAC)

	EOSEventBuffer	_keyboardBuffer;

	EOSEventBuffer	_mouseBuffer;
	Uint32			_mouseButtonStatus;	

#ifdef _PLATFORM_MAC
	Double64		_initialTimestamp;
#endif /* _PLATFORM_MAC */
	
#elif defined(_PLATFORM_IPHONE)

	EOSEventBuffer	_touchScreenBuffer;
	
#ifdef _SUPPORT_ACCELEROMETER
	EOSEventBuffer	_accelerometerBuffer;
#endif /* _SUPPORT_ACCELEROMETER */

	Double64		_initialTimestamp;

#ifdef _SUPPORT_ACCELEROMETER
	Double64		_initialAccelerometerTimestamp;
#endif /* _SUPPORT_ACCELEROMETER */

#endif /* _PLATFORM_PC || _PLATFORM_MAC */

protected:
	EOSError			createEventBuffer(EOSEventBuffer& buffer, EOSEventType type, Uint32 num);
	EOSError			destroyEventBuffer(EOSEventBuffer& buffer);
	EOSError			initializeEventBuffer(EOSEventBuffer& buffer);

	void 				addEventToBuffer(EOSEventBuffer& buffer, EOSEvent& event);

	EOSEventListener*	findFreeListener(void);

#ifdef _DEBUG
	void				validateActiveListenerList(void);
#endif /* _DEBUG */

public:
	EOSEventManager();
	~EOSEventManager();

	EOSError			initialize(void);

	EOSError			addListener(EOSEventType event, EOSEventNotifier notifer, void* object);
	EOSError			removeListener(EOSEventNotifier notifer);

	void				enableEOSEventType(EOSEventType type);
	void				disableEOSEventType(EOSEventType type);
	Boolean				isEOSEventTypeEnabled(void);
	
	void				notifyOfOrientationChange(Boolean landscape);
	
	//	These become embedded into the OS, so will look more OS specific
#ifdef _PLATFORM_PC

	EOSKeyboardKey		virtKeyToEOSKeyboardKey(Uint32 virtkey);

	static Char			EOSKeyboardKeyToChar(EOSKeyboardKey key);

	void				interceptKeyboardEventDown(Uint32 wParam, Uint32 lParam);
	void				interceptKeyboardEventUp(Uint32 wParam, Uint32 lParam);
	void				interceptKeyboardEventRepeat(Uint32 wParam, Uint32 lParam);

	void				interceptMouseEventLeftButtonDown(Uint32 wParam, Uint32 lParam);
	void				interceptMouseEventLeftButtonUp(Uint32 wParam, Uint32 lParam);
	void				interceptMouseEventRightButtonDown(Uint32 wParam, Uint32 lParam);
	void				interceptMouseEventRightButtonUp(Uint32 wParam, Uint32 lParam);
	void				interceptMouseEventMiddleButtonDown(Uint32 wParam, Uint32 lParam);
	void				interceptMouseEventMiddleButtonUp(Uint32 wParam, Uint32 lParam);
	void				interceptMouseEventMove(Uint32 wParam, Uint32 lParam);

#elif defined(_PLATFORM_MAC)
	
	EOSKeyboardKey		virtKeyToEOSKeyboardKey(Uint32 virtkey);

	static Char			EOSKeyboardKeyToChar(EOSKeyboardKey key);

	void				interceptKeyboardEventDown(NSEvent* event);
	void				interceptKeyboardEventUp(NSEvent* event);
	void				interceptMouseEventLeftButtonDown(NSEvent* event, NSView* view);
	void				interceptMouseEventLeftButtonUp(NSEvent* event, NSView* view);
	void				interceptMouseEventLeftButtonDrag(NSEvent* event, NSView* view);
	void				interceptMouseEventRightButtonDown(NSEvent* event, NSView* view);
	void				interceptMouseEventRightButtonUp(NSEvent* event, NSView* view);
	void				interceptMouseEventRightButtonDrag(NSEvent* event, NSView* view);
	
#elif defined(_PLATFORM_IPHONE)

	void				interceptTouchScreenEventDown(NSSet* touches, UIEvent* event, UIView* view);
	void				interceptTouchScreenEventUp(NSSet* touches, UIEvent* event, UIView* view);
	void				interceptTouchScreenEventMove(NSSet* touches, UIEvent* event, UIView* view);
	void				interceptTouchScreenEventCancel(NSSet* touches, UIEvent* event, UIView* view);
	
#ifdef _SUPPORT_ACCELEROMETER
	void				interceptAccelerometerEventUpdate(UIAcceleration* acceleration);
#endif /* _SUPPORT_ACCELEROMETER */
#else

	#error _PLATFORM not defined.
	 
#endif /* _PLATFORM_PC */
};

#endif /* __EOSEVENT_MANAGER_H__*/

