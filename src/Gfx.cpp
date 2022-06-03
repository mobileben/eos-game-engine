/******************************************************************************
 *
 * File: Gfx.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Graphics interface for application to the platform's VideoWindow
 * 
 *****************************************************************************/

#include "Gfx.h"
#include "App.h"
#include "Renderer.h"

#ifdef _PLATFORM_IPHONE

#ifdef _USE_OPENGL

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/EAGLDrawable.h>

#endif /* _USE_OPENGL */

#endif /* _PLATFORM_IPHONE */

GfxVideoWindowBinder::GfxVideoWindowBinder()
{
	_videoWin = NULL;

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
	_eglContext = 0;
	_hdc = NULL;
#else
	ZeroMemory(&_pxlDesc, sizeof(PIXELFORMATDESCRIPTOR));
	_hdc = NULL;
	_hrc = NULL;
#endif /* _USE_PVR_OPENGLES */

#elif defined(_PLATFORM_MAC)
	
	_pxlFormat = nil;
	_context = nil;
	
#elif defined(_PLATFORM_IPHONE)
	
	_pxlFormat = nil;
	_glView = NULL;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

GfxVideoWindowBinder::~GfxVideoWindowBinder()
{
#ifdef _PLATFORM_PC

#elif defined(_PLATFORM_MAC)
	
	if (_pxlFormat != nil)
	{
		[_pxlFormat release];
		_pxlFormat = nil;
	}
	
#elif defined(_PLATFORM_IPHONE)
	
	_pxlFormat = nil;
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */
}

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
EGLConfig GfxVideoWindowBinder::buildEGLConfiguration(GfxPixelFormat& format)
{
	EGLint		num_config;
	EGLint		conflist[32];
	EGLConfig	conf;
	int			i;

	i = 0;

	/* Select default configuration */
	conflist[i++] = EGL_LEVEL;
	conflist[i++] = 0;

	conflist[i++] = EGL_NATIVE_RENDERABLE;
	conflist[i++] = EGL_FALSE;

	conflist[i++] = EGL_BUFFER_SIZE;
	conflist[i++] = format.colorBits + format.alphaBits;

	if(format.depthBits > 0)
	{
		conflist[i++] = EGL_DEPTH_SIZE;
		conflist[i++] = format.depthBits;
	}
	else
	{
		conflist[i++] = EGL_DEPTH_SIZE;
		conflist[i++] = EGL_DONT_CARE;
	}

	if(format.stencilBits > 0)
	{
		conflist[i++] = EGL_STENCIL_SIZE;
		conflist[i++] = format.stencilBits;
	}
	else
	{
		conflist[i++] = EGL_STENCIL_SIZE;
		conflist[i++] = EGL_DONT_CARE;
	}

	conflist[i++] = EGL_SURFACE_TYPE;
	conflist[i++] = EGL_WINDOW_BIT;

	// Terminate the list with EGL_NONE
	conflist[i++] = EGL_NONE;

	// Return null config if config is not found
	if(!eglChooseConfig(_eglDisplay, conflist, &conf, 1, &num_config) || num_config != 1)
	{
		return 0;
	}

	// Return config index
	return conf;

}
#else
void GfxVideoWindowBinder::buildPIXELFORMATDESCRIPTORFromGfxPixelFormat(PIXELFORMATDESCRIPTOR& desc, GfxPixelFormat& format)
{
	ZeroMemory(&desc, sizeof(PIXELFORMATDESCRIPTOR));

	desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	desc.nVersion = 1;
	desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;

	if (format.doubleBuffer)
		desc.dwFlags |= PFD_DOUBLEBUFFER;

	desc.iPixelType = PFD_TYPE_RGBA;

	desc.cColorBits = format.colorBits;

	desc.cAlphaBits = format.alphaBits;

	desc.cDepthBits = format.depthBits;

	desc.cAccumBits = format.accumBits;

	desc.cStencilBits = format.stencilBits;

	desc.iLayerType = PFD_MAIN_PLANE;
}
#endif /* _USE_PVR_OPENGLES */
#endif /* _PLATFORM_PC */

#ifdef _PLATFORM_MAC

#define MaxGLAttributes	64

NSOpenGLPixelFormat* GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat(GfxPixelFormat& format)
{
	Uint32					attrIndex = 0;
	GLuint					attributes[MaxGLAttributes];
	NSOpenGLPixelFormat*	glpxlformat = NULL;

	memset(attributes, 0, sizeof(GLuint) * MaxGLAttributes);
	
	attributes[attrIndex++] = NSOpenGLPFAWindow;
	AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");

	attributes[attrIndex++] = NSOpenGLPFAAccelerated;
	AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");

	if (format.doubleBuffer)
	{
		attributes[attrIndex++] = NSOpenGLPFADoubleBuffer;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	
	if (format.colorBits > 0)
	{
		attributes[attrIndex++] = NSOpenGLPFAColorSize;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = format.colorBits;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	
	if (format.alphaBits > 0)
	{
		attributes[attrIndex++] = NSOpenGLPFAAlphaSize;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = format.alphaBits;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
		
	if (format.depthBits > 0)
	{
		attributes[attrIndex++] = NSOpenGLPFADepthSize;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = format.depthBits;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	
	if (format.accumBits > 0)
	{
		attributes[attrIndex++] = NSOpenGLPFAAccumSize;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = format.accumBits;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	
	if (format.stencilBits > 0)
	{
		attributes[attrIndex++] = NSOpenGLPFAStencilSize;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = format.stencilBits;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	
	if (format.auxBuffers > 0)
	{
		attributes[attrIndex++] = NSOpenGLPFAAuxBuffers;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = format.auxBuffers;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildNSOpenGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	
	glpxlformat = [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*) attributes];
	
	return glpxlformat;
}

#endif /* _PLATFORM_MAC */

#ifdef _PLATFORM_IPHONE

#define MaxGLAttributes	5

NSDictionary* GfxVideoWindowBinder::buildGLPixelFormatFromGfxPixelFormat(GfxPixelFormat& format)
{
	Uint32			attrIndex = 0;
	Uint32			i;
	id				attributes[MaxGLAttributes];
	NSDictionary*	glpxlformat = NULL;
	
	for (i=0;i<MaxGLAttributes;i++)
		attributes[i] = nil;

	attributes[attrIndex++] = [NSNumber numberWithBool:NO];
	AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	attributes[attrIndex++] = kEAGLDrawablePropertyRetainedBacking;
	AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	
	if (format.colorBits > 16)
	{
		//	This is considered RGBA8
		attributes[attrIndex++] = kEAGLColorFormatRGBA8;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = kEAGLDrawablePropertyColorFormat;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	else
	{
		//	This is considered RGB565
		attributes[attrIndex++] = kEAGLColorFormatRGB565;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildGLPixelFormatFromGfxPixelFormat() index out of bounds.");
		attributes[attrIndex++] = kEAGLDrawablePropertyColorFormat;
		AssertWDesc(attrIndex < MaxGLAttributes, "GfxVideoWindowBinder::buildGLPixelFormatFromGfxPixelFormat() index out of bounds.");
	}
	
	glpxlformat = [NSDictionary dictionaryWithObjectsAndKeys:attributes[0], attributes[1], attributes[2], attributes[3], nil];
	
	return glpxlformat;
}

#endif /* _PLATFORM_IPHONE */

EOSError GfxVideoWindowBinder::bindToVideoWindow(VideoWindow* win, GfxVideoWindowBinderInfo& info)
{
	EOSError	error = EOSErrorNone;

	AssertWDesc(win != NULL, "NULL VideoWindow.");

	if (win)
	{
#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
		_hdc = GetDC(win->getHWND());

		_eglDisplay = eglGetDisplay((NativeDisplayType) _hdc);

		if(eglInitialize(_eglDisplay, &_eglMajorVersion, &_eglMinorVersion))
		{
			if(eglBindAPI(EGL_OPENGL_ES_API))
			{
				_eglConfig = buildEGLConfiguration(info.pxlFormat);

				if (_eglContext)
					eglDestroyContext(_eglDisplay, _eglContext);

				_eglContext = eglCreateContext(_eglDisplay, _eglConfig, NULL, NULL);

				if(_eglContext == EGL_NO_CONTEXT)
					error = EOSErrorResourceCreationFailure;
				else
				{
					EGLint		attrib_list[1];
					NativeWindowType nwt = (NativeWindowType) win->getHWND();

					attrib_list[0] = EGL_NONE;

					_eglWindow = eglCreateWindowSurface(_eglDisplay, _eglConfig, nwt, NULL);

					// If we have failed to create a surface then try using Null
					if(_eglWindow == EGL_NO_SURFACE)
					{
						_eglWindow = eglCreateWindowSurface(_eglDisplay, _eglConfig, NULL, NULL);
					}

					if(_eglWindow != EGL_NO_SURFACE)
					{
						if (eglMakeCurrent(_eglDisplay, _eglWindow, _eglWindow, _eglContext))
						{
							_videoWin = win;
						}
						else
							error = EOSErrorResourceCreationFailure;
					}
					else
						error = EOSErrorResourceCreationFailure;
				}
			}
			else
				error = EOSErrorResourceCreationFailure;
		}
		else
			error = EOSErrorResourceCreationFailure;

#else
		HWND	hwnd;
		Sint32	format;

		AssertWDesc(win->getHWND() != NULL, "NULL HWND");

		hwnd = win->getHWND();

		if (hwnd)
		{
			buildPIXELFORMATDESCRIPTORFromGfxPixelFormat(_pxlDesc, info.pxlFormat);

			_hdc = GetDC(hwnd);

			format = ChoosePixelFormat(_hdc, &_pxlDesc);

			AssertWDesc(format != 0, "ChoosePixelFormat failed.");

			if (format != 0 && SetPixelFormat(_hdc, format, &_pxlDesc))
			{
				_hrc = wglCreateContext(_hdc);
				wglMakeCurrent(_hdc, _hrc);

				_videoWin = win;
			}
			else
				error = EOSErrorResourceCreationFailure;
		}
		else
			error = EOSErrorNULL;
#endif /* _USE_PVR_OPENGLES */

#elif defined(_PLATFORM_MAC)
		
		_pxlFormat = buildNSOpenGLPixelFormatFromGfxPixelFormat(info.pxlFormat);
		
		if (_pxlFormat != nil)
		{
			//	Build the context
			_context = [[NSOpenGLContext alloc] initWithFormat:_pxlFormat shareContext:nil];
			
			if (_context != nil)
			{
				[win->getOpenGLView() setNSOpenGLContext:_context];
				[win->getOpenGLView() setNSOpenGLPixelFormat:_pxlFormat];
				
				_videoWin = win;
			}
			else
				error = EOSErrorResourceCreationFailure;
		}
		else
			error = EOSErrorResourceCreationFailure;
		
#elif defined(_PLATFORM_IPHONE)
		
		_pxlFormat = buildGLPixelFormatFromGfxPixelFormat(info.pxlFormat);
		
		if (_pxlFormat != nil)
		{
			OpenGLView* view = win->getOpenGLView();
			error = [view createGLContext:_pxlFormat useDepth:info.pxlFormat.depthBits > 0];
			
			if (error == EOSErrorNone)
			{
				_videoWin = win;
				_glView = win->getOpenGLView();
			}
		}
		else
			error = EOSErrorResourceCreationFailure;
#else
	
		#error _PLATFORM not defined.
	
#endif /* _PLATFORM_PC */
	}
	else
		error = EOSErrorNULL;

	return error;
}

EOSError GfxVideoWindowBinder::unbindFromVideoWindow(VideoWindow& win)
{
	EOSError	error = EOSErrorNone;

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
	eglSwapBuffers(_eglDisplay, _eglWindow);
	eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(_eglDisplay, _eglContext);
	eglDestroySurface(_eglDisplay, _eglWindow);
	eglTerminate(_eglDisplay);
	_hdc = NULL;
#else
	if (_hrc)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(_hrc);
		ReleaseDC(_videoWin->getHWND(), _hdc);

		_hdc = NULL;
		_hrc = NULL;
	}
#endif /* _USE_PVR_OPENGLES */
	
#elif defined(_PLATFORM_MAC)
	
	if (_context != nil)
	{
		CGLLockContext((CGLContextObj)[_context CGLContextObj]);
		
		[_context release];
		_context = nil;
	}
	
	if (_pxlFormat != nil)
	{
		[_pxlFormat release];
		_pxlFormat = nil;
	}
	
#elif defined(_PLATFORM_IPHONE)
	
	
#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

	return error;
}

Gfx::Gfx()
{
	_frameCount = 0;
	_fps = 0;
	_frameProcessTime = 0;
	_swapBufferProcessTime = 0;

	_renderer = NULL;
	
	_inFrame = false;
	
	_clearColorDirty = true;
	_clearColor.r = _clearColor.g = _clearColor.b = 0.0F;
	_clearColor.a = 1.0F;

	_clearDepthDirty = true;	
	_clearDepth = 0.0F;
	
#ifndef _PLATFORM_IPHONE
	
	_clearStencilDirty = true;
	_clearStencil = 0.0F;
	
	_clearAccumDirty = true;
	_clearAccum.r = _clearAccum.g = _clearAccum.b = 0.0F;
	_clearAccum.a = 1.0F;

#endif /* _PLATFORM_IPHONE */

	_isLandscape = false;
	
	_viewWidth = 0;
	_viewHeight = 0;

	_displayFPS = false;
	_fpsDisplayPos.x =  DefaultFPS_X;
	_fpsDisplayPos.y =  DefaultFPS_Y;

	_displayRendererStats = false;
	_rendererStatsDisplayPos.x =  DefaultRendererStats_X;
	_rendererStatsDisplayPos.y =  DefaultRendererStats_Y;

	_displayRendererStats = false;

	_screenRotation = 0.0;
}

Gfx::~Gfx()
{
}

EOSError Gfx::bindToVideoWindow(VideoWindow* win, GfxVideoWindowBinderInfo& info)
{
	EOSError error;

	error = getVideoWindowBinder()->bindToVideoWindow(win, info);

	if (_isLandscape)
	{
		_viewWidth = _gfxVideoWinBinder.getVideoWindow()->getHeight();
		_viewHeight = _gfxVideoWinBinder.getVideoWindow()->getWidth();
	}
	else
	{
		_viewWidth = _gfxVideoWinBinder.getVideoWindow()->getWidth();
		_viewHeight = _gfxVideoWinBinder.getVideoWindow()->getHeight();
	}

	if (_renderer)
		_renderer->setLandscape(_isLandscape);
	
	return error;
}

void Gfx::bindRenderer(Renderer* r)
{
	_renderer = r;
	
	_renderer->setLandscape(_isLandscape);
}

void Gfx::setScreenRotation(Float32 ang)
{
	_screenRotation = ang;
}

Boolean Gfx::isInFrame(void)
{
	return _inFrame;
}

EOSError Gfx::beginFrame(void)
{
	EOSError	error = EOSErrorNone;

#ifdef _USE_OPENGL
	lockGLContext();
#endif /* _USE_OPENGL */

#ifdef _PLATFORM_IPHONE
	[getVideoWindowBinder()->getOpenGLView() beginFrame];
#endif /* _PLATFORM_IPHONE */
	
	_inFrame = true;
	_frameCount++;

	if (_fpsTimer.getTimerState() != PerformanceTimer::TimerStateRunning)
		_fpsTimer.start();
	else
	{
		//	Compute frame rate
		MicroSeconds time = _fpsTimer.getTotalRunningTime();

		if (time != 0)
		{
			_fps = (Float32) (((double) _frameCount * 1000000.0) / (double) time);
		}
		else
			_fps = 0.0F;

		_fpsTimer.lap();
	}

	error = _renderer->beginFrame();

	return error;
}

EOSError Gfx::endFrame(void)
{
	EOSError	error = EOSErrorNone;

	_frameProcessTime = _fpsTimer.getElapsedTime();

	_renderer->endFrame();

	if (_displayFPS)
	{
		char str[16];

		sprintf(str, "FPS: %.2f", _fps);
		_app->drawDebugString((Uint32) _fpsDisplayPos.x, (Uint32) _fpsDisplayPos.y, str);
	}

	if (_displayRendererStats)
	{
		char 			str[16];
		Uint32			dy = 0;
		RendererStats*	stats = _renderer->getRendererStats();

		sprintf(str, "VERTS: %d", stats->vertices);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "POLYS: %d", stats->polys);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "TEXPOLYS: %d", stats->texPolys);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "TEXENV: %d", stats->texEnvChanges);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "TEX: %d", stats->texChanges);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "ALPHA: %d", stats->alphaBlendChanges);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "COLORS: %d", stats->colorChanges);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "ROTATES: %d/%d", stats->rotates, stats->ignoredRotates);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "SCALES: %d/%d", stats->scales, stats->ignoredScales);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "REUSE UV: %d", stats->reusedTexArray);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;

		sprintf(str, "REUSE VERT: %d", stats->reusedVertArray);
		_app->drawDebugString((Sint32) _rendererStatsDisplayPos.x, (Sint32) _rendererStatsDisplayPos.y + dy, str);
		dy += 10;
	}

	_inFrame = false;

#ifdef _PLATFORM_IPHONE
	[getVideoWindowBinder()->getOpenGLView() endFrame];
#endif /* _PLATFORM_IPHONE */
	
#ifdef _USE_OPENGL
	unlockGLContext();
#endif /* _USE_OPENGL */
	
	return error;
}

EOSError Gfx::swapBuffer(void)
{
	EOSError	error = EOSErrorNone;

	_fpsTimer.lap();

#ifdef _PLATFORM_PC

#ifdef _USE_PVR_OPENGLES
	eglSwapBuffers (getVideoWindowBinder()->getEGLDisplay(), getVideoWindowBinder()->getEGLSurface());

#else
	SwapBuffers(getVideoWindowBinder()->getHDC());
#endif /* _USE_PVR_OPENGLES */

#elif defined(_PLATFORM_MAC)

	[getVideoWindowBinder()->getOpenGLContext() flushBuffer];

#elif defined(_PLATFORM_IPHONE)
	
#endif

	_swapBufferProcessTime = _fpsTimer.getElapsedTime();

	return error;
}

void Gfx::clear(Uint32 mask)
{
#ifdef _USE_OPENGL	
	GLbitfield	glMask = 0;

	//	If our clear values are dirty, set them now
	if (_clearColorDirty)
	{
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
		_clearColorDirty = false;
	}
	
	if (_clearDepthDirty)
	{
#ifdef _USE_OPENGL_ES_1_1

#ifdef _USE_PVR_OPENGLES
		glClearDepthx((GLclampx)_clearDepth);
#else
		glClearDepthf(_clearDepth);
#endif /* _USE_PVR_OPENGLES */
#else

		glClearDepth(_clearDepth);

#endif /* _USE_OPENGL_ES_1_1 */
		
		_clearDepthDirty = false;
	}
	
#ifndef _PLATFORM_IPHONE
	
	if (_clearStencilDirty)
	{
#ifdef _USE_PVR_OPENGLES
		glClearStencil((GLint) _clearStencil);
#else
		glClearStencil(_clearStencil);
#endif
		_clearStencilDirty = false;
	}

	if (_clearAccumDirty)
	{
#ifndef _USE_OPENGL_ES_1_1
		glClearAccum(_clearAccum.r, _clearAccum.g, _clearAccum.b, _clearAccum.a);
#endif /* _USE_OPENGL_ES_1_1 */
		_clearAccumDirty = false;
	}
	
#endif /* _PLATFORM_IPHONE */
	
	if (mask & ClearMaskColorBuffer)
		glMask |= GL_COLOR_BUFFER_BIT;

	if (mask & ClearMaskDepthBuffer)
		glMask |= GL_DEPTH_BUFFER_BIT;

#ifndef _PLATFORM_IPHONE
	
	if (mask & ClearMaskStencilBuffer)
		glMask |= GL_STENCIL_BUFFER_BIT;

#ifndef _USE_OPENGL_ES_1_1
	if (mask & ClearMaskAccumBuffer)
		glMask |= GL_ACCUM_BUFFER_BIT;
#endif /* _USE_OPENGL_ES_1_1 */
#endif /* _PLATFORM_IPHONE */
	
	glClear(glMask);

#endif /* _USE_OPENGL */
}

void Gfx::setClearColor(ColorRGBA& color)
{
	_clearColor.r = color.r;
	_clearColor.g = color.g;
	_clearColor.b = color.b;
	_clearColor.a = color.a;

#ifdef _USE_OPENGL
	if (_inFrame)
	{
		glClearColor(_clearColor.r, _clearColor.g, _clearColor.b, _clearColor.a);
		_clearColorDirty = false;
	}
	else
		_clearColorDirty = true;
#endif /* _USE_OPENGL */
}

void Gfx::setClearDepth(Float32 value)
{
	_clearDepth = value;

#ifdef _USE_OPENGL
	if (_inFrame)
	{
#ifdef _USE_OPENGL_ES_1_1
		
		glClearDepthf(_clearDepth);
		
#else
		
		glClearDepth(_clearDepth);
		
#endif /* _USE_OPENGL_ES_1_1 */
		
		_clearDepthDirty = false;
	}
	else
		_clearDepthDirty = true;
#endif /* _USE_OPENGL */
}

#ifndef _PLATFORM_IPHONE

void Gfx::setClearStencil(Float32 value)
{
	_clearStencil = value;

#ifdef _USE_OPENGL
	if (_inFrame)
	{
#ifdef _USE_PVR_OPENGLES
		glClearStencil((GLint) _clearStencil);
#else
		glClearStencil(_clearStencil);
#endif
		_clearStencilDirty = false;
	}
	else
		_clearStencilDirty = true;
#endif /* _USE_OPENGL */
}

void Gfx::setClearAccum(ColorRGBA& accum)
{
	_clearAccum.r = accum.r;
	_clearAccum.g = accum.g;
	_clearAccum.b = accum.b;
	_clearAccum.a = accum.a;

#ifdef _USE_OPENGL
	if (_inFrame)
	{
#ifndef _USE_OPENGL_ES_1_1
		glClearAccum(_clearAccum.r, _clearAccum.g, _clearAccum.b, _clearAccum.a);
#endif /* _USE_OPENGL_ES_1_1 */
		_clearAccumDirty = false;
	}
	else
		_clearAccumDirty = true;
#endif /* _USE_OPENGL */
}

#endif /* _PLATFORM_IPHONE */

void Gfx::getClearColor(ColorRGBA& color)
{
	color.r = _clearColor.r;
	color.g = _clearColor.g;
	color.b = _clearColor.b;
	color.a = _clearColor.a;
}

Float32	Gfx::getClearDepth(void)
{
	return _clearDepth;
}

#ifndef _PLATFORM_IPHONE

Float32	Gfx::getClearStencil(void)
{
	return _clearStencil;
}

void	Gfx::getClearAccum(ColorRGBA& accum)
{
	accum.r = _clearAccum.r;
	accum.g = _clearAccum.g;
	accum.b = _clearAccum.b;
	accum.a = _clearAccum.a;
}

#endif /* _PLATFORM_IPHONE */

#ifdef _USE_OPENGL
void Gfx::lockGLContext(void)
{
#ifdef _PLATFORM_MAC
	CGLContextObj	ctx = (CGLContextObj) [getVideoWindowBinder()->getOpenGLContext() CGLContextObj];
	
	[getVideoWindowBinder()->getOpenGLContext() makeCurrentContext];
	CGLLockContext(ctx);
	
#elif defined(_PLATFORM_IPHONE)

	[getVideoWindowBinder()->getOpenGLView() lock];

#endif /* _PLATFORM_MAC */
}

void Gfx::unlockGLContext(void)
{
#ifdef _PLATFORM_MAC
	CGLContextObj	ctx = (CGLContextObj) [getVideoWindowBinder()->getOpenGLContext() CGLContextObj];

	[getVideoWindowBinder()->getOpenGLContext() makeCurrentContext];
	CGLUnlockContext(ctx);
	
#elif defined(_PLATFORM_IPHONE)

	[getVideoWindowBinder()->getOpenGLView() unlock];

#endif /* _PLATFORM_MAC */
}

#endif /* _USE_OPENGL */

void Gfx::setLandscape(Boolean landscape)
{
	_isLandscape = landscape;
	
	if (_isLandscape)
	{
		_viewWidth = _gfxVideoWinBinder.getVideoWindow()->getHeight();
		_viewHeight = _gfxVideoWinBinder.getVideoWindow()->getWidth();
	}
	else
	{
		_viewWidth = _gfxVideoWinBinder.getVideoWindow()->getWidth();
		_viewHeight = _gfxVideoWinBinder.getVideoWindow()->getHeight();
	}	
	
	if (_renderer)
		_renderer->setLandscape(landscape);

#ifdef _PLATFORM_IPHONE
	[getVideoWindowBinder()->getOpenGLView() setLandscape:landscape];
#endif /* _PLATFORM_IPHONE */
}

void Gfx::displayFPS(Boolean display)
{
	_displayFPS = display;
}

void Gfx::setFPSDisplayPosition(Point2D& pt)
{
	_fpsDisplayPos = pt;
}

void Gfx::displayRendererStats(Boolean display)
{
	_displayRendererStats = display;
}

void Gfx::setRendererStatsDisplayPosition(Point2D& pt)
{
	_rendererStatsDisplayPos = pt;
}

