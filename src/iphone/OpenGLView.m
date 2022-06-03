//
//  OpenGLView.m
//  eosTest
//
//  Created by Benjamin Lee on 2/1/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>

#import "OpenGLView.h"
#include "App.h"

@implementation OpenGLView

@synthesize context;
@synthesize width;
@synthesize height;
@synthesize renderBufferID;
@synthesize frameBufferID;
@synthesize useDepthBuffer;
@synthesize depthBufferID;
@synthesize ignoreLayoutSubviews;

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (id)initWithCoder:(NSCoder*)coder 
{
#ifdef _DEBUG
	NSLog(@"initWithCoder");
#endif /* _DEBUG */
	
	ignoreLayoutSubviews = false;	
	
    if ((self = [super initWithCoder:coder])) 
	{
		contextLock = [[NSLock alloc] init];
		
		_appRefPtr->initVideo(self);
		
#if 0	//	 FIX BY ADDING RETURN VALUE LATER
		if (_appRefPtr->initVideo(self) == EOSErrorNone)
		{
		}
		else
		{
			[self release];
			return nil;
		}
#endif
    }
	
	[self setUserInteractionEnabled:YES];
	
    return self;
}

- (id)initWithFrame:(CGRect)frame {

#ifdef _DEBUG
	NSLog(@"initWithFrame");
#endif /* _DEBUG */
	
	ignoreLayoutSubviews = false;
	
    if (self = [super initWithFrame:frame]) 
	{
        // Initialization code
		contextLock = [[NSLock alloc] init];
		
		_appRefPtr->initVideo(self);		

		[self setUserInteractionEnabled:YES];
		[self setMultipleTouchEnabled:YES];
	}
	
    return self;
}

-(EOSError) createGLBuffers
{
	EOSError	error = EOSErrorNone;
	
	glGenFramebuffersOES(1, &frameBufferID);
    glGenRenderbuffersOES(1, &renderBufferID);

	glBindFramebufferOES(GL_FRAMEBUFFER_OES, frameBufferID);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderBufferID);
	
    [context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:(CAEAGLLayer*)self.layer];
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, renderBufferID);
    
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_WIDTH_OES, &width);
    glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES, GL_RENDERBUFFER_HEIGHT_OES, &height);
    
#ifdef _DEBUG
	NSLog(@"Context Size %d %d\n", width, height);
#endif /* _DEBUG */
	
    if (useDepthBuffer) 
	{
        glGenRenderbuffersOES(1, &depthBufferID);
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthBufferID);
        glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH_COMPONENT16_OES, width, height);
        glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, depthBufferID);
    }
	
	return error;
}

-(void) destroyGLBuffers
{
    glDeleteFramebuffersOES(1, &frameBufferID);
    frameBufferID= 0;
    glDeleteRenderbuffersOES(1, &renderBufferID);
    renderBufferID = 0;
    
    if(depthBufferID) {
        glDeleteRenderbuffersOES(1, &depthBufferID);
        depthBufferID = 0;
    }
}

-(EOSError) createGLContext:(NSDictionary*) fmt useDepth:(Boolean) use
{
	EOSError	error = EOSErrorNone;
	CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;

	eaglLayer.opaque = YES;
	eaglLayer.drawableProperties = fmt;
	
	useDepthBuffer = use;
	
	context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
	
	return error;
}

- (void)drawRect:(CGRect)rect {
    // Drawing code
}

-(void) renderView
{
	[EAGLContext setCurrentContext:context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, frameBufferID);

	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	
	

	glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderBufferID);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];

}

-(void) beginFrame
{
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, frameBufferID);
}

-(void) endFrame
{
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, renderBufferID);
	[context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)layoutSubviews 
{
	if (ignoreLayoutSubviews == false)
	{
#ifdef _DEBUG
		NSLog(@"layoutSubviews");
#endif /* _DEBUG */
		
		[EAGLContext setCurrentContext:context];
		[self destroyGLBuffers];
		[self createGLBuffers];
		[self renderView];
		
		CGRect rect = [self frame];
		
#ifdef _DEBUG
		NSLog(@"setLandscape %f %f %f %f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
#endif /* _DEBUG */
	}
	else
	{
#ifdef _DEBUG
		NSLog(@"layoutSubviews ignored");
#endif /* _DEBUG */
	}
}

-(void) lock
{
	[contextLock lock];
	[EAGLContext setCurrentContext:context];
}

-(void) unlock
{
	[contextLock unlock];
}

-(Boolean) landscape
{
	return landscape;
}

-(void) setLandscape:(Boolean) l
{
	CGRect rect = [self frame];
	
#ifdef _DEBUG
	NSLog(@"setLandscape %f %f %f %f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
#endif /* _DEBUG */
	
	landscape = l;
	
	if (landscape)
	{
		[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationLandscapeRight animated:NO];

#ifdef _BASE_SDK_3_2_OR_HIGHER
		if (_appRefPtr->isOS32OrHigher())
			[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
		else
			[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
#else
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
#endif
		//	Rebuild the frame information
	}
	else
	{
		[[UIApplication sharedApplication] setStatusBarOrientation:UIInterfaceOrientationPortrait animated:NO];

#ifdef _BASE_SDK_3_2_OR_HIGHER
		if (_appRefPtr->isOS32OrHigher())
			[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];
		else
			[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
#else
		[[UIApplication sharedApplication] setStatusBarHidden:YES animated:NO];
#endif
		//	Rebuild the frame information
	}
}

-(void) enableMultiTouch:(Boolean) enable
{
	[self setMultipleTouchEnabled:enable];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	_appRefPtr->getEOSEventManager()->interceptTouchScreenEventDown(touches, event, self);
}


// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{  
	_appRefPtr->getEOSEventManager()->interceptTouchScreenEventMove(touches, event, self);
}

// Handles the end of a touch event when the touch is a tap.
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	_appRefPtr->getEOSEventManager()->interceptTouchScreenEventUp(touches, event, self);
}

// Handles the end of a touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
	_appRefPtr->getEOSEventManager()->interceptTouchScreenEventCancel(touches, event, self);
}

- (void)dealloc {
	
	[contextLock release];
	
	if([EAGLContext currentContext] == context)
	{
		[EAGLContext setCurrentContext:nil];
	}
	
	[context release];
	context = nil;
	
    [super dealloc];
}


@end
