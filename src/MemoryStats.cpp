/******************************************************************************
 *
 * File: MemoryStats.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Memory Stats 
 * 
 *****************************************************************************/

#include "Platform.h"
#include "MemoryStats.h"

MemoryStats*	_memoryStats = NULL;

#ifdef _DEBUG_MEMORY

void* memoryAllocate(size_t size)
{
	if (_memoryStats)
		return _memoryStats->memoryAllocate(size);
	else
		return malloc(size);
}

void memoryFree(void* ptr)
{
	if (_memoryStats)
		_memoryStats->memoryFree(ptr);
	else
		free(ptr);
}

#endif /* _DEBUG_MEMORY */

MemoryStats::MemoryStats() : _totalMemory(0), _initialMemoryAvail(0), _currMemoryAvail(0), _currMemoryUsage(0), _maxMemoryUsage(0), 
							_numNewCalls(0), _newedMemory(0), _numDeleteCalls(0), _deletedMemory(0), _unaccountedForDeletes(0),
							_maxMemoryRecords(0), _numMemoryRecords(0), _memoryRecords(NULL)
{
#ifdef _PLATFORM_PC

	MEMORYSTATUSEX	status;

#ifdef _DEBUG_MEMORY

#if defined(_PLATFORM_PC) && defined(_CRTDBG_MAP_ALLOC)

	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#endif /* _PLATFORM_PC && _CRTDBG_MAP_ALLOC */

#endif /* _DEBUG_MEMORY */

	memset(&status, 0, sizeof(MEMORYSTATUSEX));

	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	_totalMemory = status.ullTotalVirtual;
	_initialMemoryAvail = status.ullAvailVirtual;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)

	_totalMemory = (MemorySize) [[NSProcessInfo processInfo] physicalMemory];
	_initialMemoryAvail = NSRealMemoryAvailable();
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

MemoryStats::~MemoryStats()
{
	if (_memoryRecords)
	{
		free(_memoryRecords);

		_memoryRecords = NULL;
		_maxMemoryRecords = _numMemoryRecords = 0;
	}
}

void MemoryStats::increaseMemoryRecords(void)
{
	if (_memoryRecords)
	{
		MemoryStatsRecord*	newRecords;

		newRecords = (MemoryStatsRecord*) malloc(sizeof(MemoryStatsRecord) * (_maxMemoryRecords + MEMORY_STATS_RECORD_MIN_SET_SIZE));

		if (newRecords)
		{
			memset(&newRecords[_maxMemoryRecords], 0, sizeof(MemoryStatsRecord) * MEMORY_STATS_RECORD_MIN_SET_SIZE);
			memcpy(newRecords, _memoryRecords, sizeof(MemoryStatsRecord) * _maxMemoryRecords);

			free(_memoryRecords);

			_maxMemoryRecords += MEMORY_STATS_RECORD_MIN_SET_SIZE;

			_memoryRecords = newRecords;
		}
		else
		{
			free(_memoryRecords);
			_memoryRecords = NULL;
			_maxMemoryRecords = _numMemoryRecords = 0;
		}
	}
	else
	{
		_memoryRecords = (MemoryStatsRecord*) malloc(sizeof(MemoryStatsRecord) * MEMORY_STATS_RECORD_MIN_SET_SIZE);

		if (_memoryRecords)
		{
			_maxMemoryRecords = MEMORY_STATS_RECORD_MIN_SET_SIZE;
			_numMemoryRecords = 0;
	
			memset(_memoryRecords, 0, sizeof(MemoryStatsRecord) * _maxMemoryRecords);
		}
	}
}

MemorySize MemoryStats::getCurrentMemoryAvailable(void)
{
	MemorySize	size = 0;
	MemorySize	diff = 0;

#ifdef _PLATFORM_PC

	MEMORYSTATUSEX	status;

	memset(&status, 0, sizeof(MEMORYSTATUSEX));

	status.dwLength = sizeof(MEMORYSTATUSEX);

	GlobalMemoryStatusEx(&status);

	_currMemoryAvail = status.ullAvailVirtual;

	size = _currMemoryAvail;

#elif defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	
	_currMemoryAvail = NSRealMemoryAvailable();

	size = _currMemoryAvail;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
	
	if (_currMemoryAvail < _initialMemoryAvail)
		diff = _initialMemoryAvail - _currMemoryAvail;

	return size;
}

MemoryStatsRecord* MemoryStats::findMemoryRecord(void* ptr)
{
	MemoryStatsRecord*	record = NULL;
	Uint32				i;

	for (i=0;i<_numMemoryRecords;i++)
	{
		if (_memoryRecords[i].ptr == ptr)
		{
			record = &_memoryRecords[i];
			break;
		}
	}

	return record;
}

MemoryStatsRecord* MemoryStats::findNextFreeMemoryRecord(void)
{
	MemoryStatsRecord*	record = NULL;

	if (_numMemoryRecords >= _maxMemoryRecords)
		increaseMemoryRecords();

	record = &_memoryRecords[_numMemoryRecords];

	record->state = MEMORY_STATS_RECORD_STATE_USED;
	_numMemoryRecords++;

	return record;
}

void* MemoryStats::memoryAllocate(size_t size)
{
	MemoryStatsRecord*	record;
	void*				ptr = NULL;

	record = findNextFreeMemoryRecord();

	if (record)
	{
		_numNewCalls++;

		ptr = malloc(size);

		record->amount = size;
		record->state = MEMORY_STATS_RECORD_STATE_ALLOCATED;
		record->ptr = ptr;

		_newedMemory += size;
	}

	_currMemoryUsage = _newedMemory - _deletedMemory;

	if (_currMemoryUsage > _maxMemoryUsage)
		_maxMemoryUsage = _currMemoryUsage;

	return ptr;
}

void MemoryStats::memoryFree(void* ptr)
{
	MemoryStatsRecord*	record;

	record = findMemoryRecord(ptr);

	if (record)
	{
		_numDeleteCalls++;
		record->state = MEMORY_STATS_RECORD_STATE_FREED;

		_deletedMemory += record->amount;

		_currMemoryUsage = _newedMemory - _deletedMemory;
	}
	else
		_unaccountedForDeletes++;

	free(ptr);
}

