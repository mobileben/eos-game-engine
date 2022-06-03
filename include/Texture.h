/******************************************************************************
 *
 * File: Texture.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Texture object
 * 
 *****************************************************************************/

#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Platform.h"
#include "EOSError.h"

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

typedef struct
{
	Uint32 headerSize;
	Uint32 height;
	Uint32 width;
	Uint32 mipMapCount;
	Uint32 pxFlags;
	Uint32 texSize;
	Uint32 bpp;	
	Uint32 redMask;
	Uint32 greenMask;
	Uint32 blueMask;
	Uint32 alphaMask;
	Uint32 pvrMagic;
	Uint32 numSurfaces;
} PVRTHeader;

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

#ifdef _USE_OPENGL

#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

#endif /* _USE_OPENGL */

#else

	#error _PLATFORM not defined.

#endif /* _PLATFORM_PC */

typedef Uint32	TextureID;

#ifdef _USE_OPENGL

typedef GLuint	HWTextureID;

#else

typedef Uint32	HWTextureID;

#endif /* _USE_OPENGL */

const Uint32		InvalidTextureID = 0xFFFFFFFF;
const HWTextureID	InvalidHWTextureID = 0xFFFFFFFF;

typedef enum
{
	TextureFormatTGA = 0,
	TextureFormatGL_RGB_888,
	TextureFormatGL_RGBA_8888,
	TextureFormatGL_RGB_565,
	TextureFormatGL_RGBA_5551,
	TextureFormatGL_RGBA_4444,
	TextureFormatGL_P8_RGBA_8888,
	TextureFormatGL_P4_RGBA_4444,
	TextureFormatPVRTC_4BPP_RGBA,
	TextureFormatPVRTC_4BPP_RGB,
	TextureFormatPVRTC_2BPP_RGBA,
	TextureFormatPVRTC_2BPP_RGB,
	TextureFormatRAWPVRTC_4BPP_RGBA,
	TextureFormatRAWPVRTC_4BPP_RGB,
	TextureFormatRAWPVRTC_2BPP_RGBA,
	TextureFormatRAWPVRTC_2BPP_RGB,
	TextureFormatLast,
	TextureFormatIllegal = TextureFormatLast,
} TextureFormat;

#define TEXTURE_STATE_ENV_REPLACE							0x00000001
#define TEXTURE_STATE_ENV_MODULATE							0x00000002
#define TEXTURE_STATE_ENV_DECAL								0x00000004
#define TEXTURE_STATE_ENV_BLEND								0x00000008
#define TEXTURE_STATE_ENV_ADD								0x00000010
#define TEXTURE_STATE_ENV_COMBINE							0x00000020

#define TEXTURE_STATE_WRAP_S_CLAMP							0x00000100
#define TEXTURE_STATE_WRAP_S_REPEAT							0x00000200
#define TEXTURE_STATE_WRAP_S_CLAMP_BORDER					0x00000400
#define TEXTURE_STATE_WRAP_S_CLAMP_EDGE						0x00000800

#define TEXTURE_STATE_WRAP_T_CLAMP							0x00001000
#define TEXTURE_STATE_WRAP_T_REPEAT							0x00002000
#define TEXTURE_STATE_WRAP_T_CLAMP_BORDER					0x00004000
#define TEXTURE_STATE_WRAP_T_CLAMP_EDGE						0x00008000

#define TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST				0x00010000
#define TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR				0x00020000
#define TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_NEAREST		0x00040000
#define TEXTURE_STATE_MIN_MIPMAP_FILTER_NEAREST_LINEAR		0x00080000
#define TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_NEAREST		0x00100000	
#define TEXTURE_STATE_MIN_MIPMAP_FILTER_LINEAR_LINEAR		0x00200000

#define TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST				0x00400000
#define TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR				0x00800000
#define TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_NEAREST		0x01000000
#define TEXTURE_STATE_MAG_MIPMAP_FILTER_NEAREST_LINEAR		0x02000000
#define TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_NEAREST		0x04000000
#define TEXTURE_STATE_MAG_MIPMAP_FILTER_LINEAR_LINEAR		0x08000000

typedef struct
{
	Uint32	refID;
	Uint32	nameOffset;
	Uint32	filenameOffset;
	Uint32	width;
	Uint32	height;
	Uint32	colorFormat;
	Uint32	texState;
	Uint32	texBlend;
} TexInfoExport;

typedef TexInfoExport TexInfoRAW;

typedef struct
{
	ObjectID	refID;
	Char*		refName;
	Char*		filename;
	Uint32		width;
	Uint32		height;
	Uint32		colorFormat;
	Uint32		state;
	Uint32		blending;
} TextureInfo;

typedef struct
{
	Uint32		endian;
	Uint32		version;
	Uint32		nameOffset;
	Uint32		numTexInfo;
	Uint32		texInfo;
	Uint32		names;
} TexInfoDBHeader;

class TextureState;

class Texture : public EOSObject
{
private:
	friend class TextureState;

	Char*			_name;

	ObjectID		_refID;
	HWTextureID		_hwID;

	Boolean			_used;			//	Used by texture pools
	Boolean			_instantiated;	//	Since we support async access, a texture can be used, but not instantiated
	Boolean			_freeable;

	Uint8*			_buffer;

	Uint32			_width;
	Uint32			_height;

	Uint32			_numPalColors;

#ifdef _USE_OPENGL
	GLenum			_glTarget;
	GLenum			_glFormat;
	GLenum			_glType;
#endif /* _USE_OPENGL */

	Uint32			_memUsage;

	TextureState*	_boundTextureState;

	void			_init(void);

public:
	Texture();
	~Texture();

	inline ObjectID		getRefID(void) { return _refID; }
	inline Char*		getName(void) { return _name; }
	inline HWTextureID	getHWTextureID(void) { return _hwID; }

	inline Boolean		isUsed(void) { return _used; }
	inline Boolean		isInstantiated(void) { return _instantiated; }
	inline Uint8*		getBuffer(void) { return _buffer; }

	inline Uint32		getWidth(void) { return _width; }
	inline Uint32		getHeight(void) { return _height; }

	inline Uint32		getMemoryUsage(void) { return _memUsage; }

#ifdef _USE_OPENGL

	inline GLenum		getGLTarget(void) { return _glTarget; }
	inline GLenum		getGLFormat(void) { return _glFormat; }
	inline GLenum		getGLType(void) { return _glType; }

	static GLenum		convertTextureFormatToGLFormat(TextureFormat format);
	static GLenum		convertTextureFormatToGLType(TextureFormat format);

#endif /* _USE_OPENGL */

	void					bindTextureState(TextureState* state);
	inline TextureState*	getBoundTextureState(void) const { return _boundTextureState; }

	void				setAsUsed(void);
	void				setAsUnused(void);

	inline void			setRefID(ObjectID refID) { _refID = refID; }
	void 				setName(const Char* name);

	void				uninstantiate(void);

#ifdef _USE_OPENGL

	EOSError			createFromGLImage(GLubyte* image, GLint width, GLint height, GLenum format, GLenum type, Uint32 texState = 0);

	EOSError			createPALTextureFromGLImage(GLubyte* image, GLint width, GLint height, Uint32 numColors, GLenum format, Uint32 texState = 0);

#endif /* _USE_OPENGL */
	
	EOSError			createFromTGAImage(Uint8* image, Uint32 size, Uint32 texState = 0);
	EOSError			createFromTGAFile(const Char* name, ObjectID objid, const Char* objname, Uint8* buffer, Uint32 maxSize, Uint32 texState = 0);
};

#endif /* __TEXTURE_H__ */
