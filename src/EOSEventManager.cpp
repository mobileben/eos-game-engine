/******************************************************************************
 *
 * File: EOSEventManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Event Manager
 * 
 *****************************************************************************/

#include "Platform.h"
#include "App.h"
#include "EOSEventManager.h"

EOSEventManager::EOSEventManager()
{
	_maxListeners = EOS_EVENT_MANAGER_MAX_LISTENERS;

	memset(_listeners, 0, sizeof(_listeners));
	memset(_activeListeners, 0, sizeof(_activeListeners));

	memset(&_osBuffer, 0, sizeof(_osBuffer));
	createEventBuffer(_osBuffer, EOSEventTypeOS, EOS_EVENT_MANAGER_MAX_OS_BUFFER);

#if defined(_PLATFORM_PC) || defined(_PLATFORM_MAC)

	memset(&_keyboardBuffer, 0, sizeof(_keyboardBuffer));
	createEventBuffer(_keyboardBuffer, EOSEventTypeKeyboard, EOS_EVENT_MANAGER_MAX_KEYBOARD_BUFFER);

	memset(&_mouseBuffer, 0, sizeof(_mouseBuffer));
	createEventBuffer(_mouseBuffer, EOSEventTypeMouse, EOS_EVENT_MANAGER_MAX_MOUSE_BUFFER);

	_mouseButtonStatus = 0;

#ifdef _PLATFORM_MAC
	
	_initialTimestamp = 0.0;
	
#endif /* _PLATFORM_MAC */
	
#elif defined(_PLATFORM_IPHONE)

	memset(&_touchScreenBuffer, 0, sizeof(_touchScreenBuffer));
	createEventBuffer(_touchScreenBuffer, EOSEventTypeTouchScreen, EOS_EVENT_MANAGER_MAX_TOUCHSCREEN_BUFFER);

#ifdef _SUPPORT_ACCELEROMETER
	memset(&_accelerometerBuffer, 0, sizeof(_accelerometerBuffer));
	createEventBuffer(_accelerometerBuffer, EOSEventTypeAccelerometer, EOS_EVENT_MANAGER_MAX_ACCELEROMETER_BUFFER);

#endif /* _SUPPORT_ACCELEROMETER */
	
	_initialTimestamp = 0.0;
	
#ifdef _SUPPORT_ACCELEROMETER
	_initialAccelerometerTimestamp = 0.0;
#endif /* _SUPPORT_ACCELEROMETER */	
	
#endif /* _PLATFORM_PC || _PLATFORM_MAC */

}

EOSEventManager::~EOSEventManager()
{
	destroyEventBuffer(_osBuffer);

#if defined(_PLATFORM_PC) || defined(_PLATFORM_MAC)

	destroyEventBuffer(_keyboardBuffer);

	destroyEventBuffer(_mouseBuffer);
	_mouseButtonStatus = 0;

#elif defined(_PLATFORM_IPHONE)

	destroyEventBuffer(_touchScreenBuffer);
	
#ifdef _SUPPORT_ACCELEROMETER
	destroyEventBuffer(_accelerometerBuffer);
#endif /* _SUPPORT_ACCELEROMETER */

#endif /* _PLATFORM_PC || _PLATFORM_MAC */
}

EOSError EOSEventManager::initializeEventBuffer(EOSEventBuffer& buffer)
{
	EOSError	error = EOSErrorNone;
	Uint32 		i;

	if (buffer.pool)
	{
		memset(buffer.pool, 0, sizeof(EOSEvent) * buffer.max);

		buffer.inputIndex = buffer.readIndex = 0;
		buffer.lastInputTime = _app->getHeartbeat()->getMicroSeconds();

		for (i=0;i<buffer.max;i++)
		{
			buffer.pool[i].type = buffer.type;
			buffer.pool[i].timestamp = buffer.lastInputTime;
		}
	}
	else
		error = EOSErrorNotInitialized;

	return error;
}

EOSError EOSEventManager::initialize(void)
{
	EOSError error = EOSErrorNone;

	error = initializeEventBuffer(_osBuffer);

#if defined(_PLATFORM_PC) || defined(_PLATFORM_MAC)

	if (error == EOSErrorNone)
		error = initializeEventBuffer(_keyboardBuffer);

	if (error == EOSErrorNone)
	{
		error = initializeEventBuffer(_mouseBuffer);
		_mouseButtonStatus = 0;
	}

#elif defined(_PLATFORM_IPHONE)

	if (error == EOSErrorNone)
		error = initializeEventBuffer(_touchScreenBuffer);
	
#ifdef _SUPPORT_ACCELEROMETER
	if (error == EOSErrorNone)
		error = initializeEventBuffer(_accelerometerBuffer);
#endif /* _SUPPORT_ACCELEROMETER */

#endif /* _PLATFORM_PC || _PLATFORM_MAC */

	return error;
}

EOSError EOSEventManager::createEventBuffer(EOSEventBuffer& buffer, EOSEventType type, Uint32 num)
{
	EOSError	error = EOSErrorNone;

	destroyEventBuffer(buffer);

	buffer.pool = new EOSEvent[num];

	if (buffer.pool)
	{
		buffer.type = type;
		buffer.inputIndex = buffer.readIndex = 0;
		buffer.max = num;
		buffer.lastInputTime = 0;
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

EOSError EOSEventManager::destroyEventBuffer(EOSEventBuffer& buffer)
{
	EOSError	error = EOSErrorNone;

	if (buffer.pool)
	{
		delete [] buffer.pool;
		buffer.pool = NULL;
	}

	buffer.type = EOSEventTypeNone;
	buffer.inputIndex = buffer.readIndex = 0;
	buffer.max = 0;
	buffer.lastInputTime = 0;

	return error;
}

void EOSEventManager::addEventToBuffer(EOSEventBuffer& buffer, EOSEvent& event)
{
	Uint32	i;

	if (buffer.pool)
	{
		buffer.pool[buffer.inputIndex] = event;
		buffer.inputIndex++;

		if (buffer.inputIndex >= buffer.max)
			buffer.inputIndex = 0;

		//	Now notify
		while (buffer.readIndex != buffer.inputIndex)
		{
			for (i=0;i<buffer.max;i++)
			{
				if (_activeListeners[i] != NULL)
				{
					if (_activeListeners[i]->event == buffer.type)
					{
						if (_activeListeners[i]->notifer(&buffer.pool[buffer.readIndex], _activeListeners[i]->object))
						{
							//	Listener accepted the input
							break;
						}
					}
				}
				else
					break;
			}

			buffer.readIndex++;

			if (buffer.readIndex >= buffer.max)
				buffer.readIndex = 0;
		}
	}
}

EOSEventListener* EOSEventManager::findFreeListener(void)
{
	Uint32				i;
	EOSEventListener*	listener = NULL;

	for (i=0;i<_maxListeners;i++)
	{
		if (_listeners[i].used == false)
		{
			listener = &_listeners[i];
			break;
		}
	}

	return listener;
}

#ifdef _DEBUG
void EOSEventManager::validateActiveListenerList(void)
{
	Uint32	i;
	Uint32	check = 0;

	for (i=0;i<_maxListeners;i++)
	{
		if (_activeListeners[i] != NULL)
		{
			AssertWDesc(i == 0 || (i > 0 && check != 0), "EOSEventManager::validateActiveListenerList() problem with activeListener list");

			if (i == 0)
				check++;
			else if (_activeListeners[i-1] == NULL)
				check++;
		}
	}

	AssertWDesc(check <= 1, "EOSEventManager::validateActiveListenerList() problem with activeListener list");
}
#endif /* _DEBUG */

EOSError EOSEventManager::addListener(EOSEventType event, EOSEventNotifier notifer, void* object)
{
	EOSError 			error = EOSErrorNone;
	EOSEventListener*	listener;
	Uint32 				i;

	listener = findFreeListener();

	if (listener)
	{
		listener->used = true;
		listener->event = event;
		listener->notifer = notifer;
		listener->object = object;

#ifdef _DEBUG
		validateActiveListenerList();
#endif /* _DEBUG */

		for (i=0;i<_maxListeners;i++)
		{
			if (_activeListeners[i] == NULL)
			{
				_activeListeners[i] = listener;
				break;
			}
		}
	}
	else
		error = EOSErrorResourceNotAvailable;

	return error;
}

EOSError EOSEventManager::removeListener(EOSEventNotifier notifer)
{
	EOSError 			error = EOSErrorNone;
	EOSEventListener*	listener = NULL;
	Uint32				i, j;

#ifdef _DEBUG
	validateActiveListenerList();
#endif /* _DEBUG */

	for (i=0;i<_maxListeners;i++)
	{
		if (_listeners[i].used && _listeners[i].notifer == notifer)
		{
			listener = &_listeners[i];

			listener->used = false;
			listener->event = EOSEventTypeNone;
			listener->notifer = NULL;
			listener->object = NULL;
			break;
		}
	}

	if (listener)
	{
		//	Now remove it from the active list and then compress the active list
		for (i=0;i<_maxListeners;i++)
		{
			if (_activeListeners[i] == listener)
			{
				_activeListeners[i] = NULL;
				break;
			}
		}

		//	Now compress
		for (i=0;i<_maxListeners;i++)
		{
			if (_activeListeners[i] == NULL)
			{
				for (j=i+1;j<_maxListeners;j++)
				{
					_activeListeners[i] = _activeListeners[j];
				}

				_activeListeners[_maxListeners-1] = NULL;
			}
		}
	}
	else
		error = EOSErrorResourceDoesNotExist;

#ifdef _DEBUG
	validateActiveListenerList();
#endif /* _DEBUG */

	return error;
}

#if defined(_PLATFORM_PC) || defined(_PLATFORM_MAC)
Char	EOSEventManager::EOSKeyboardKeyToChar(EOSKeyboardKey key)
{
	Char	ch;

	switch (key)
	{
		case EOSKeyboardKeySpace:
			ch = ' ';
			break;

		case EOSKeyboardKey0:
			ch = '0';
			break;

		case EOSKeyboardKey1:
			ch = '1';
			break;

		case EOSKeyboardKey2:
			ch = '2';
			break;

		case EOSKeyboardKey3:
			ch = '3';
			break;

		case EOSKeyboardKey4:
			ch = '4';
			break;

		case EOSKeyboardKey5:
			ch = '5';
			break;

		case EOSKeyboardKey6:
			ch = '6';
			break;

		case EOSKeyboardKey7:
			ch = '7';
			break;

		case EOSKeyboardKey8:
			ch = '8';
			break;

		case EOSKeyboardKey9:
			ch = '9';
			break;

		case EOSKeyboardKeyA:
			ch = 'A';
			break;

		case EOSKeyboardKeyB:
			ch = 'B';
			break;

		case EOSKeyboardKeyC:
			ch = 'C';
			break;

		case EOSKeyboardKeyD:
			ch = 'D';
			break;

		case EOSKeyboardKeyE:
			ch = 'E';
			break;

		case EOSKeyboardKeyF:
			ch = 'F';
			break;

		case EOSKeyboardKeyG:
			ch = 'G';
			break;

		case EOSKeyboardKeyH:
			ch = 'H';
			break;

		case EOSKeyboardKeyI:
			ch = 'I';
			break;

		case EOSKeyboardKeyJ:
			ch = 'J';
			break;

		case EOSKeyboardKeyK:
			ch = 'K';
			break;

		case EOSKeyboardKeyL:
			ch = 'L';
			break;

		case EOSKeyboardKeyM:
			ch = 'M';
			break;

		case EOSKeyboardKeyN:
			ch = 'N';
			break;

		case EOSKeyboardKeyO:
			ch = 'O';
			break;

		case EOSKeyboardKeyP:
			ch = 'P';
			break;

		case EOSKeyboardKeyQ:
			ch = 'Q';
			break;

		case EOSKeyboardKeyR:
			ch = 'R';
			break;

		case EOSKeyboardKeyS:
			ch = 'S';
			break;

		case EOSKeyboardKeyT:
			ch = 'T';
			break;

		case EOSKeyboardKeyU:
			ch = 'U';
			break;

		case EOSKeyboardKeyV:
			ch = 'V';
			break;

		case EOSKeyboardKeyW:
			ch = 'W';
			break;

		case EOSKeyboardKeyX:
			ch = 'X';
			break;

		case EOSKeyboardKeyY:
			ch = 'Y';
			break;

		case EOSKeyboardKeyZ:
			ch = 'Z';
			break;

		default:
			ch = -1;
			break;
	}

	return ch;
}
#endif /* _PLATFORM_PC || _PLATFORM_MAC */


#ifdef _PLATFORM_PC

EOSKeyboardKey EOSEventManager::virtKeyToEOSKeyboardKey(Uint32 virtkey)
{
	EOSKeyboardKey	key = EOSKeyboardKeyNone;

	if (virtkey >= '0' && virtkey <= '9')
		key = (EOSKeyboardKey) (EOSKeyboardKey0 + (virtkey - '0'));
	else if (virtkey >= 'A' && virtkey <= 'Z')
		key = (EOSKeyboardKey) (EOSKeyboardKeyA + (virtkey - 'A'));
	else
	{
		//	Brute force
		switch (virtkey)
		{
			case VK_BACK:
				key = EOSKeyboardKeyBack;
				break;

			case VK_ESCAPE:
				key = EOSKeyboardKeyESC;
				break;

			case VK_RETURN:
				key = EOSKeyboardKeyReturn;
				break;

			case VK_SPACE:
				key = EOSKeyboardKeySpace;
				break;

			case VK_TAB:
				key = EOSKeyboardKeyTab;
				break;

			case VK_HOME:
				key = EOSKeyboardKeyHome;
				break;

			case VK_END:
				key = EOSKeyboardKeyEnd;
				break;

			case VK_INSERT:
				key = EOSKeyboardKeyInsert;
				break;

			case VK_DELETE:
				key = EOSKeyboardKeyDelete;
				break;

			case VK_PRIOR:
				key = EOSKeyboardKeyPageUp;
				break;

			case VK_NEXT:
				key = EOSKeyboardKeyPageDown;
				break;

			case VK_LEFT:
				key = EOSKeyboardKeyLeft;
				break;

			case VK_RIGHT:
				key = EOSKeyboardKeyRight;
				break;

			case VK_UP:
				key = EOSKeyboardKeyUp;
				break;

			case VK_DOWN:
				key = EOSKeyboardKeyDown;
				break;

			case VK_F1:
				key = EOSKeyboardKeyF1;
				break;

			case VK_F2:
				key = EOSKeyboardKeyF2;
				break;

			case VK_F3:
				key = EOSKeyboardKeyF3;
				break;

			case VK_F4:
				key = EOSKeyboardKeyF4;
				break;

			case VK_F5:
				key = EOSKeyboardKeyF5;
				break;

			case VK_F6:
				key = EOSKeyboardKeyF6;
				break;

			case VK_F7:
				key = EOSKeyboardKeyF7;
				break;

			case VK_F8:
				key = EOSKeyboardKeyF8;
				break;

			case VK_F9:
				key = EOSKeyboardKeyF9;
				break;

			case VK_F10:
				key = EOSKeyboardKeyF10;
				break;

			case VK_F11:
				key = EOSKeyboardKeyF11;
				break;

			case VK_F12:
				key = EOSKeyboardKeyF12;
				break;
		}
	}

	return key;
}

void EOSEventManager::interceptKeyboardEventDown(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeKeyboard;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.keyboard.type = EOSKeyboardEventTypeKeyDown;
	event.data.keyboard.key = virtKeyToEOSKeyboardKey(wParam);
	event.data.keyboard.modifers = 0;

	addEventToBuffer(_keyboardBuffer, event);
}

void EOSEventManager::interceptKeyboardEventUp(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeKeyboard;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.keyboard.type = EOSKeyboardEventTypeKeyUp;
	event.data.keyboard.key = virtKeyToEOSKeyboardKey(wParam);
	event.data.keyboard.modifers = 0;

	addEventToBuffer(_keyboardBuffer, event);
}

void EOSEventManager::interceptKeyboardEventRepeat(Uint32 wParam, Uint32 lParam)
{
}

void EOSEventManager::interceptMouseEventLeftButtonDown(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeMouse;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.mouse.type = EOSMouseEventTypeLeftButtonDown;
	event.data.mouse.x = GET_X_LPARAM(lParam);
	event.data.mouse.y = GET_Y_LPARAM(lParam);
	event.data.mouse.modifers = 0;

	if (wParam & MK_CONTROL)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;

	if (wParam & MK_SHIFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	_mouseButtonStatus |= EOS_EVENT_MOUSE_BUTTON_STATUS_LEFT;

	addEventToBuffer(_mouseBuffer, event);
}

void EOSEventManager::interceptMouseEventLeftButtonUp(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeMouse;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.mouse.type = EOSMouseEventTypeLeftButtonUp;
	event.data.mouse.x = GET_X_LPARAM(lParam);
	event.data.mouse.y = GET_Y_LPARAM(lParam);
	event.data.mouse.modifers = 0;

	if (wParam & MK_CONTROL)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;

	if (wParam & MK_SHIFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	_mouseButtonStatus &= ~EOS_EVENT_MOUSE_BUTTON_STATUS_LEFT;

	addEventToBuffer(_mouseBuffer, event);
}

void EOSEventManager::interceptMouseEventRightButtonDown(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeMouse;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.mouse.type = EOSMouseEventTypeRightButtonDown;
	event.data.mouse.x = GET_X_LPARAM(lParam);
	event.data.mouse.y = GET_Y_LPARAM(lParam);
	event.data.mouse.modifers = 0;

	if (wParam & MK_CONTROL)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;

	if (wParam & MK_SHIFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	_mouseButtonStatus |= EOS_EVENT_MOUSE_BUTTON_STATUS_RIGHT;

	addEventToBuffer(_mouseBuffer, event);
}

void EOSEventManager::interceptMouseEventRightButtonUp(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeMouse;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.mouse.type = EOSMouseEventTypeRightButtonUp;
	event.data.mouse.x = GET_X_LPARAM(lParam);
	event.data.mouse.y = GET_Y_LPARAM(lParam);
	event.data.mouse.modifers = 0;

	if (wParam & MK_CONTROL)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;

	if (wParam & MK_SHIFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	_mouseButtonStatus &= ~EOS_EVENT_MOUSE_BUTTON_STATUS_RIGHT;

	addEventToBuffer(_mouseBuffer, event);
}

void EOSEventManager::interceptMouseEventMiddleButtonDown(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeMouse;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.mouse.type = EOSMouseEventTypeMiddleButtonDown;
	event.data.mouse.x = GET_X_LPARAM(lParam);
	event.data.mouse.y = GET_Y_LPARAM(lParam);
	event.data.mouse.modifers = 0;

	if (wParam & MK_CONTROL)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;

	if (wParam & MK_SHIFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	_mouseButtonStatus |= EOS_EVENT_MOUSE_BUTTON_STATUS_MIDDLE;

	addEventToBuffer(_mouseBuffer, event);
}

void EOSEventManager::interceptMouseEventMiddleButtonUp(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeMouse;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	event.data.mouse.type = EOSMouseEventTypeMiddleButtonUp;
	event.data.mouse.x = GET_X_LPARAM(lParam);
	event.data.mouse.y = GET_Y_LPARAM(lParam);
	event.data.mouse.modifers = 0;

	if (wParam & MK_CONTROL)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;

	if (wParam & MK_SHIFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	_mouseButtonStatus &= ~EOS_EVENT_MOUSE_BUTTON_STATUS_MIDDLE;

	addEventToBuffer(_mouseBuffer, event);
}

void EOSEventManager::interceptMouseEventMove(Uint32 wParam, Uint32 lParam)
{
	EOSEvent	event;

	event.type = EOSEventTypeMouse;
	event.timestamp = _app->getHeartbeat()->getMicroSeconds();

	if (_mouseButtonStatus)
		event.data.mouse.type = EOSMouseEventTypeDrag;
	else
		event.data.mouse.type = EOSMouseEventTypeMove;

	event.data.mouse.x = GET_X_LPARAM(lParam);
	event.data.mouse.y = GET_Y_LPARAM(lParam);
	event.data.mouse.modifers = 0;

	if (wParam & MK_CONTROL)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;

	if (wParam & MK_SHIFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	if (_mouseButtonStatus & EOS_EVENT_MOUSE_BUTTON_STATUS_LEFT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_LEFT_BUTTON;

	if (_mouseButtonStatus & EOS_EVENT_MOUSE_BUTTON_STATUS_RIGHT)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_RIGHT_BUTTON;

	if (_mouseButtonStatus & EOS_EVENT_MOUSE_BUTTON_STATUS_MIDDLE)
		event.data.mouse.modifers |= EOS_MOUSE_MODIFER_MIDDLE_BUTTON;

	addEventToBuffer(_mouseBuffer, event);
}

#elif _PLATFORM_MAC

EOSKeyboardKey EOSEventManager::virtKeyToEOSKeyboardKey(Uint32 virtkey)
{
	EOSKeyboardKey	key = EOSKeyboardKeyNone;

	switch (virtkey)
	{
		case 0x1D:
			key = EOSKeyboardKey0;
			break;
			
		case 0x12:
			key = EOSKeyboardKey1;
			break;
			
		case 0x13:
			key = EOSKeyboardKey2;
			break;
			
		case 0x14:
			key = EOSKeyboardKey3;
			break;
			
		case 0x15:
			key = EOSKeyboardKey4;
			break;
			
		case 0x17:
			key = EOSKeyboardKey5;
			break;
			
		case 0x16:
			key = EOSKeyboardKey6;
			break;
			
		case 0x1A:
			key = EOSKeyboardKey7;
			break;
			
		case 0x1C:
			key = EOSKeyboardKey8;
			break;
			
		case 0x19:
			key = EOSKeyboardKey9;
			break;
			
		case 0x00:
			key = EOSKeyboardKeyA;
			break;
			
		case 0x0B:
			key = EOSKeyboardKeyB;
			break;
			
		case 0x08:
			key = EOSKeyboardKeyC;
			break;
			
		case 0x02:
			key = EOSKeyboardKeyD;
			break;
			
		case 0x0E:
			key = EOSKeyboardKeyE;
			break;
			
		case 0x03:
			key = EOSKeyboardKeyF;
			break;
			
		case 0x05:
			key = EOSKeyboardKeyG;
			break;
			
		case 0x04:
			key = EOSKeyboardKeyH;
			break;
			
		case 0x22:
			key = EOSKeyboardKeyI;
			break;
			
		case 0x26:
			key = EOSKeyboardKeyJ;
			break;
			
		case 0x28:
			key = EOSKeyboardKeyK;
			break;
			
		case 0x25:
			key = EOSKeyboardKeyL;
			break;
			
		case 0x2E:
			key = EOSKeyboardKeyM;
			break;
			
		case 0x2D:
			key = EOSKeyboardKeyN;
			break;
			
		case 0x1F:
			key = EOSKeyboardKeyO;
			break;
			
		case 0x23:
			key = EOSKeyboardKeyP;
			break;
			
		case 0x0C:
			key = EOSKeyboardKeyQ;
			break;
			
		case 0x0F:
			key = EOSKeyboardKeyR;
			break;
			
		case 0x01:
			key = EOSKeyboardKeyS;
			break;
			
		case 0x11:
			key = EOSKeyboardKeyT;
			break;
			
		case 0x20:
			key = EOSKeyboardKeyU;
			break;
			
		case 0x09:
			key = EOSKeyboardKeyV;
			break;
			
		case 0x0D:
			key = EOSKeyboardKeyW;
			break;
			
		case 0x07:
			key = EOSKeyboardKeyX;
			break;
			
		case 0x10:
			key = EOSKeyboardKeyY;
			break;
			
		case 0x06:
			key = EOSKeyboardKeyZ;
			break;
#ifdef NOT_YET
		case 0x75:
			key = EOSKeyboardKeyBack;
			break;
#endif
			
		case 0x35:
			key = EOSKeyboardKeyESC;
			break;
			
		case 0x24:
			key = EOSKeyboardKeyReturn;
			break;
			
		case 0x31:
			key = EOSKeyboardKeySpace;
			break;
			
		case 0x30:
			key = EOSKeyboardKeyTab;
			break;
			
		case 0x73:
			key = EOSKeyboardKeyHome;
			break;
			
		case 0x77:
			key = EOSKeyboardKeyEnd;
			break;
#ifdef NOT_YET			
		case 0x:
			key = EOSKeyboardKeyInsert;
			break;
#endif
			
		case 0x75:
			key = EOSKeyboardKeyDelete;
			break;
			
		case 0x74:
			key = EOSKeyboardKeyPageUp;
			break;
			
		case 0x79:
			key = EOSKeyboardKeyPageDown;
			break;
			
		case 0x7B:
			key = EOSKeyboardKeyLeft;
			break;
			
		case 0x7C:
			key = EOSKeyboardKeyRight;
			break;
			
		case 0x7E:
			key = EOSKeyboardKeyUp;
			break;
			
		case 0x7D:
			key = EOSKeyboardKeyDown;
			break;
			
		case 0x7A:
			key = EOSKeyboardKeyF1;
			break;
			
		case 0x78:
			key = EOSKeyboardKeyF2;
			break;
			
		case 0x63:
			key = EOSKeyboardKeyF3;
			break;
			
		case 0x76:
			key = EOSKeyboardKeyF4;
			break;
			
		case 0x60:
			key = EOSKeyboardKeyF5;
			break;
			
		case 0x61:
			key = EOSKeyboardKeyF6;
			break;
			
		case 0x62:
			key = EOSKeyboardKeyF7;
			break;
			
		case 0x64:
			key = EOSKeyboardKeyF8;
			break;
			
		case 0x65:
			key = EOSKeyboardKeyF9;
			break;
			
		case 0x6D:
			key = EOSKeyboardKeyF10;
			break;
			
		case 0x67:
			key = EOSKeyboardKeyF11;
			break;
			
		case 0x6F:
			key = EOSKeyboardKeyF12;
			break;
	}
	
	return key;
}

void EOSEventManager::interceptKeyboardEventDown(NSEvent* event)
{
	EOSEvent	eosevent;
	Uint32		modifers;
	
	eosevent.type = EOSEventTypeKeyboard;
	
	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.keyboard.type = EOSKeyboardEventTypeKeyDown;
	eosevent.data.keyboard.key = virtKeyToEOSKeyboardKey([event keyCode]);
	eosevent.data.keyboard.modifers = 0;
	
	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.keyboard.modifers |= EOS_KEYBOARD_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.keyboard.modifers |= EOS_KEYBOARD_MODIFER_SHIFT;	
	
	addEventToBuffer(_keyboardBuffer, eosevent);
}

void EOSEventManager::interceptKeyboardEventUp(NSEvent* event)
{
	EOSEvent	eosevent;
	Uint32		modifers;
	
	eosevent.type = EOSEventTypeKeyboard;

	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.keyboard.type = EOSKeyboardEventTypeKeyUp;
	eosevent.data.keyboard.key = virtKeyToEOSKeyboardKey([event keyCode]);
	eosevent.data.keyboard.modifers = 0;
	
	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.keyboard.modifers |= EOS_KEYBOARD_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.keyboard.modifers |= EOS_KEYBOARD_MODIFER_SHIFT;
	
	addEventToBuffer(_keyboardBuffer, eosevent);
}

void EOSEventManager::interceptMouseEventLeftButtonDown(NSEvent* event, NSView* view)
{
	EOSEvent	eosevent;
	NSPoint p = [view convertPoint:[event locationInWindow] fromView:nil];
	Uint32		modifers;

	eosevent.type = EOSEventTypeMouse;

	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.mouse.type = EOSMouseEventTypeLeftButtonDown;
	eosevent.data.mouse.x = p.x;
	eosevent.data.mouse.y = _app->getGfx()->getHeight() -  p.y;
	eosevent.data.mouse.modifers = 0;

	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;
	
	_mouseButtonStatus |= EOS_EVENT_MOUSE_BUTTON_STATUS_LEFT;
	
	addEventToBuffer(_mouseBuffer, eosevent);
}

void EOSEventManager::interceptMouseEventLeftButtonUp(NSEvent* event, NSView* view)
{
	EOSEvent	eosevent;
	NSPoint p = [view convertPoint:[event locationInWindow] fromView:nil];
	Uint32		modifers;
	
	eosevent.type = EOSEventTypeMouse;
		
	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.mouse.type = EOSMouseEventTypeLeftButtonUp;
	eosevent.data.mouse.x = p.x;
	eosevent.data.mouse.y = _app->getGfx()->getHeight() -  p.y;
	eosevent.data.mouse.modifers = 0;
	
	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;
	
	_mouseButtonStatus &= ~EOS_EVENT_MOUSE_BUTTON_STATUS_LEFT;
	
	addEventToBuffer(_mouseBuffer, eosevent);
}

void EOSEventManager::interceptMouseEventLeftButtonDrag(NSEvent* event, NSView* view)
{
	EOSEvent	eosevent;
	NSPoint p = [view convertPoint:[event locationInWindow] fromView:nil];
	Uint32		modifers;
	
	eosevent.type = EOSEventTypeMouse;
	
	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.mouse.type = EOSMouseEventTypeDrag;
	eosevent.data.mouse.x = p.x;
	eosevent.data.mouse.y = _app->getGfx()->getHeight() -  p.y;
	eosevent.data.mouse.modifers = EOS_MOUSE_MODIFER_LEFT_BUTTON;
	
	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;

	addEventToBuffer(_mouseBuffer, eosevent);
}

void EOSEventManager::interceptMouseEventRightButtonDown(NSEvent* event, NSView* view)
{
	EOSEvent	eosevent;
	NSPoint p = [view convertPoint:[event locationInWindow] fromView:nil];
	Uint32		modifers;
	
	eosevent.type = EOSEventTypeMouse;
	
	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.mouse.type = EOSMouseEventTypeRightButtonDown;
	eosevent.data.mouse.x = p.x;
	eosevent.data.mouse.y = _app->getGfx()->getHeight() -  p.y;
	eosevent.data.mouse.modifers = 0;
	
	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;
	
	_mouseButtonStatus |= EOS_EVENT_MOUSE_BUTTON_STATUS_RIGHT;
	
	addEventToBuffer(_mouseBuffer, eosevent);
}

void EOSEventManager::interceptMouseEventRightButtonUp(NSEvent* event, NSView* view)
{
	EOSEvent	eosevent;
	NSPoint p = [view convertPoint:[event locationInWindow] fromView:nil];
	Uint32		modifers;
	
	eosevent.type = EOSEventTypeMouse;
	
	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.mouse.type = EOSMouseEventTypeRightButtonUp;
	eosevent.data.mouse.x = p.x;
	eosevent.data.mouse.y = _app->getGfx()->getHeight() -  p.y;
	eosevent.data.mouse.modifers = 0;
	
	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;
	
	_mouseButtonStatus &= ~EOS_EVENT_MOUSE_BUTTON_STATUS_RIGHT;
	
	addEventToBuffer(_mouseBuffer, eosevent);
}

void EOSEventManager::interceptMouseEventRightButtonDrag(NSEvent* event, NSView* view)
{
	EOSEvent	eosevent;
	NSPoint p = [view convertPoint:[event locationInWindow] fromView:nil];
	Uint32		modifers;
	
	eosevent.type = EOSEventTypeMouse;
	
	if (_initialTimestamp == 0.0)
		_initialTimestamp = event->timestamp;
	
	eosevent.timestamp = (MicroSeconds) ((Double64) (event->timestamp - _initialTimestamp) * 1000000.0);
	
	eosevent.data.mouse.type = EOSMouseEventTypeDrag;
	eosevent.data.mouse.x = p.x;
	eosevent.data.mouse.y = _app->getGfx()->getHeight() -  p.y;
	eosevent.data.mouse.modifers = EOS_MOUSE_MODIFER_RIGHT_BUTTON;
	
	modifers = [event modifierFlags];
	
	if (modifers & NSControlKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_CONTROL;
	
	if (modifers & NSShiftKeyMask)
		eosevent.data.mouse.modifers |= EOS_MOUSE_MODIFER_SHIFT;
	
	addEventToBuffer(_mouseBuffer, eosevent);
}

#elif defined(_PLATFORM_IPHONE)

void EOSEventManager::interceptTouchScreenEventDown(NSSet* touches, UIEvent* event, UIView* view)
{
	EOSEvent	eosevent;
	UITouch *touch;
	CGPoint p;
	Float32		scale = 1.0F;
	
	if (_appRefPtr->isOS40OrHigher() && _appRefPtr->isHighResolution())
		scale = [[UIScreen mainScreen] scale];
	
	if ([touches count] > 0)
	{
		for (touch in touches)
		{
			p = [touch locationInView:view];

			p.x *= scale;
			p.y *= scale;
			
			eosevent.type = EOSEventTypeTouchScreen;

			if (_initialTimestamp == 0.0)
				_initialTimestamp = [touch timestamp];
			else if ([touch timestamp] < _initialTimestamp)
				_initialTimestamp = [touch timestamp];

			eosevent.data.touchscreen.touchObj = touch;
			eosevent.timestamp = (MicroSeconds) ((Double64) ([touch timestamp] - _initialTimestamp) * 1000000.0);

			eosevent.data.touchscreen.type = EOSTouchScreenEventTypeStart;

			if (_app->getGfx()->isLandscape())
			{
				eosevent.data.touchscreen.x = p.y;
				eosevent.data.touchscreen.y = _app->getGfx()->getHeight() - p.x;
			}
			else
			{
				eosevent.data.touchscreen.x = p.x;
				eosevent.data.touchscreen.y = p.y;
			}

			eosevent.data.touchscreen.tapCount = [touch tapCount];

			addEventToBuffer(_touchScreenBuffer, eosevent);
		}
	}
}

void EOSEventManager::interceptTouchScreenEventUp(NSSet* touches, UIEvent* event, UIView* view)
{
	EOSEvent	eosevent;
	UITouch *touch;
	CGPoint p;
	Float32		scale = 1.0F;
	
	if (_appRefPtr->isOS40OrHigher() && _appRefPtr->isHighResolution())
		scale = [[UIScreen mainScreen] scale];
	
	if ([touches count] > 0)
	{
		for (touch in touches)
		{
			p = [touch locationInView:view];

			p.x *= scale;
			p.y *= scale;
			
			eosevent.type = EOSEventTypeTouchScreen;

			if (_initialTimestamp == 0.0)
				_initialTimestamp = [touch timestamp];
			else if ([touch timestamp] < _initialTimestamp)
				_initialTimestamp = [touch timestamp];

			eosevent.data.touchscreen.touchObj = touch;
			eosevent.timestamp = (MicroSeconds) ((Double64) ([touch timestamp] - _initialTimestamp) * 1000000.0);

			eosevent.data.touchscreen.type = EOSTouchScreenEventTypeEnd;

			if (_app->getGfx()->isLandscape())
			{
				eosevent.data.touchscreen.x = p.y;
				eosevent.data.touchscreen.y = _app->getGfx()->getHeight() - p.x;
			}
			else
			{
				eosevent.data.touchscreen.x = p.x;
				eosevent.data.touchscreen.y = p.y;
			}

			eosevent.data.touchscreen.tapCount = [touch tapCount];

			addEventToBuffer(_touchScreenBuffer, eosevent);
		}
	}
}

void EOSEventManager::interceptTouchScreenEventMove(NSSet* touches, UIEvent* event, UIView* view)
{
	EOSEvent	eosevent;
	UITouch *touch;
	CGPoint p;
	Float32		scale = 1.0F;
	
	if (_appRefPtr->isOS40OrHigher() && _appRefPtr->isHighResolution())
		scale = [[UIScreen mainScreen] scale];
	
	if ([touches count] > 0)
	{
		for (touch in touches)
		{
			p = [touch locationInView:view];

			p.x *= scale;
			p.y *= scale;
			
			eosevent.type = EOSEventTypeTouchScreen;

			if (_initialTimestamp == 0.0)
				_initialTimestamp = [touch timestamp];
			else if ([touch timestamp] < _initialTimestamp)
				_initialTimestamp = [touch timestamp];

			eosevent.data.touchscreen.touchObj = touch;
			eosevent.timestamp = (MicroSeconds) ((Double64) ([touch timestamp] - _initialTimestamp) * 1000000.0);

			eosevent.data.touchscreen.type = EOSTouchScreenEventTypeMove;

			if (_app->getGfx()->isLandscape())
			{
				eosevent.data.touchscreen.x = p.y;
				eosevent.data.touchscreen.y = _app->getGfx()->getHeight() - p.x;
			}
			else
			{
				eosevent.data.touchscreen.x = p.x;
				eosevent.data.touchscreen.y = p.y;
			}

			eosevent.data.touchscreen.tapCount = [touch tapCount];

			addEventToBuffer(_touchScreenBuffer, eosevent);
		}
	}
}

void EOSEventManager::interceptTouchScreenEventCancel(NSSet* touches, UIEvent* event, UIView* view)
{
	EOSEvent	eosevent;
	UITouch *touch;
	CGPoint p;
	Float32		scale = 1.0F;
	
	if (_appRefPtr->isOS40OrHigher() && _appRefPtr->isHighResolution())
		scale = [[UIScreen mainScreen] scale];
	
	if ([touches count] > 0)
	{
		for (touch in touches)
		{
			p = [touch locationInView:view];

			p.x *= scale;
			p.y *= scale;
			
			eosevent.type = EOSEventTypeTouchScreen;

			if (_initialTimestamp == 0.0)
				_initialTimestamp = [touch timestamp];
			else if ([touch timestamp] < _initialTimestamp)
				_initialTimestamp = [touch timestamp];

			eosevent.data.touchscreen.touchObj = touch;
			eosevent.timestamp = (MicroSeconds) ((Double64) ([touch timestamp] - _initialTimestamp) * 1000000.0);

			eosevent.data.touchscreen.type = EOSTouchScreenEventTypeCancel;

			if (_app->getGfx()->isLandscape())
			{
				eosevent.data.touchscreen.x = p.y;
				eosevent.data.touchscreen.y = _app->getGfx()->getHeight() - p.x;
			}
			else
			{
				eosevent.data.touchscreen.x = p.x;
				eosevent.data.touchscreen.y = p.y;
			}

			eosevent.data.touchscreen.tapCount = [touch tapCount];

			addEventToBuffer(_touchScreenBuffer, eosevent);
		}
	}
}

#ifdef _SUPPORT_ACCELEROMETER
void EOSEventManager::interceptAccelerometerEventUpdate(UIAcceleration* acceleration)
{
	EOSEvent	eosevent;

	eosevent.type = EOSEventTypeAccelerometer;

	if (_initialAccelerometerTimestamp == 0.0)
		_initialAccelerometerTimestamp = [acceleration timestamp];
	else if ([acceleration timestamp] < _initialAccelerometerTimestamp)
		_initialAccelerometerTimestamp = [acceleration timestamp];
	
	eosevent.timestamp = (MicroSeconds) ((Double64) ([acceleration timestamp] - _initialAccelerometerTimestamp) * 1000000.0);

	eosevent.data.accelerometer.type = EOSAccelerometerEventTypeUpdate;
	
	eosevent.data.accelerometer.x = [acceleration x];
	eosevent.data.accelerometer.y = [acceleration y];
	eosevent.data.accelerometer.z = [acceleration z];

	addEventToBuffer(_accelerometerBuffer, eosevent);
}
#endif /* _SUPPORT_ACCELEROMETER */

#else

	#error _PLATFORM not defined.
	 
#endif /* _PLATFORM_PC */

