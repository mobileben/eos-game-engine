/******************************************************************************
 *
 * File: TextureDataFormat.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * TextureData Format base class
 * 
 *****************************************************************************/

#ifndef __TEXTURE_DATA_FORMAT_H__
#define __TEXTURE_DATA_FORMAT_H__

#include "Platform.h"
#include "toolkit/TextureDataObject.h"
#include "Endian.h"

class TextureDataFormat : public Endian
{
protected:
	TextureDataObject	_texture_object;

public:
	TextureDataFormat() {}
	~TextureDataFormat(){}

	virtual Uint32							length(void) =0;

	//	Case of image buffer (for file and memory images)
	virtual ImageDataBuffer::ImageDataBufferError	importFormat(const Uint8* buffer, Uint32 len) =0;

	//	Case of TextureDataObject
	virtual ImageDataBuffer::ImageDataBufferError	importFormat(TextureDataObject& tex) =0;

	//	Case of image buffer (for file and memory images)
	virtual ImageDataBuffer::ImageDataBufferError	exportFormat(Uint8* buffer, Uint32 len) =0;

	//	Case of TextureDataObject
	virtual ImageDataBuffer::ImageDataBufferError	exportFormat(TextureDataObject& tex) { 	tex = _texture_object; return ImageDataBuffer::ImageDataBufferErrorNone; }
};

#endif /* __TEXTURE_DATA_FORMAT_H__ */
