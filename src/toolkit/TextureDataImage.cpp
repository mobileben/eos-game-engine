/******************************************************************************
 *
 * File: TextureDataImage.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * TextureData Image
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/TextureDataImage.h"

TextureDataImage::TextureDataImage()
{

}

TextureDataImage::~TextureDataImage()
{

}

TextureDataImage& TextureDataImage::operator=(const TextureDataImage& ti)
{
	if (this != &ti)
	{
		ImageDataBuffer::operator=(ti);
	}

	return *this;
}

ImageDataBuffer::ImageDataBufferError TextureDataImage::configure(ImageDataBuffer::ImageDataBufferEncoding encoding, Uint32 width, Uint32 height)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;

	switch (encoding)
	{
		case ImageDataBuffer::ImageDataBufferEncodingData8:
		case ImageDataBuffer::ImageDataBufferEncodingData4:
			error = allocate(width, height, ImageDataBuffer::ImageDataBufferTypeData, encoding);
			break;

		case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_8888:
		case ImageDataBuffer::ImageDataBufferEncodingColorRGB_888:
		case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_5551:
		case ImageDataBuffer::ImageDataBufferEncodingColorRGB_565:
		case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_4444:
			error = allocate(width, height, ImageDataBuffer::ImageDataBufferTypeColor, encoding);
			break;

		default:
			error = ImageDataBuffer::ImageDataBufferErrorIllegalDefinition;
			break;
	}

	return error;
}

