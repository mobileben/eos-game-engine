/******************************************************************************
 *
 * File: TextureDataPalette.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * TextureData Palette
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/TextureDataPalette.h"

TextureDataPalette::TextureDataPalette()
{
	_num_colors = 0;
}

TextureDataPalette::~TextureDataPalette()
{

}

TextureDataPalette& TextureDataPalette::operator=(const TextureDataPalette& tp)
{
	if (this != &tp)
	{
		_num_colors = tp._num_colors;
		ImageDataBuffer::operator=(tp);
	}

	return *this;
}

ImageDataBuffer::ImageDataBufferError TextureDataPalette::configure(ImageDataBuffer::ImageDataBufferEncoding encoding, Uint32 num_color)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;

	switch (num_color)
	{
		case 256:
			_num_colors = num_color;

			allocate(_num_colors, 1, ImageDataBuffer::ImageDataBufferTypeColor, encoding);
			break;

		case 16:
			_num_colors = num_color;

			allocate(_num_colors, 1, ImageDataBuffer::ImageDataBufferTypeColor, encoding);
			break;

		default:
			error = ImageDataBuffer::ImageDataBufferErrorIllegalDefinition;
			break;
	}

	return error;
}

