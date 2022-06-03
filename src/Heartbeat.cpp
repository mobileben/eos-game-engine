/******************************************************************************
 *
 * File: Heartbeat.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Heartbeat data structures
 * 
 *****************************************************************************/

#include "Heartbeat.h"
#include "App.h"

const FixedFrame Heartbeat::_microseconds_per_frame = (FixedFrame) (((float) 1000000 / (float) FRAMES_PER_SECOND) * (float) (1 << ANIM_PRECISION));

Heartbeat::Heartbeat() : _last_delta_time(0), _last_delta_time_running(0), _delta_frames(0), _delta_frames_running(0), _running(true)
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

Heartbeat::~Heartbeat()
{
}

void Heartbeat::setRunning(bool run)
{
	_running = run;

	if(_running == false)
	{
		_last_delta_time_running = 0;
		_delta_frames_running = 0;
	}
	else
	{
		_last_delta_time_running = _last_delta_time;
		_delta_frames_running = _delta_frames;
	}
}

void Heartbeat::platformHeartbeat(void)
{
#ifdef _PLATFORM_PC

	double	elapsedTime;

	QueryPerformanceCounter(&_counter);

	elapsedTime = (_counter.QuadPart - _lastCounter.QuadPart) * 1000000.0 / _frequency.QuadPart;

	_lastCounter = _counter;

	tick((Uint32) elapsedTime);

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

#ifdef _BASE_SDK_3_0_OR_HIGHER
	if (_appRefPtr->isOS30OrHigher() == false)
	{
		Float32		elapsedTime;
		Uint32		delta;

		_counter = mach_absolute_time();

		delta = _counter - _lastCounter;

		elapsedTime = (Float32) delta * (Float32) _info.numer / (Float32) _info.denom / 1000.0F;

		_lastCounter = _counter;

		tick(elapsedTime);
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
		
		tick(elapsedTime);
	}
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

void Heartbeat::reset(void)
{
#ifdef _PLATFORM_PC

	QueryPerformanceCounter(&_counter);

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	_info.numer = 0; _info.denom = 0;
	mach_timebase_info(&_info); 

#else
	
#error _PLATFORM not defined.	
	
#endif /* _PLATFORM_PC */

	_lastCounter = _counter;

	tick(0);
}

void Heartbeat::tick (unsigned long delta_t_microseconds)
{
#ifdef _PLATFORM_IPHONE
#ifdef _BASE_SDK_3_0_OR_HIGHER
	if (_appRefPtr->isOS30OrHigher() == false)
	{
		Sint32	dt = (delta_t_microseconds << ANIM_PRECISION);

		_last_delta_time = delta_t_microseconds;

		_delta_frames = (FixedFrame) ((dt << ANIM_PRECISION) / _microseconds_per_frame);

		if(_running)
		{
			_last_delta_time_running = _last_delta_time;
			_delta_frames_running = _delta_frames;
		}
		else
		{
			_last_delta_time_running = 0;
			_delta_frames_running = 0;
		}
	}
	else
#endif
#endif
	{
		Sint64	dt = (delta_t_microseconds << ANIM_PRECISION);

		_last_delta_time = delta_t_microseconds;

		_delta_frames = (FixedFrame) ((dt << ANIM_PRECISION) / _microseconds_per_frame);

		if(_running)
		{
			_last_delta_time_running = _last_delta_time;
			_delta_frames_running = _delta_frames;
		}
		else
		{
			_last_delta_time_running = 0;
			_delta_frames_running = 0;
		}
	}
}

MicroSeconds Heartbeat::getMicroSeconds(void)
{
#ifdef _PLATFORM_PC
	LARGE_INTEGER	time;

	double	elapsedTime;

	QueryPerformanceCounter(&time);

	elapsedTime = time.QuadPart * 1000000.0 / _frequency.QuadPart;

	return (MicroSeconds) elapsedTime;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

#ifdef _BASE_SDK_3_0_OR_HIGHER
	if (_appRefPtr->isOS30OrHigher() == false)
	{
		Float32		elapsedTime;
		Uint32		time;

		time = mach_absolute_time();

		elapsedTime = (Float32) time * (Float32) _info.numer / (Float32) _info.denom / 1000.0F;

		return (MicroSeconds) elapsedTime;
	}
	else
#endif
	{
		double		elapsedTime;
		uint64_t	time;

		time = mach_absolute_time();

		elapsedTime = (double) time * (double) _info.numer / (double) _info.denom / 1000.0;

		return (MicroSeconds) elapsedTime;
	}
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

