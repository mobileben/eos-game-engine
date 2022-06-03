/******************************************************************************
 *
 * File: EOSEvent.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Event values.
 * 
 *****************************************************************************/

#ifndef __EOSEVENT_H__
#define __EOSEVENT_H__

typedef enum
{
	EOSEventTypeNone = 0,
	EOSEventTypeOS,
	EOSEventTypeKeyboard,
	EOSEventTypeMouse,
	EOSEventTypeTouchScreen,
#ifdef _SUPPORT_ACCELEROMETER
	EOSEventTypeAccelerometer,
#endif /* _SUPPORT_ACCELEROMETER */
} EOSEventType;

typedef enum
{
	EOSOSEventTypeNone = 0,
	EOSOSEventTypeBoot,
	EOSOSEventTypeLaunched,
	EOSOSEventTypeSuspend,
	EOSOSEventTypeResume,
	EOSOSEventTypeLowMemory,
} EOSOSEventType;

typedef enum
{
	EOSKeyboardEventTypeNone = 0,
	EOSKeyboardEventTypeKeyDown,
	EOSKeyboardEventTypeKeyUp,
	EOSKeyboardEventTypeKeyRepeat,
} EOSKeyboardEventType;

typedef enum
{
	EOSKeyboardKeyNone = 0,
	EOSKeyboardKeyESC,
	EOSKeyboardKeyBack,
	EOSKeyboardKeyReturn,
	EOSKeyboardKeySpace,
	EOSKeyboardKeyTab,
	EOSKeyboardKeyHome,
	EOSKeyboardKeyEnd,
	EOSKeyboardKeyPageUp,
	EOSKeyboardKeyPageDown,
	EOSKeyboardKeyInsert,
	EOSKeyboardKeyDelete,
	EOSKeyboardKeyLeft,
	EOSKeyboardKeyRight,
	EOSKeyboardKeyUp,
	EOSKeyboardKeyDown,
	EOSKeyboardKeyF1,
	EOSKeyboardKeyF2,
	EOSKeyboardKeyF3,
	EOSKeyboardKeyF4,
	EOSKeyboardKeyF5,
	EOSKeyboardKeyF6,
	EOSKeyboardKeyF7,
	EOSKeyboardKeyF8,
	EOSKeyboardKeyF9,
	EOSKeyboardKeyF10,
	EOSKeyboardKeyF11,
	EOSKeyboardKeyF12,
	EOSKeyboardKey0,
	EOSKeyboardKey1,
	EOSKeyboardKey2,
	EOSKeyboardKey3,
	EOSKeyboardKey4,
	EOSKeyboardKey5,
	EOSKeyboardKey6,
	EOSKeyboardKey7,
	EOSKeyboardKey8,
	EOSKeyboardKey9,
	EOSKeyboardKeyA,
	EOSKeyboardKeyB,
	EOSKeyboardKeyC,
	EOSKeyboardKeyD,
	EOSKeyboardKeyE,
	EOSKeyboardKeyF,
	EOSKeyboardKeyG,
	EOSKeyboardKeyH,
	EOSKeyboardKeyI,
	EOSKeyboardKeyJ,
	EOSKeyboardKeyK,
	EOSKeyboardKeyL,
	EOSKeyboardKeyM,
	EOSKeyboardKeyN,
	EOSKeyboardKeyO,
	EOSKeyboardKeyP,
	EOSKeyboardKeyQ,
	EOSKeyboardKeyR,
	EOSKeyboardKeyS,
	EOSKeyboardKeyT,
	EOSKeyboardKeyU,
	EOSKeyboardKeyV,
	EOSKeyboardKeyW,
	EOSKeyboardKeyX,
	EOSKeyboardKeyY,
	EOSKeyboardKeyZ,
} EOSKeyboardKey;

#define EOS_KEYBOARD_MODIFER_SHIFT		1
#define EOS_KEYBOARD_MODIFER_CONTROL	2
#define EOS_KEYBOARD_MODIFER_ALT		4

typedef enum
{
	EOSMouseEventTypeNone = 0,
	EOSMouseEventTypeLeftButtonDown,
	EOSMouseEventTypeLeftButtonUp,
	EOSMouseEventTypeRightButtonDown,
	EOSMouseEventTypeRightButtonUp,
	EOSMouseEventTypeMiddleButtonDown,
	EOSMouseEventTypeMiddleButtonUp,
	EOSMouseEventTypeMove,
	EOSMouseEventTypeDrag,
} EOSMouseEventType;

#define EOS_MOUSE_MODIFER_SHIFT			1
#define EOS_MOUSE_MODIFER_CONTROL		2
#define EOS_MOUSE_MODIFER_LEFT_BUTTON	4
#define EOS_MOUSE_MODIFER_MIDDLE_BUTTON	8
#define EOS_MOUSE_MODIFER_RIGHT_BUTTON	16

typedef enum
{
	EOSTouchScreenEventTypeNone = 0,
	EOSTouchScreenEventTypeStart,
	EOSTouchScreenEventTypeEnd,
	EOSTouchScreenEventTypeMove,
	EOSTouchScreenEventTypeCancel,
} EOSTouchScreenEventType;

#ifdef _SUPPORT_ACCELEROMETER
typedef enum
{
	EOSAccelerometerEventTypeNone = 0,
	EOSAccelerometerEventTypeUpdate,
} EOSAccelerometerEventType;
#endif /* _SUPPORT_ACCELEROMETER */

typedef struct
{
	EOSEventType	type;
	MicroSeconds	timestamp;

	union
	{
		struct
		{
			EOSOSEventType	type;
		} os;

		struct
		{
			EOSKeyboardEventType	type;
			EOSKeyboardKey			key;
			Uint32					modifers;
		} keyboard;

		struct
		{
			EOSMouseEventType 		type;
			Uint16					x;		
			Uint16					y;
			Uint32					modifers;
		} mouse;

		struct
		{
			EOSTouchScreenEventType type;
#ifdef _PLATFORM_IPHONE
			UITouch*				touchObj;
#endif /* _PLATFORM_IPHONE */
			Uint16					x;		
			Uint16					y;		
			Uint32					tapCount;
		} touchscreen;
		
#ifdef _SUPPORT_ACCELEROMETER
		struct
		{
			EOSAccelerometerEventType	type;
			UIAccelerationValue			x;
			UIAccelerationValue			y;
			UIAccelerationValue			z;
		} accelerometer;
#endif /* _SUPPORT_ACCELEROMETER */

	} data;

} EOSEvent;

typedef Boolean (*EOSEventNotifier)(EOSEvent* event, void* object);

#endif /* __EOSEVENT_H__ */

