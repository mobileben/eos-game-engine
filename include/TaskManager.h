/******************************************************************************
 *
 * File: TaskManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Task Manager Base Class
 * Task Manager manages the tasks. Specialized task managers can be created
 * for various purposes. This task manager runs things one at a time. Therefore
 * a particular task must complete before the next task can be executed.
 * 
 *****************************************************************************/

#ifndef __TASKMANAGER_H__
#define __TASKMANAGER_H__

#include "Platform.h"
#include "Task.h"
#include "LoadBufferObject.h"

class App;

class TaskManager : public EOSFrameworkComponent
{
public:
	typedef enum
	{
		TaskManagerCompleted = 0xFFFFFFFF
	};

private:
	App*				_app;

	Uint32				_numTasks;
	Uint32				_currentTask;

	Uint32				_maxTasks;
	Task**				_tasks;

	Boolean				_lockedLoadBuffer;
	LoadBufferObject*	_loadBufferObject;

public:
	TaskManager();
	~TaskManager();

	EOSError			allocateTaskPool(Uint32 num);
	void				deallocateTaskPool(void);

	void				setLoadBufferObject(LoadBufferObject* obj);

	virtual EOSError	addTask(Task* task);

	virtual void		reset(void);
	virtual Uint32		process(EOSError& error);

	Boolean				isComplete(void);
	
	inline void			setApp(App* app) { _app = app; }
	inline App*			getApp(void) { return _app; }

	inline Uint32		getCurrentTaskIndex(void) { return _currentTask; }
	Task*				getCurrentTask(void);	
	inline Uint32		getNumTaskObjects(void) { return _numTasks; }
};

#endif /* __TASKMANAGER_H__ */

