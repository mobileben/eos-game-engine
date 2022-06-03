/******************************************************************************
 *
 * File: Gfx.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Graphics interface for application to the platform's VideoWindow
 * 
 *****************************************************************************/

#ifndef __GFX_H__
#define __GFX_H__

#ifdef _PLATFORM_MAC
#ifdef _USE_OPENGL
#import <OpenGL/OpenGL.h>
#endif /* _USE_OPENGL */
#endif /* _PLATFORM_MAC */

#include "Platform.h"
#include "VideoWindow.h"
#include "Graphics.h"
#include "PerformanceTimer.h"

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
#ifndef _USE_OPENGL_ES_1_1
#define _USE_OPENGL_ES_1_1
#endif /* _USE_OPENGL_ES_1_1 */

#ifndef _EOS_TOOL
#include "GLES/gl.h"
#include "GLES/egl.h"
#include "GLES/glext.h"

#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG			0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG			0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG			0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG			0x8C03
#endif

#else

#ifndef _EOS_TOOL
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
#include <gl/glut.h>
#include <gl/glext.h>
#endif

#endif /* _USE_PVR_OPENGLES */

#elif defined(_PLATFORM_MAC)

#elif defined(_PLATFORM_IPHONE)

#include "OpenGLView.h"

#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

typedef struct
{
	Boolean	doubleBuffer;
	Uint8	colorBits;
	Uint8	alphaBits;
	Uint8	depthBits;
	
#ifndef _PLATFORM_IPHONE
	
	Uint8	accumBits;
	Uint8	stencilBits;
	Uint8	auxBuffers;
	
#endif /* _PLATFORM_IPHONE */
} GfxPixelFormat;

typedef struct
{
	GfxPixelFormat			pxlFormat;
	
#ifdef _PLATFORM_PC

#elif defined(_PLATFORM_MAC)
	
#elif defined(_PLATFORM_IPHONE)
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

} GfxVideoWindowBinderInfo;

class GfxVideoWindowBinder : public EOSObject
{
private:
	VideoWindow*			_videoWin;

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
	EGLDisplay	_eglDisplay;
	EGLSurface	_eglWindow;
	EGLContext	_eglContext;
	EGLConfig	_eglConfig;
	EGLint		_eglMajorVersion;
	EGLint		_eglMinorVersion;
	HDC			_hdc;

#else
	PIXELFORMATDESCRIPTOR	_pxlDesc;	//	For now, keep a copy in case we need to reference this later on
	HDC						_hdc;
	HGLRC					_hrc;
#endif /* _USE_PVR_OPENGLES */

#elif defined(_PLATFORM_MAC)
	
	NSOpenGLPixelFormat*	_pxlFormat;
	NSOpenGLContext*		_context;
	
#elif defined(_PLATFORM_IPHONE)
	
	NSDictionary*			_pxlFormat;
	OpenGLView*				_glView;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
	EGLConfig 	buildEGLConfiguration(GfxPixelFormat& format);
#else
	void	buildPIXELFORMATDESCRIPTORFromGfxPixelFormat(PIXELFORMATDESCRIPTOR& desc, GfxPixelFormat& format);
#endif /* _USE_PVR_OPENGLES */

#endif /* _PLATFORM_PC */

#ifdef _PLATFORM_MAC
	NSOpenGLPixelFormat*	buildNSOpenGLPixelFormatFromGfxPixelFormat(GfxPixelFormat& format);
#endif /* _PLATFORM_MAC */
	
#ifdef _PLATFORM_IPHONE
	NSDictionary*			buildGLPixelFormatFromGfxPixelFormat(GfxPixelFormat& format);
#endif /* _PLATFORM_IPHONE */
	
public:
	GfxVideoWindowBinder();
	~GfxVideoWindowBinder();

	EOSError		bindToVideoWindow(VideoWindow* win, GfxVideoWindowBinderInfo& info);
	EOSError		unbindFromVideoWindow(VideoWindow& win);

	inline VideoWindow*	getVideoWindow(void) const { return _videoWin; }

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
	EGLDisplay		getEGLDisplay(void) { return _eglDisplay; }
	EGLSurface		getEGLSurface(void) { return _eglWindow; }
#else
	inline HDC		getHDC(void) const { return _hdc; }
	inline HGLRC	getHRC(void) const { return _hrc; }
#endif /* _USE_PVR_OPENGLES */

#elif defined(_PLATFORM_MAC)	

	inline NSOpenGLPixelFormat*	getNSOpenGLPixelFormat(void) const { return _pxlFormat; }
	inline NSOpenGLContext*		getOpenGLContext(void) const { return _context; }
	
#elif defined(_PLATFORM_IPHONE)
	
	inline OpenGLView*		getOpenGLView(void) const { return _glView; }
	
#endif /* _PLATFORM_PC*/

};

class Renderer;

class Gfx : public EOSFrameworkComponent
{
public:
	typedef enum
	{
		ClearMaskColorBuffer = 1,
		ClearMaskDepthBuffer = 2,
		
#ifndef _PLATFORM_IPHONE
		
		ClearMaskStencilBuffer = 4,
		ClearMaskAccumBuffer = 8,
		
#endif /* _PLATFORM_IPHONE */
	} ClearMask;

	static const int DefaultFPS_X = 8;
	static const int DefaultFPS_Y = 8;
	static const int DefaultRendererStats_X = 8;
	static const int DefaultRendererStats_Y = 18;

private:
	GfxVideoWindowBinder	_gfxVideoWinBinder;

	Boolean					_isLandscape;

	Uint32					_viewWidth;
	Uint32					_viewHeight;

	Boolean					_displayFPS;
	Point2D					_fpsDisplayPos;

	Boolean					_displayRendererStats;
	Point2D					_rendererStatsDisplayPos;

	PerformanceTimer		_fpsTimer; 
	Uint32					_frameCount;
	Float32					_fps;
	MicroSeconds			_frameProcessTime;
	MicroSeconds			_swapBufferProcessTime;

	Boolean					_inFrame;
	
	Boolean					_clearColorDirty;
	ColorRGBA				_clearColor;
	
	Boolean					_clearDepthDirty;
	Float32					_clearDepth;
	
#ifndef _PLATFORM_IPHONE
	
	Boolean					_clearStencilDirty;
	Float32					_clearStencil;
	
	Boolean					_clearAccumDirty;
	ColorRGBA				_clearAccum;
	
#endif /* _PLATFORM_IPHONE */

	Renderer*				_renderer;

	Float32					_screenRotation;

public:
	Gfx();
	~Gfx();

	EOSError						bindToVideoWindow(VideoWindow* win, GfxVideoWindowBinderInfo& info);
	inline GfxVideoWindowBinder*	getVideoWindowBinder(void) { return &_gfxVideoWinBinder; }

	inline Uint32					getWidth(void) { return _viewWidth; }
	inline Uint32					getHeight(void) { return _viewHeight; }

#ifdef _USE_OPENGL	
	void							lockGLContext(void);
	void							unlockGLContext(void);
#endif /* _USE_OPENGL */

	void							bindRenderer(Renderer* r);
	inline Renderer*				getBoundRenderer(void) { return _renderer; }

	inline Uint32					getFrameCount(void) { return _frameCount; }
	inline Float32					getFPS(void) { return _fps; }
	inline MicroSeconds				getFrameProcessTime(void) { return _frameProcessTime; }
	inline MicroSeconds				getSwapBufferProcessTime(void) { return _swapBufferProcessTime; }

	void							setScreenRotation(Float32 ang);
	inline Float32					getScreenRotation(void) { return _screenRotation; }

	Boolean							isInFrame(void);

	EOSError						beginFrame(void);

	void							clear(Uint32 mask);

	EOSError						endFrame(void);
	EOSError						swapBuffer(void);

	void 							setClearColor(ColorRGBA& color);
	void							setClearDepth(Float32 value);
	
#ifndef _PLATFORM_IPHONE
	
	void							setClearStencil(Float32 value);
	void							setClearAccum(ColorRGBA& accum);

#endif /* _PLATFORM_IPHONE */
	
	void							getClearColor(ColorRGBA& color);
	Float32							getClearDepth(void);
	
#ifndef _PLATFORM_IPHONE
	
	Float32							getClearStencil(void);
	void							getClearAccum(ColorRGBA& accum);
	
#endif /* _PLATFORM_IPHONE */

	inline Boolean					isLandscape(void) { return _isLandscape; }
	void							setLandscape(Boolean landscape); 

	void 							displayFPS(Boolean display);
	void							setFPSDisplayPosition(Point2D& pt);

	void 							displayRendererStats(Boolean display);
	void							setRendererStatsDisplayPosition(Point2D& pt);
};

#endif /* __GFX_H__ */

