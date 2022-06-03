//
//  OpenGLView.h
//  eosTest
//
//  Created by Benjamin Lee on 1/26/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include <OpenGL/OpenGL.h>

@interface OpenGLView : NSView {
	@private
	NSOpenGLContext*		_context;
	NSOpenGLPixelFormat*	_pxlFmt;
}

-(void) setNSOpenGLContext: (NSOpenGLContext*) ctx;
-(void) setNSOpenGLPixelFormat: (NSOpenGLPixelFormat*) fmt;

-(NSOpenGLContext*) openGLContext;
-(void) prepareOpenGL;

@end
