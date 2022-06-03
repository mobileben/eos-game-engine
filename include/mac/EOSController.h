//
//  EOSController.h
//  eosTest
//
//  Created by Benjamin Lee on 1/27/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface EOSController : NSObject {	
	NSThread*		_updateThread;
	Boolean			_updateThreadRunning;

	NSThread*		_renderThread;
	Boolean			_renderThreadRunning;

	NSConditionLock*	_needsRender;
}

-(void) updateThread;
-(void) renderThread;
-(void) notifyExit;

@end
