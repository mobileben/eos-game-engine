/******************************************************************************
 *
 * File: AppContainer.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * App Container is used to hold the instance of the App. This is done so that
 * we can let the app use the destructor to do the final cleanup of the app
 * 
 *****************************************************************************/

#ifndef __APPCONTAINER_H__
#define __APPCONTAINER_H__

#include "Platform.h"
#include "App.h"

class AppContainer : EOSObject
{
private:
	App*			_app;
	MemoryStats*	_memStats;

public:
	AppContainer();
	~AppContainer();

	void	setApp(App* app);
	void	setMemoryStats(MemoryStats* memory);
};

#endif /* __APPCONTAINER_H__ */

