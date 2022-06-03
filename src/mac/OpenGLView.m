//
//  OpenGLView.m
//  eosTest
//
//  Created by Benjamin Lee on 1/26/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import "OpenGLView.h"
#include "App.h"

extern App*	_appRefPtr;

@implementation OpenGLView

-(void) awakeFromNib
{
}

-(id) initWithFrame:(NSRect) frameRect
{
	if ((self = [super initWithFrame:frameRect]) != nil)
	{
		_appRefPtr->initVideo(self);
	}
	
	return self;
}

-(void) setNSOpenGLContext: (NSOpenGLContext*) ctx
{
	_context = ctx;
}

-(void) setNSOpenGLPixelFormat: (NSOpenGLPixelFormat*) fmt
{
	_pxlFmt = fmt;
}

-(NSOpenGLContext*) openGLContext
{
	return _context;
}

-(void) lockFocus
{
	[super lockFocus];
	
	if (_context != nil)
	{
		if ([_context view] != self)
		{
			[_context setView:self];
		}
	}
	
	[_context makeCurrentContext];
}

-(void) prepareOpenGL
{
}

-(void) drawRect:(NSRect) rect
{
	NSLog(@"drawRect");
#if 0	
	glViewport(0, 0, (GLsizei) rect.size.width, (GLsizei) rect.size.height);
	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	[[self openGLContext] flushBuffer];
#endif
}

-(BOOL) acceptsFirstResponder
{
	return YES;
}

-(void) keyDown:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptKeyboardEventDown(event);
}

-(void) keyUp:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptKeyboardEventUp(event);
}

-(void) mouseDown:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptMouseEventLeftButtonDown(event, self);
}

-(void) mouseUp:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptMouseEventLeftButtonUp(event, self);
}

-(void) mouseDragged:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptMouseEventLeftButtonDrag(event, self);
}

-(void) rightMouseDown:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptMouseEventRightButtonDown(event, self);
}

-(void) rightMouseUp:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptMouseEventRightButtonUp(event, self);
}

-(void) rightMouseDragged:(NSEvent*) event
{
	_appRefPtr->getEOSEventManager()->interceptMouseEventRightButtonDrag(event, self);
}

@end
