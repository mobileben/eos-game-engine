//
//  EOSController.m
//  eosTest
//
//  Created by Benjamin Lee on 1/27/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import "EOSController.h"
#include "App.h"

@implementation EOSController


extern App*	_appRefPtr;

-(void) applicationDidFinishLaunching:(NSNotification*) notification
{	
	AssertWDesc(_appRefPtr != NULL, "applicationDidFinishLaunching() NULL app");
	
	if (_appRefPtr != NULL)
	{
		_appRefPtr->initializeEOSFramework(self);
		_appRefPtr->init();
		_appRefPtr->getHeartbeat()->setRunning(true);
		_appRefPtr->getHeartbeat()->reset();
	}
	
	_updateThread = [[NSThread alloc] initWithTarget:self selector:@selector(updateThread) object:nil];	
	
	_renderThread = [[NSThread alloc] initWithTarget:self selector:@selector(renderThread) object:nil];	

	_needsRender = [[NSConditionLock alloc] initWithCondition:NO];
	
	[_updateThread start];
	[_renderThread start];
}

-(void) applicationWillTerminate:(NSNotification*) notification
{
	NSLog(@"applicationWillTerminate");
	
	if (_appRefPtr != NULL)
		_appRefPtr->exit();
}

-(BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication*) app
{
	BOOL	close = YES;

	NSLog(@"appShouldTerminate");
	if (_appRefPtr != NULL)
		_appRefPtr->exit();
	
	return close;
}

-(void) notifyExit
{
	NSLog(@"notifyExit");
	if (_updateThread != nil)
	{
		_updateThreadRunning = false;
		if ([_updateThread isCancelled])
			NSLog(@"Already cancelled");
		[_updateThread cancel];
	}
	
	if (_renderThread != nil)
	{
		_renderThreadRunning = false;
		[_renderThread cancel];
	}
	
	if (_needsRender != nil)
	{
//		_needsRender = nil;
	}
}

-(void) updateThread
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	_updateThreadRunning = true;
	_appRefPtr->getHeartbeat()->reset();

	//	We do the initial lock here to keep everything in sync
	//	Since you cannot unlock before a lock
	[_needsRender lockWhenCondition:NO];

	_appRefPtr->getGfx()->lockGLContext();

	_appRefPtr->getGfx()->unlockGLContext();
	
	while ([_updateThread isCancelled] == NO && _updateThreadRunning)
	{
		_appRefPtr->getHeartbeat()->platformHeartbeat();
		
		if (_appRefPtr->updateAssertHitCheck() == false)
			_appRefPtr->update(_appRefPtr->getHeartbeat()->getDeltaMicroSecondsFree());
		
		//	Signal the render thread to wake up and truly render
		[_needsRender unlockWithCondition:YES];
		[_needsRender lockWhenCondition:NO];
	}

	NSLog(@"Update released");
	
	[pool release];
}

-(void) renderThread
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	_renderThreadRunning = true;
	
	while (_renderThreadRunning)
	{
		[_needsRender lockWhenCondition:YES];
		
		if (_appRefPtr->renderAssertHitCheck() == false)
			_appRefPtr->render();

		[_needsRender unlockWithCondition:NO];
	}
	
	NSLog(@"Render released");
	[pool release];
}

@end
