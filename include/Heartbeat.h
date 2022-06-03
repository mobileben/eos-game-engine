/******************************************************************************
 *
 * File: Heartbeat.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Heartbeat data structures
 * 
 *****************************************************************************/

#ifndef __HEARTBEAT_H__
#define __HEARTBEAT_H__

#include "Platform.h"

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

#include <mach/mach.h>
#include <mach/mach_time.h>

#endif /* _PLATFORM_MAC */

class Heartbeat : EOSFrameworkComponent
{
public:
	static const Uint32			MicroSecondsPerSecond = 1000000;
	static const Uint32			MilliSecondsPerSecond = 1000;
	static const Uint32			MicroSecondsToMilliSecondsPerSecond = MicroSecondsPerSecond / MilliSecondsPerSecond;

private:
	static const FixedFrame		_microseconds_per_frame;

	MicroSeconds				_last_delta_time;
	MicroSeconds				_last_delta_time_running;
	FixedFrame					_delta_frames;
	FixedFrame					_delta_frames_running;

	bool						_running;

#ifdef _PLATFORM_PC

	LARGE_INTEGER				_counter;
	LARGE_INTEGER				_lastCounter;
	LARGE_INTEGER				_frequency;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	uint64_t						_counter;
	uint64_t						_lastCounter;
	mach_timebase_info_data_t		_info;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

public:
	Heartbeat();
	~Heartbeat();

	//	This is an internal routine for systems that do not provide an external tick
	void					platformHeartbeat(void);

	//	This is an external routine for systems that provide an external tick. If the system
	//	internalizes the heartbeat (ie. EOS takes care of it, then tick() will be called from
	// 	platformHeartbeat();
	void 					tick(unsigned long delta_t_microseconds);

	void 					reset(void);

	inline bool				isRunning(void) const { return _running; }
	void 					setRunning(bool run);

	inline MicroSeconds		getDeltaMicroSecondsFree(void) const { return _last_delta_time; }
	inline MicroSeconds		getDeltaMicroSecondsRunning(void) const { return _last_delta_time_running; }
	inline FixedFrame 		getDeltaFramesFree(void) const { return _delta_frames; }
	inline FixedFrame 		getDeltaFramesRunning(void) const { return _delta_frames_running; }

	MicroSeconds			getMicroSeconds(void);
};

#endif /* __HEARTBEAT_H__ */
