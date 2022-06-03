/******************************************************************************
 *
 * File: GLTextureDataFormat.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * GL TextureData Format
 * 
 *****************************************************************************/

#ifndef __GL_TEXTURE_DATA_FORMAT_H__
#define __GL_TEXTURE_DATA_FORMAT_H__

#include "Platform.h"
#include "toolkit/TextureDataFormat.h"

#define GL_IMAGE_TYPE_NONE		0
#define GL_IMAGE_TYPE_INDEXED	1
#define GL_IMAGE_TYPE_RGB		2
#define GL_IMAGE_TYPE_GRAY		4
#define GL_IMAGE_TYPE_RLE		8

class GLTextureDataFormat : TextureDataFormat
{
protected:

	ImageDataBuffer::ImageDataBufferError	_writeRGBA_8888FromTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_writeRGB_888FromTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_writeRGBA_4444FromTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_writeRGB_565FromTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_writeRGBA_5551FromTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_writePAL8_RGBA_8888FromTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_writePAL4_RGBA_4444FromTextureDataObject(Uint8* buffer);

public:
	GLTextureDataFormat();
	~GLTextureDataFormat();

	Uint32									length(void);

	//	Case of image buffer (for file and memory images)
	inline ImageDataBuffer::ImageDataBufferError	importFormat(const Uint8* buffer, Uint32 len) { return ImageDataBuffer::ImageDataBufferErrorUnsupported; }

	//	Case of TextureDataObject
	ImageDataBuffer::ImageDataBufferError			importFormat(TextureDataObject& tex);

	//	Case of image buffer (for file and memory images)
	ImageDataBuffer::ImageDataBufferError			exportFormat(Uint8* buffer, Uint32 len);

	//	Case of TextureDataObject
	inline ImageDataBuffer::ImageDataBufferError	exportFormat(TextureDataObject& tex) { return TextureDataFormat::exportFormat(tex); }
};

#endif /* __GL_TEXTURE_DATA_FORMAT_H__ */
