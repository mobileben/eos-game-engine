/******************************************************************************
 *
 * File: MemoryStats.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Memory Stats 
 * 
 *****************************************************************************/

#ifndef __MEMORYSTATS_H__
#define __MEMORYSTATS_H__

#include "Platform.h"

#ifdef _PLATFORM_PC

typedef	Uint64	MemorySize;

#elif defined(_PLATFORM_MAC)

typedef	Uint64	MemorySize;

#elif defined(_PLATFORM_IPHONE)

typedef	Uint32	MemorySize;

#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

#define MEMORY_STATS_RECORD_MIN_SET_SIZE	64

#define MEMORY_STATS_RECORD_STATE_UNUSED	0
#define MEMORY_STATS_RECORD_STATE_USED		1
#define MEMORY_STATS_RECORD_STATE_ALLOCATED	2
#define MEMORY_STATS_RECORD_STATE_FREED		3

typedef struct
{
	Uint8	state;
	size_t 	amount;
	void*	ptr;
} MemoryStatsRecord;

class MemoryStats : public EOSObject
{
private:
	MemorySize	_totalMemory;

	MemorySize			_initialMemoryAvail;
	MemorySize			_currMemoryAvail;		//	Saved for debugging if needed

	MemorySize			_currMemoryUsage;
	MemorySize			_maxMemoryUsage;

	Uint32				_numNewCalls;
	MemorySize			_newedMemory;

	Uint32				_numDeleteCalls;
	Uint32				_unaccountedForDeletes;
	MemorySize			_deletedMemory;

	Uint32				_maxMemoryRecords;
	Uint32				_numMemoryRecords;
	MemoryStatsRecord*	_memoryRecords;

	void				increaseMemoryRecords(void);
	MemoryStatsRecord*	findMemoryRecord(void* ptr);
	MemoryStatsRecord*	findNextFreeMemoryRecord(void);

public:
	MemoryStats();
	~MemoryStats();

	inline MemorySize	getTotalSystemMemory(void) { return _totalMemory; }
	inline MemorySize	getInitialMemoryAvailable(void) { return _initialMemoryAvail; }
	MemorySize			getCurrentMemoryAvailable(void);

	inline MemorySize	getTotalAllocatedMemory(void) { return _newedMemory; }
	inline MemorySize	getTotalCurrentMemory(void) { return _currMemoryUsage; }
	inline MemorySize	getMaxAllocatedMemory(void) { return _maxMemoryUsage; }

	void*				memoryAllocate(size_t size);
	void 				memoryFree(void* ptr);
};

#ifdef _DEBUG_MEMORY

extern void* memoryAllocate(size_t size);
extern void memoryFree(void* ptr);

#ifdef _PLATFORM_PC

#if defined(_CRTDBG_MAP_ALLOC)

#define DEBUG_NEW	new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

#else

inline void* operator new(size_t size)
{
	return memoryAllocate(size);
}

inline void operator delete(void* ptr)
{
	memoryFree(ptr);
}

#endif /* _CRTDBG_MAP_ALLOC */

#else


inline void* operator new(size_t size)
{
	return memoryAllocate(size);
}

inline void operator delete(void* ptr)
{
	memoryFree(ptr);
}

#endif /* _PLATFORM_PC */

#endif /* _DEBUG_MEMORY */

extern MemoryStats*	_memoryStats;

#endif /* __MEMORYSTATS_H__ */
