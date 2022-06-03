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

#include "Platform.h"
#include "AppContainer.h"

AppContainer::AppContainer() : _app(NULL)
{
	_memStats = new MemoryStats;
	_memoryStats = _memStats;
}

AppContainer::~AppContainer()
{
	if (_app)
	{
		delete _app;
		_app = NULL;
	}

	if (_memStats)
	{
		delete _memoryStats;
		_memStats = NULL;
	}
}

void AppContainer::setApp(App* app)
{
	_app = app;
	_appRefPtr = app;
}
