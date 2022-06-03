/******************************************************************************
 *
 * File: PerformanceTimer.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Performance timer data structures
 * 
 *****************************************************************************/

#include "PerformanceTimer.h"
#include "App.h"

PerformanceTimer::PerformanceTimer() : _state(TimerStateOff), _totalTime(0), _elapsedTime(0)
{
#ifdef _PLATFORM_PC

	QueryPerformanceCounter(&_counter);
	_lastCounter = _counter;
	QueryPerformanceFrequency(&_frequency);

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	_info.numer = 0; _info.denom = 0;
	mach_timebase_info(&_info); 

	_counter = mach_absolute_time();
	
	_lastCounter = _counter;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

PerformanceTimer::~PerformanceTimer()
{
}

void PerformanceTimer::reset(void)
{
	_state = TimerStateOff;

	_totalTime = 0;
	_elapsedTime = 0;
}

void PerformanceTimer::start(void)
{
	_state = TimerStateRunning;

#ifdef _PLATFORM_PC

	QueryPerformanceCounter(&_counter);

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	_info.numer = 0; _info.denom = 0;
	mach_timebase_info(&_info); 

#else
	
#error _PLATFORM not defined.	
	
#endif /* _PLATFORM_PC */

	_lastCounter = _counter;
}

void PerformanceTimer::updateCurrentTimes(void)
{
#ifdef _PLATFORM_PC

	double	elapsedTime;

	QueryPerformanceCounter(&_counter);

	elapsedTime = (_counter.QuadPart - _lastCounter.QuadPart) * 1000000.0 / _frequency.QuadPart;

	_lastCounter = _counter;

	_totalTime += (Uint32) elapsedTime;
	_elapsedTime += (Uint32) elapsedTime;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
#ifdef _BASE_SDK_3_0_OR_HIGHER
	if (_appRefPtr->isOS30OrHigher() == false)
	{
		Float32		elapsedTime;
		Uint32	delta;
		
		_counter = mach_absolute_time();
		
		delta = _counter - _lastCounter;
		
		elapsedTime = (Float32) delta * (Float32) _info.numer / (Float32) _info.denom / 1000.0F;
		
		_lastCounter = _counter;
		
		_totalTime += (Uint32) elapsedTime;
		_elapsedTime += (Uint32) elapsedTime;
	}
	else
#endif
	{
		double		elapsedTime;
		uint64_t	delta;
		
		_counter = mach_absolute_time();
		
		delta = _counter - _lastCounter;
		
		elapsedTime = (double) delta * (double) _info.numer / (double) _info.denom / 1000.0;
		
		_lastCounter = _counter;
		
		_totalTime += (Uint64) elapsedTime;
		_elapsedTime += (Uint64) elapsedTime;
	}
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

void PerformanceTimer::pause(void)
{
	if (_state == TimerStateRunning)
		updateCurrentTimes();

	_state = TimerStatePaused;
}

void PerformanceTimer::lap(void)
{
	_elapsedTime = 0;
}

MicroSeconds PerformanceTimer::getTotalRunningTime(void)
{
	if (_state == TimerStateRunning)
		updateCurrentTimes();

	return _totalTime;
}

MicroSeconds PerformanceTimer::getElapsedTime(void)
{
	if (_state == TimerStateRunning)
		updateCurrentTimes();

	return _elapsedTime;
}

