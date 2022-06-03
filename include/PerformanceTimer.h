/******************************************************************************
 *
 * File: PerformanceTimer.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Performance timer data structures
 * 
 *****************************************************************************/

#ifndef __PERFORMANCE_TIMER_H__
#define __PERFORMANCE_TIMER_H__

#include "Platform.h"

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

#include <mach/mach.h>
#include <mach/mach_time.h>

#endif /* _PLATFORM_MAC */

class PerformanceTimer : EOSObject
{
public:
	typedef enum
	{
		TimerStateOff = 0,
		TimerStateRunning,
		TimerStatePaused,
	} TimerState;

private:
	TimerState					_state;

	MicroSeconds				_totalTime;
	MicroSeconds				_elapsedTime;

#ifdef _PLATFORM_PC

	LARGE_INTEGER				_counter;
	LARGE_INTEGER				_lastCounter;
	LARGE_INTEGER				_frequency;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	uint64_t					_counter;
	uint64_t					_lastCounter;
	mach_timebase_info_data_t	_info;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

	void				updateCurrentTimes(void);

public:
	PerformanceTimer();
	~PerformanceTimer();

	void				reset(void);

	void				start(void);
	void				pause(void);
	void				lap(void);

	inline TimerState	getTimerState(void) { return _state; }

	MicroSeconds		getTotalRunningTime(void);
	MicroSeconds 		getElapsedTime(void);
};

#endif /* __PERFORMANCE_TIMER_H__ */
