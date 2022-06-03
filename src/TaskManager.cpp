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

#include "TaskManager.h"

TaskManager::TaskManager() : _loadBufferObject(NULL), _lockedLoadBuffer(false)
{
	_app = NULL;

	_numTasks = _currentTask = 0;

	_maxTasks = 0;
	_tasks = NULL;
}

TaskManager::~TaskManager()
{
	deallocateTaskPool();
}

void TaskManager::setLoadBufferObject(LoadBufferObject* obj)
{
	_loadBufferObject = obj;
	_lockedLoadBuffer = false;
}

EOSError TaskManager::allocateTaskPool(Uint32 num)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	AssertWDesc(num != 0, "num must != 0");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateTaskPool();

	_tasks = new Task*[num];

	AssertWDesc(_tasks != NULL, "_tasks is NULL");

	if (_tasks)
	{
		_maxTasks = num;
		_currentTask = 0;

		for (i=0;i<_maxTasks;i++)
			_tasks[i] = NULL;
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

void TaskManager::deallocateTaskPool(void)
{
	Uint32	i;

	if (_tasks)
	{
		for (i=0;i<_maxTasks;i++)
		{
			if (_tasks[i] != NULL)
			{
				if (_tasks[i]->isPersistent() == false)
				{
					delete _tasks[i];
					_tasks[i] = NULL;
				}
			}
		}

		delete [] _tasks;
		_tasks = NULL;
	}

	_maxTasks = 0;
}

EOSError TaskManager::addTask(Task* task)
{
	EOSError	error = EOSErrorNone;

	AssertWDesc(task != NULL, "task is NULL");

	if (task)
	{
		if (_numTasks < _maxTasks)
		{
			if (_numTasks == 0 || _currentTask < _numTasks)
			{
				task->setTaskManager(this);
				_tasks[_numTasks] = task;

				_numTasks++;
			}
			else
				error = EOSErrorUnsupported;
		}
		else
		{
			AssertWDesc(1 == 0, "TaskManager::addTask() task list full.\n");
			error = EOSErrorOutOfBounds;
		}
	}

	return error;
}

void TaskManager::reset(void)
{
	Uint32	i;

	for (i=0;i<_numTasks;i++)
	{
		if (_tasks[i]->isPersistent() == false)
			delete _tasks[i];

		_tasks[i] = NULL;
	}

	for (i=_numTasks;i<_maxTasks;i++)
		_tasks[i] = NULL;

	_numTasks = 0;
	_currentTask = 0;

	if (_loadBufferObject)
	{
		if (_lockedLoadBuffer)
		{
			_loadBufferObject->unlock();
			_lockedLoadBuffer = false;
		}
	}
}

Uint32 TaskManager::process(EOSError& error)
{
	Uint32	current = TaskManagerCompleted;
	Task*	task;
	Boolean	processTask = false;

	error = EOSErrorNone;

	if ((_numTasks >= 0) && (_currentTask < _numTasks))
	{
		if (_loadBufferObject)
		{
			if (_lockedLoadBuffer == false)
			{
				if (_loadBufferObject->isLocked() == false)
				{
					_loadBufferObject->lock();
					_lockedLoadBuffer = true;
				}
			}
			else
				processTask = true;
		}
		else
			processTask = true;

		if (processTask)
		{
			task = _tasks[_currentTask];
	
			if (task)
			{
				switch (task->getTaskState())
				{
					case Task::TaskStateInitial:
						error = task->prologue();
						task->setTaskState(Task::TaskStateRunning);
						break;
	
					case Task::TaskStateRunning:
						if (task->isComplete(error) == true)
						{
							error = task->epilogue();
							task->setTaskState(Task::TaskStateDone);
	
							if (task->getCompleteFunc())
								task->getCompleteFunc()(task->getCompleteObject(), task->getCompleteData());

							_currentTask++;
						}
						break;
	
					default:
//						_currentTask++;
						break;
				}
			}
			else
			{
				error = EOSErrorNULL;
				_currentTask++;
			}
		}

		if (_currentTask < _numTasks)
			current = _currentTask;
		else if (_loadBufferObject)
		{
			if (_lockedLoadBuffer)
			{
				_loadBufferObject->unlock();
				_lockedLoadBuffer = false;
			}
		}

	}

	return current;
}

Boolean TaskManager::isComplete(void)
{
	Boolean	done;

	if (_currentTask >= _numTasks)
		done = true;
	else
		done = false;

	return done;
}

Task* TaskManager::getCurrentTask(void)
{
	Task* task = NULL;
	
	if (_currentTask < _numTasks)
		task = _tasks[_currentTask];
	
	return task;
}

