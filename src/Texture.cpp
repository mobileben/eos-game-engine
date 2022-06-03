/******************************************************************************
 *
 * File: Texture.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Texture object
 * 
 *****************************************************************************/

#include "Texture.h"
#include "TextureState.h"
#include "Endian.h"
#include "File.h"
#include "App.h"

#ifdef _USE_OPENGL
#ifdef _PLATFORM_PC

#define GL_PALETTE4_RGB8_OES 0x8B90
#define GL_PALETTE4_RGBA8_OES 0x8B91
#define GL_PALETTE4_R5_G6_B5_OES 0x8B92
#define GL_PALETTE4_RGBA4_OES 0x8B93
#define GL_PALETTE4_RGB5_A1_OES 0x8B94
#define GL_PALETTE8_RGB8_OES 0x8B95
#define GL_PALETTE8_RGBA8_OES 0x8B96
#define GL_PALETTE8_R5_G6_B5_OES 0x8B97
#define GL_PALETTE8_RGBA4_OES 0x8B98
#define GL_PALETTE8_RGB5_A1_OES 0x8B99

#endif /* _PLATOFORM_PC */
#endif /* _USE_OPENGL */

Texture::Texture() : _name(NULL), _boundTextureState(NULL), _memUsage(0)
{
	_init();
}

Texture::~Texture()
{
	if (_boundTextureState)
	{
		delete _boundTextureState;
		_boundTextureState = NULL;
	}
}

void Texture::_init(void)
{
	if (_name)
	{
		delete _name;
		_name = NULL;
	}

	_refID = InvalidObjectID;
	_hwID = InvalidHWTextureID;

	_used = false;
	_instantiated = false;
	_freeable = false;

	_buffer = NULL;

	_width = _height = 0;

	_numPalColors = 0;

#ifdef _USE_OPENGL
	_glTarget = 0;
	_glFormat = 0;
	_glType = 0;
#endif /* _USE_OPENGL */

	_memUsage = 0;

	if (_boundTextureState)
	{
		delete _boundTextureState;
		_boundTextureState = NULL;
	}
}

void Texture::setAsUsed(void)
{
	if (_used == false)
	{
		_init();

		_used = true;
		
		if (_appRefPtr)
			_appRefPtr->getTextureManager()->updateNumUsedTextures();
	}
}

void Texture::setAsUnused(void)
{
	if (_used == true)
	{
		uninstantiate();

		_init();

		_used = false;
		
		if (_appRefPtr)
			_appRefPtr->getTextureManager()->updateNumUsedTextures();
	}
}

void Texture::setName(const Char* name)
{
	delete _name;
	_name = NULL;

	if (name)
	{
		_name = new Char[strlen(name) + 1];

		if (_name)
		{
			strcpy(_name, name);
		}
	}
}

void Texture::uninstantiate(void)
{
	if (_instantiated)
	{
		if (_hwID != InvalidHWTextureID)
		{
#ifdef _USE_OPENGL			
			glDeleteTextures(1, &_hwID);
#endif /* _USE_OPENGL */
			
			_hwID = InvalidHWTextureID;
			_instantiated = false;
			_memUsage = 0;

			_appRefPtr->getRenderer()->flushAllTextures();
		}
	}
}

#ifdef _USE_OPENGL

GLenum Texture::convertTextureFormatToGLFormat(TextureFormat format)
{
	GLenum	glFormat = 0;

	switch (format)
	{
		case TextureFormatTGA:
			glFormat = GL_RGBA;
			break;

		case TextureFormatGL_RGBA_8888:
		case TextureFormatGL_RGBA_5551:
		case TextureFormatGL_RGBA_4444:
			glFormat = GL_RGBA;
			break;

		case TextureFormatGL_RGB_888:
		case TextureFormatGL_RGB_565:
			glFormat = GL_RGB;
			break;

		case TextureFormatGL_P8_RGBA_8888:
			glFormat = GL_PALETTE8_RGBA8_OES;
			break;

		case TextureFormatGL_P4_RGBA_4444:
			glFormat = GL_PALETTE4_RGBA4_OES;
			break;

#ifdef _USE_PVR_OPENGLES
		case TextureFormatPVRTC_4BPP_RGBA:
		case TextureFormatRAWPVRTC_4BPP_RGBA:
			glFormat = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
			break;

		case TextureFormatPVRTC_4BPP_RGB:
		case TextureFormatRAWPVRTC_4BPP_RGB:
			glFormat = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
			break;

		case TextureFormatPVRTC_2BPP_RGBA:
		case TextureFormatRAWPVRTC_2BPP_RGBA:
			glFormat = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
			break;

		case TextureFormatPVRTC_2BPP_RGB:
		case TextureFormatRAWPVRTC_2BPP_RGB:
			glFormat = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
			break;
#endif /* _USE_PVR_OPENGLES */

		default:
			AssertWDesc(1 == 0, "Illegal TextureFormat.");
			break;
	}

	return glFormat;
}

GLenum Texture::convertTextureFormatToGLType(TextureFormat format)
{
	GLenum glType = 0;

	switch (format)
	{
		case TextureFormatTGA:
			glType = GL_UNSIGNED_BYTE;
			break;

		case TextureFormatGL_RGBA_8888:
		case TextureFormatGL_RGB_888:
			glType = GL_UNSIGNED_BYTE;
			break;

		case TextureFormatGL_RGBA_5551:
			glType = GL_UNSIGNED_SHORT_5_5_5_1;
			break;

		case TextureFormatGL_RGB_565:
			glType = GL_UNSIGNED_SHORT_5_6_5;
			break;

		case TextureFormatGL_RGBA_4444:
			glType = GL_UNSIGNED_SHORT_4_4_4_4;
			break;

		default:
			AssertWDesc(1 == 0, "Illegal TextureFormat.");
			break;
	}

	return glType;
}

#endif /* _USE_OPENGL */

EOSError Texture::createFromTGAFile(const Char* name, ObjectID objid, const Char* objname, Uint8* buffer, Uint32 maxSize, Uint32 texState)
{
	EOSError		error = EOSErrorNone;
	FileDescriptor	fdesc;
	File			file;

	AssertWDesc(name != NULL, "Texture::createFromTGAFile() NULL name");

	fdesc.setFileAccessType(FileAccessTypeReadOnly);

	fdesc.setFilename(name);

	file.setFileDescriptor(fdesc);

	error = file.open();

	if (error == EOSErrorNone)
	{
		error = file.readUint8(buffer, file.length());

		if (error == EOSErrorNone)
		{
#ifdef _USE_OPENGL
			_appRefPtr->getGfx()->lockGLContext();
#endif /* _USE_OPENGL */
			error = createFromTGAImage((Uint8*) buffer, file.length(), texState);
#ifdef _USE_OPENGL
			_appRefPtr->getGfx()->unlockGLContext();
#endif /* _USE_OPENGL */

			if (error == EOSErrorNone)
			{
				setRefID(objid);
				setName(objname);
			}
		}

		file.close();
	}
	else
	{
		AssertWDesc(1 == 0, "Could not load\n");
	}

	return error;
}

EOSError Texture::createFromTGAImage(Uint8* image, Uint32 size, Uint32 texState)
{
	EOSError 	error = EOSErrorNone;
	short 		width, height;
	char 		headerLength = 0;                  
	char 		imageType = 0;             
	char 		bits = 0;                      
	int 		format= 0;                  
	int			type = 0;
	int 		lineWidth = 0;                          
	int			curr_ptr = 0;
	Endian		endian;

#ifdef _USE_OPENGL
	endian.setTargetEndian(Endian::EndianTypeLittle);
	
	headerLength = image[curr_ptr];
	curr_ptr++;

	curr_ptr++;

	imageType = image[curr_ptr];
	curr_ptr++;

	curr_ptr += 9;

	//	This may need to be fixed for endianess
	width = endian.readSint16(&image[curr_ptr]);
	curr_ptr += 2;

	height = endian.readSint16(&image[curr_ptr]);
	curr_ptr += 2;

	bits = image[curr_ptr];
	curr_ptr++;

	curr_ptr += headerLength + 1;

	GLubyte *buffer = NULL;

	if(imageType != 10)
	{
		// Support for LUMINANCE and RGBA textures
		if((bits == 24)||(bits == 32)) 
		{
			format = bits >> 3; 
			lineWidth = format * width;

			buffer = (GLubyte*) malloc(sizeof(GLubyte) * lineWidth * height);

			if(buffer)
			{
				for(int y = 0; y < height; y++)
				{
					GLubyte *line = &buffer[lineWidth * y];             

					memcpy(line, &image[curr_ptr], lineWidth);
					curr_ptr += lineWidth;

					if(format!= 1)
					{
						for(int i=0;i<lineWidth ; i+=format) //swap R and B because TGA are stored in BGR format
						{
							int temp  = line[i];
							line[i]   = line[i+2];
							line[i+2] = temp;
						}
					}
				}
			}
			else
				error = EOSErrorNoMemory;
		}
		else if(bits == 16)
		{
			format = bits >> 3; 
			lineWidth = format * width;

			buffer = (GLubyte*) malloc(sizeof(GLubyte) * lineWidth * height);

			if(buffer)
			{
				for(int y = 0; y < height; y++)
				{
					GLushort *line = (GLushort*) &buffer[lineWidth * y];             

					curr_ptr += endian.copyUint16((unsigned short*) line, &image[curr_ptr], lineWidth / 2);

					if(format!= 1)
					{
						for(int i=0;i<lineWidth / 2 ; i+=1) //swap R and B because TGA are stored in BGR format
						{
							GLushort a = (line[i] & 0x8000) >> 15;
							GLushort r = (line[i] & 0x001F) >> 0;
							GLushort g = (line[i] & 0x03E0) >> 5;
							GLushort b = (line[i] & 0x7C00) >> 10;

							line[i] = (a << 0) | (b << 11) | (g << 6) | (r << 1);
						}
					}
				}
			}
			else
				error = EOSErrorNoMemory;
		}
		else
			error = EOSErrorUnsupported; 
	}
	else
		error = EOSErrorUnsupported; 

	if(error == EOSErrorNone)
	{

		switch(format)
		{
			case 1:
				format = GL_LUMINANCE;
				break;

			case 2:
				format = GL_RGBA;
				break;

			case 3:
				format = GL_RGB;
				break; 

			case 4:
				format = GL_RGBA;
				break;
		};
		
		switch(bits)
		{
			default:
				type = GL_UNSIGNED_BYTE;
				break;
		}

		createFromGLImage(buffer, width, height, format, type, texState);
	}

	if(buffer != NULL)
	{
		free(buffer);
	}

	if (error == EOSErrorNone)
		_instantiated = true;

#endif /* _USE_OPENGL */

	AssertWDesc(error == EOSErrorNone, "TextureManager::createFromTGAImage() error");

	return error;
}

#ifdef _USE_OPENGL

EOSError Texture::createFromGLImage(GLubyte* image, GLint width, GLint height, GLenum format, GLenum type, Uint32 texState)
{
	EOSError 		error = EOSErrorNone;
	TextureState*	defaultTState = new TextureState;
	Uint32			pixelSize = 1;
#ifdef _USE_OPENGL
	GLenum			glError;
#endif

	if(_used == true)
	{
		if (_hwID == InvalidHWTextureID)
		{
#ifdef _USE_OPENGL
			glGenTextures(1, &_hwID);
#endif /* _USE_OPENGL */
		}

		_width = width;
		_height = height;
		_numPalColors = 0;

		switch (type)
		{
			case GL_UNSIGNED_BYTE:
				pixelSize = 1;
				break;

			case GL_UNSIGNED_SHORT_5_5_5_1:
			case GL_UNSIGNED_SHORT_5_6_5:
			case GL_UNSIGNED_SHORT_4_4_4_4:
				pixelSize = 2;
				break;

			default:
				AssertWDesc(1 == 0, "Illegal TextureFormat.");
				break;
		}

		switch (format)
		{
			case GL_RGBA:
				pixelSize *= 4;
				break;

			case GL_RGB:
				pixelSize *= 3;
				break;
		}

		_memUsage = _width * _height * pixelSize;

#ifdef _USE_OPENGL
		_glTarget = GL_TEXTURE_2D;
		_glFormat = format;
		_glType = type;

		_appRefPtr->getRenderer()->setTexture(_glTarget, _hwID);

		glTexImage2D(_glTarget, 0, _glFormat, _width, _height, 0, _glFormat, _glType, image);    

		glError = glGetError();

		AssertWDesc(glError == GL_NO_ERROR || glError == GL_INVALID_ENUM, "TextureManager::createFromGLImage() GL Error\n");

#endif /* _USE_OPENGL */

		defaultTState->setFromStateFlags(texState);
		defaultTState->bindToTexture(this);
		
		_boundTextureState = defaultTState;
	}
	else
		error = EOSErrorUnallocated;

	if (error == EOSErrorNone)
		_instantiated = true;

	AssertWDesc(error == EOSErrorNone, "TextureManager::createFromGLImage() error");

	_appRefPtr->getRenderer()->flushAllTextures();

	return error;
}

EOSError Texture::createPALTextureFromGLImage(GLubyte* image, GLint width, GLint height, Uint32 numColors, GLenum format, Uint32 texState)
{
	EOSError 		error = EOSErrorNone;
	TextureState*	defaultTState = new TextureState;
	Sint32			size;
#if defined(_PLATFORM_PC)&&!(_USE_PVR_OPENGLES)
	PFNGLCOMPRESSEDTEXIMAGE2DPROC	func =  (PFNGLCOMPRESSEDTEXIMAGE2DPROC) wglGetProcAddress("glCompressedTexImage2D");
#endif /* _PLAFORM_PC */
#ifdef _USE_OPENGL
	GLenum			glError;
#endif

	if(_used == true)
	{
		if (_hwID == InvalidHWTextureID)
		{
			glGenTextures(1, &_hwID);
		}

		_width = width;
		_height = height;
		_numPalColors = numColors;

		_glTarget = GL_TEXTURE_2D;
		_glFormat = format;

		switch (_glFormat)
		{
			case GL_PALETTE8_RGBA8_OES:
				//	4 is for the 4 bytes in RGBA_8888 "8 *" is for 8 bit per pixel
				size = 4 * _numPalColors + 8 * _width * _height / 8;
				break;

			case GL_PALETTE4_RGBA4_OES:
				//	2 is for the 4 bytes in RGBA_4444 "4 *" is for 4 bit per pixel
				size = 2 * _numPalColors + 4 * _width * _height / 8;
				break;

#ifdef _USE_PVR_OPENGLES
			case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
			case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
				//	"4 *" is for 4 bit per pixel
				size = 4 * _width * _height / 8;
				break;

			case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
			case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
				//	"2 *" is for 2 bit per pixel
				size = 2 * _width * _height / 8;
				break;
#endif /* _USE_PVR_OPENGLES */
		}

		_memUsage = size;

		_glType = 0;

		_appRefPtr->getRenderer()->setTexture(_glTarget, _hwID);

#if defined(_PLATFORM_PC)&&!(_USE_PVR_OPENGLES)
		func(_glTarget, 0, _glFormat, _width, _height, 0, size, image);
#else
		glCompressedTexImage2D(_glTarget, 0, _glFormat, _width, _height, 0, size, image);
#endif

#ifdef _USE_OPENGL
		glError = glGetError();

		AssertWDesc(glError == GL_NO_ERROR || glError == GL_INVALID_ENUM, "TextureManager::createPALTextureFromGLImage() GL Error\n");
#endif

		defaultTState->setFromStateFlags(texState);
		defaultTState->bindToTexture(this);
		
		_boundTextureState = defaultTState;
	}
	else
		error = EOSErrorUnallocated;

	if (error == EOSErrorNone)
		_instantiated = true;

	AssertWDesc(error == EOSErrorNone, "TextureManager::createPALTextureFromGLImage() error");

	_appRefPtr->getRenderer()->flushAllTextures();

	return error;
}

#endif /* _USE_OPENGL */

void Texture::bindTextureState(TextureState* state)
{
	_boundTextureState = state;
}
