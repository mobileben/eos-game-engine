//
//  OpenGLView.h
//  eosTest
//
//  Created by Benjamin Lee on 2/1/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#ifndef __OPENGLVIEW_H__
#define __OPENGLVIEW_H__

#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#include "EOSError.h"

@interface OpenGLView : UIView {
	NSLock*			contextLock;
	
    /* The pixel dimensions of the backbuffer */
	EAGLContext*	context;
	
	GLint			width;
	GLint			height;
	GLuint			renderBufferID;
	GLuint			frameBufferID;
	
	Boolean			useDepthBuffer;
	GLuint			depthBufferID;
	
	Boolean			ignoreLayoutSubviews;
	
	Boolean			landscape;
}

-(EOSError) createGLBuffers;
-(void) destroyGLBuffers;
-(EOSError) createGLContext:(NSDictionary*) fmt useDepth: (Boolean) use;
-(void) renderView;
-(void) lock;
-(void) unlock;

-(void) beginFrame;
-(void) endFrame;

-(void) setLandscape:(Boolean) l;

-(void) enableMultiTouch:(Boolean) enable;

@property (nonatomic, retain) EAGLContext*			context;
@property (nonatomic, readwrite, assign) GLint		width;
@property (nonatomic, readwrite, assign) GLint		height;
@property (nonatomic, readwrite, assign) GLuint		renderBufferID;
@property (nonatomic, readwrite, assign) GLuint		frameBufferID;
@property (nonatomic, readwrite, assign) Boolean	useDepthBuffer;
@property (nonatomic, readwrite, assign) GLuint		depthBufferID;
@property (nonatomic, readwrite, assign) Boolean	landscape;
@property (nonatomic, readwrite, assign) Boolean	ignoreLayoutSubviews;

@end

#endif /* __OPENGLVIEW_H__ */
