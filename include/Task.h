/******************************************************************************
 *
 * File: Task.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Task Base Class
 * Tasks are simple tasks that may take a trivial or significant amount of time.
 * They are not threads or processes. They are meant to encapsulate a task to be
 * completed over time. The program can query as to whether or not the task is
 * complete.
 * 
 *****************************************************************************/

#ifndef __TASK_H__
#define __TASK_H__

#include "Platform.h"
#include "EOSError.h"

class TaskManager;

typedef void (*TaskCompleteFunc)(void* object, Uint32 data);

class Task : EOSFrameworkComponent
{
public:
	typedef enum
	{
		TaskStateInitial = 0,
		TaskStateRunning,
		TaskStateDone,
	} TaskState;

private:
	TaskState			_state;
	EOSError			_error;
	Boolean				_persistent;
	TaskManager*		_taskMgr;
	TaskCompleteFunc	_completeFunc;
	void*				_completeObject;
	Uint32				_completeData;
#ifdef _DEBUG
	Char*				_name;
#endif /* _DEBUG */

public:
	Task() 
	{ 
		_state = TaskStateInitial; 
		_error = EOSErrorNone; 
		_persistent = false; 
		_taskMgr = NULL; 
		_completeFunc = NULL; 
		_completeObject = NULL; 
		_completeData = 0; 
#ifdef _DEBUG
		_name = NULL; 
#endif /* _DEBUG */
	}

	~Task() 
	{
#ifdef _DEBUG
		if (_name)
			delete _name;
#endif /* _DEBUG */
	}

	virtual EOSError		prologue(void) { return EOSErrorNone; }
	virtual EOSError		epilogue(void) { return EOSErrorNone; }
	virtual Boolean			isComplete(EOSError& error) { error = EOSErrorNone; return true; }

	inline void				setTaskManager(TaskManager* mgr) { _taskMgr = mgr; }
	inline TaskManager*		getTaskManager(void) { return _taskMgr; }

	inline void				setPersistence(Boolean persistent) { _persistent = persistent; }

	inline void				setCompleteFunc(TaskCompleteFunc func, void* compObject, Uint32 compData) { _completeFunc = func; _completeObject = compObject; _completeData = compData; }
	inline TaskCompleteFunc	getCompleteFunc(void) { return _completeFunc; }
	inline void*			getCompleteObject(void) { return _completeObject; }
	inline Uint32			getCompleteData(void) { return _completeData; }

	inline Boolean			isPersistent(void) { return _persistent; }

	inline TaskState		getTaskState(void) { return _state; }
	inline void				setTaskState(TaskState state) { _state = state; }
	
#ifdef _DEBUG
	inline void				setName(const Char* name)
	{
		if (_name)
			delete _name;
		
		_name = new Char[strlen(name) + 1];
		
		if (_name)
			strcpy(_name, name);
	}
	
	inline Char*			getName(void) { return _name; }
#endif /* _DEBUG */

	inline EOSError			getError(void) { return _error; }
};

#endif /* __TASK_H__ */
