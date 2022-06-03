/******************************************************************************
 *
 * File: TextureDataObjectRGB2PALConverter.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * TextureData Object Converter
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/TextureDataObjectRGB2PALConverter.h"

TextureDataObjectRGB2PALConverter::TextureDataObjectRGB2PALConverter()
{
}

TextureDataObjectRGB2PALConverter::~TextureDataObjectRGB2PALConverter()
{
}

Uint8	TextureDataObjectRGB2PALConverter::_findColorIndex(Uint8 a, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32	i;

	for (i=0;i<_basic_pixel_profile.size();i++)
	{
		if (a == 0)
		{
			if (_basic_pixel_profile[i].a == a)
				return i;
		}
		else if (_basic_pixel_profile[i].a == a && _basic_pixel_profile[i].r == r && _basic_pixel_profile[i].g == g && _basic_pixel_profile[i].b == b)
			return i;
	}

	return 0;
}

bool TextureDataObjectRGB2PALConverter::_addUniquePixelToBasicProfile(ImagePixel& pixel)
{
	Uint32	i;

	for (i=0;i<_basic_pixel_profile.size();i++)
	{
		if (pixel.a == 0)
		{
			//	Transparent pixels are always made magenta
			if (_basic_pixel_profile[i].a == 0)
			{
				_basic_pixel_profile[i].count++;
				return true;
			}
		}
		else
		{
			if (_basic_pixel_profile[i].a == pixel.a && _basic_pixel_profile[i].r == pixel.r && _basic_pixel_profile[i].g == pixel.g && _basic_pixel_profile[i].b == pixel.b)
			{
				_basic_pixel_profile[i].count++;
				return true;
			}
		}
	}

	pixel.count = 0;

	if (pixel.a == 0)
	{
		pixel.a = 0;
		pixel.r = 255;
		pixel.g = 0;
		pixel.b = 255;

		_basic_pixel_profile.push_back(pixel);
	}
	else
		_basic_pixel_profile.push_back(pixel);

	return false;
}

void TextureDataObjectRGB2PALConverter::_buildPixelProfile(TextureDataObject& src, Uint8 alpha_shift, Uint8 shift_op)
{
	Uint8*							src_r;
	Uint8*							src_g;
	Uint8*							src_b;
	Uint8*							src_a;
	Uint32							i;
	ImagePixel						pixel;

	_basic_pixel_profile.clear();

	src_r = src.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	src_g = src.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	src_b = src.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	src_a = src.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (src_r && src_g && src_b && src_a)
	{
		for (i=0;i<src.getImageWidth() * src.getImageHeight();i++)
		{
			pixel.a = (src_a[i] >> shift_op) >> alpha_shift;
			pixel.r = (src_r[i] >> shift_op);
			pixel.g = (src_g[i] >> shift_op);
			pixel.b = (src_b[i] >> shift_op);

			_addUniquePixelToBasicProfile(pixel);
		}
	}
}

void TextureDataObjectRGB2PALConverter::_convertToP8RGBA8888(TextureDataObject& result, TextureDataObject& source)
{
	Uint8*							index_buffer;
	Uint8*							pal_r;
	Uint8*							pal_g;
	Uint8*							pal_b;
	Uint8*							pal_a;
	Uint32							i;
	Uint8*							src_r;
	Uint8*							src_g;
	Uint8*							src_b;
	Uint8*							src_a;
	Uint32							max_pal;
	Uint8							alpha_shift = -1;
	double							alpha_adjust;

	do
	{
		alpha_shift++;
		_buildPixelProfile(source, alpha_shift);

	} while (_basic_pixel_profile.size() > 256 && alpha_shift < 8 );

	printf("Number of colors %d %x\n", _basic_pixel_profile.size(), alpha_shift);

	//	For now we assume all just works.
	result.configure(TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888, source.getImageWidth(), source.getImageHeight());

	pal_r = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	pal_g = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	pal_b = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	pal_a = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);
	index_buffer = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelData);

	max_pal = (Uint32) _basic_pixel_profile.size();

	if (max_pal > 256)
		max_pal = 256;

	alpha_adjust = (pow(2.0, 8.0) - 1.0) / (pow(2.0, (8.0 - (double) alpha_shift)) - 1.0);

	for (i=0;i<max_pal;i++)
	{
		pal_a[i] = (int) (_basic_pixel_profile[i].a * alpha_adjust);
		pal_r[i] = _basic_pixel_profile[i].r;
		pal_g[i] = _basic_pixel_profile[i].g;
		pal_b[i] = _basic_pixel_profile[i].b;
	}

	//	Now fill out the indices
	src_r = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	src_g = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	src_b = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	src_a = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	for (i=0;i<result.getImageWidth() * result.getImageHeight();i++)
	{
		index_buffer[i] = _findColorIndex(src_a[i] >> alpha_shift, src_r[i], src_g[i], src_b[i]);
	}
}

void TextureDataObjectRGB2PALConverter::_convertToP4RGBA4444(TextureDataObject& result, TextureDataObject& source)
{
	Uint8*							index_buffer;
	Uint8*							pal_r;
	Uint8*							pal_g;
	Uint8*							pal_b;
	Uint8*							pal_a;
	Uint32							i;
	Uint8*							src_r;
	Uint8*							src_g;
	Uint8*							src_b;
	Uint8*							src_a;
	Uint32							max_pal;
	Uint8							alpha_shift = -1;
	double							alpha_adjust;

	do
	{
		alpha_shift++;
		_buildPixelProfile(source, alpha_shift, 4);
	} while (_basic_pixel_profile.size() > 16 && alpha_shift < 4 );

	printf("Number of colors %d %x\n", _basic_pixel_profile.size(), alpha_shift);

	//	For now we assume all just works.
	result.configure(TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444, source.getImageWidth(), source.getImageHeight());

	pal_r = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	pal_g = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	pal_b = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	pal_a = result.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);
	index_buffer = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelData);

	max_pal = (Uint32) _basic_pixel_profile.size();

	if (max_pal > 16)
		max_pal = 16;

	alpha_adjust = (pow(2.0, 4.0) - 1.0) / (pow(2.0, (4.0 - (double) alpha_shift)) - 1.0);

	for (i=0;i<max_pal;i++)
	{
		pal_a[i] = (unsigned char) (_basic_pixel_profile[i].a * alpha_adjust);
		pal_r[i] = _basic_pixel_profile[i].r;
		pal_g[i] = _basic_pixel_profile[i].g;
		pal_b[i] = _basic_pixel_profile[i].b;
	}

	//	Now fill out the indices
	src_r = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	src_g = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	src_b = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	src_a = source.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	for (i=0;i<result.getImageWidth() * result.getImageHeight();i++)
	{
		index_buffer[i] = _findColorIndex((src_a[i] >> 4) >> alpha_shift, src_r[i] >> 4, src_g[i] >> 4, src_b[i] >> 4);
	}
}

void TextureDataObjectRGB2PALConverter::convert(TextureDataObject& result, TextureDataObject& source, TextureDataObject::TextureDataObjectType new_type)
{
	switch (new_type)
	{
		case TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888:
			_convertToP8RGBA8888(result, source);
			break;
		case TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444:
			_convertToP4RGBA4444(result, source);
			break;
	}
}

