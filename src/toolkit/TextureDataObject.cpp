/******************************************************************************
 *
 * File: TextureDataObject.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * TextureData Object
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/TextureDataObject.h"
#include "toolkit/TextureDataObjectRGB2PALConverter.h"

TextureDataObject::TextureDataObject()
{
	_type = TextureDataObjectTypeRGBA_8888;
}

TextureDataObject::~TextureDataObject()
{

}

TextureDataObject& TextureDataObject::operator=(const TextureDataObject& to)
{
	if (this != &to)
	{
		_type = to._type;
		_image = to._image;
		_palette = to._palette;
	}

	return *this;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::configure(TextureDataObject::TextureDataObjectType type, Uint32 width, Uint32 height)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;

	_type = type;

	_image.deallocate();
	_palette.deallocate();

	switch (_type)
	{
		case TextureDataObjectTypeRGBA_8888:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_8888, width, height);
			break;

		case TextureDataObjectTypeRGB_888:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGB_888, width, height);
			break;

		case TextureDataObjectTypeRGBA_5551:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_5551, width, height);
			break;

		case TextureDataObjectTypeRGB_565:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGB_565, width, height);
			break;

		case TextureDataObjectTypeRGBA_4444:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_4444, width, height);
			break;

		case TextureDataObjectTypePAL8_RGBA_8888:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData8, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_8888, 256);
			break;

		case TextureDataObjectTypePAL8_RGB_888:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData8, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGB_888, 256);
			break;

		case TextureDataObjectTypePAL8_RGBA_5551:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData8, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_5551, 256);
			break;

		case TextureDataObjectTypePAL8_RGB_565:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData8, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGB_565, 256);
			break;

		case TextureDataObjectTypePAL8_RGBA_4444:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData8, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_4444, 256);
			break;

		case TextureDataObjectTypePAL4_RGBA_8888:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData4, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_8888, 16);
			break;

		case TextureDataObjectTypePAL4_RGB_888:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData4, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGB_888, 16);
			break;

		case TextureDataObjectTypePAL4_RGBA_5551:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData4, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_5551, 16);
			break;

		case TextureDataObjectTypePAL4_RGB_565:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData4, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGB_565, 16);
			break;

		case TextureDataObjectTypePAL4_RGBA_4444:
			error = _image.configure(ImageDataBuffer::ImageDataBufferEncodingData4, width, height);

			if (error == ImageDataBuffer::ImageDataBufferErrorNone)
				error = _palette.configure(ImageDataBuffer::ImageDataBufferEncodingColorRGBA_4444, 16);
			break;
	}

	if (error != ImageDataBuffer::ImageDataBufferErrorNone)
	{
		_image.deallocate();
		_palette.deallocate();
	}

	return error;
}
	
Uint8* TextureDataObject::getPaletteChannel(ImageDataBuffer::ImageDataBufferChannel channel)
{
	return _palette.getChannel(channel);
}

Uint8* TextureDataObject::setPaletteChannel(ImageDataBuffer::ImageDataBufferChannel channel, Uint8* buffer)
{
	return _palette.setChannel(channel, buffer);
}

Uint8* TextureDataObject::getImageChannel(ImageDataBuffer::ImageDataBufferChannel channel)
{
	return _image.getChannel(channel);
}

Uint8* TextureDataObject::setImageChannel(ImageDataBuffer::ImageDataBufferChannel channel, Uint8* buffer)
{
	return _image.setChannel(channel, buffer);
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_8888ToRGBA_4444(TextureDataObject& result)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Uint8*							src_r;
	Uint8*							src_g;
	Uint8*							src_b;
	Uint8*							src_a;
	Uint8*							dst_r;
	Uint8*							dst_g;
	Uint8*							dst_b;
	Uint8*							dst_a;
	Uint32							i;

	error = result.configure(TextureDataObjectTypeRGBA_4444, getImageWidth(), getImageHeight());

	if (error == ImageDataBuffer::ImageDataBufferErrorNone)
	{
		src_r = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		src_g = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		src_b = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
		src_a = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

		dst_r = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		dst_g = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		dst_b = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
		dst_a = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

		if (src_r && src_g && src_b && src_a && dst_r && dst_g && dst_b && dst_a)
		{
			for (i=0;i<getImageWidth() * getImageHeight();i++)
			{
				dst_r[i] = src_r[i] >> 4;
				dst_g[i] = src_g[i] >> 4;
				dst_b[i] = src_b[i] >> 4;
				dst_a[i] = src_a[i] >> 4;
			}
		}
		else
			error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_8888ToRGBA_5551(TextureDataObject& result)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Uint8*							src_r;
	Uint8*							src_g;
	Uint8*							src_b;
	Uint8*							src_a;
	Uint8*							dst_r;
	Uint8*							dst_g;
	Uint8*							dst_b;
	Uint8*							dst_a;
	Uint32							i;

	error = result.configure(TextureDataObjectTypeRGBA_5551, getImageWidth(), getImageHeight());

	if (error == ImageDataBuffer::ImageDataBufferErrorNone)
	{
		src_r = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		src_g = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		src_b = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
		src_a = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

		dst_r = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		dst_g = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		dst_b = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
		dst_a = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

		if (src_r && src_g && src_b && src_a && dst_r && dst_g && dst_b && dst_a)
		{
			for (i=0;i<getImageWidth() * getImageHeight();i++)
			{
				dst_r[i] = src_r[i] >> 3;
				dst_g[i] = src_g[i] >> 3;
				dst_b[i] = src_b[i] >> 3;

				if (src_a[i] > 32)
					dst_a[i] = 1;
				else
					dst_a[i] = 0;
			}
		}
		else
			error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_8888ToRGB_565(TextureDataObject& result)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Uint8*							src_r;
	Uint8*							src_g;
	Uint8*							src_b;
	Uint8*							dst_r;
	Uint8*							dst_g;
	Uint8*							dst_b;
	Uint32							i;

	error = result.configure(TextureDataObjectTypeRGB_565, getImageWidth(), getImageHeight());

	if (error == ImageDataBuffer::ImageDataBufferErrorNone)
	{
		src_r = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		src_g = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		src_b = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);

		dst_r = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		dst_g = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		dst_b = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);

		if (src_r && src_g && src_b && dst_r && dst_g && dst_b)
		{
			for (i=0;i<getImageWidth() * getImageHeight();i++)
			{
				dst_r[i] = src_r[i] >> 3;
				dst_g[i] = src_g[i] >> 2;
				dst_b[i] = src_b[i] >> 3;
			}
		}
		else
			error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_8888ToPAL8_RGBA_8888(TextureDataObject& result)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	TextureDataObjectRGB2PALConverter	converter;

	converter.convert(result, *this, TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888);

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_8888ToPAL4_RGBA_4444(TextureDataObject& result)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	TextureDataObjectRGB2PALConverter	converter;

	converter.convert(result, *this, TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444);

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_8888(TextureDataObject& result, TextureDataObject::TextureDataObjectType new_type)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;

	switch (new_type)
	{
		case TextureDataObjectTypeRGBA_8888:
			result = *this;					//	Same type of object
			break;

		case TextureDataObjectTypeRGBA_4444:
			error = _convertRGBA_8888ToRGBA_4444(result);
			break;

		case TextureDataObjectTypeRGBA_5551:
			error = _convertRGBA_8888ToRGBA_5551(result);
			break;

		case TextureDataObjectTypePAL8_RGBA_8888:
			error = _convertRGBA_8888ToPAL8_RGBA_8888(result);
			break;

		case TextureDataObjectTypePAL4_RGBA_4444:
			error = _convertRGBA_8888ToPAL4_RGBA_4444(result);
			 break;

		case TextureDataObjectTypeRGB_565:
			error = _convertRGBA_8888ToRGB_565(result);
			break;

		case TextureDataObjectTypeRGB_888:

		case TextureDataObjectTypePAL4_RGBA_8888:

		case TextureDataObjectTypePAL8_RGB_888:

		case TextureDataObjectTypePAL8_RGBA_5551:

		case TextureDataObjectTypePAL8_RGB_565:

		case TextureDataObjectTypePAL8_RGBA_4444:

		case TextureDataObjectTypePAL4_RGB_888:

		case TextureDataObjectTypePAL4_RGBA_5551:

		case TextureDataObjectTypePAL4_RGB_565:

		default:
			error = ImageDataBuffer::ImageDataBufferErrorUnsupported;
			break;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_5551ToRGBA_8888(TextureDataObject& result)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Uint8*							src_r;
	Uint8*							src_g;
	Uint8*							src_b;
	Uint8*							src_a;
	Uint8*							dst_r;
	Uint8*							dst_g;
	Uint8*							dst_b;
	Uint8*							dst_a;
	Uint32							i;

	error = result.configure(TextureDataObjectTypeRGBA_8888, getImageWidth(), getImageHeight());

	if (error == ImageDataBuffer::ImageDataBufferErrorNone)
	{
		src_r = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		src_g = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		src_b = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
		src_a = getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

		dst_r = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		dst_g = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		dst_b = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
		dst_a = result.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

		if (src_r && src_g && src_b && src_a && dst_r && dst_g && dst_b && dst_a)
		{
			for (i=0;i<getImageWidth() * getImageHeight();i++)
			{
				//	This is a lazy method and needs to be fixed so that it is more properly mapped
				dst_r[i] = src_r[i] << 3;
				dst_g[i] = src_g[i] << 3;
				dst_b[i] = src_b[i] << 3;

				if (src_a[i])
					dst_a[i] = 255;
				else
					dst_a[i] = 0;
			}
		}
		else
			error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::_convertRGBA_5551(TextureDataObject& result, TextureDataObject::TextureDataObjectType new_type)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;

	switch (new_type)
	{
		case TextureDataObjectTypeRGBA_8888:
			error = _convertRGBA_5551ToRGBA_8888(result);
			result = *this;					//	Same type of object
			break;

		case TextureDataObjectTypeRGBA_5551:
			result = *this;					//	Same type of object
			break;

		case TextureDataObjectTypePAL8_RGBA_8888:

		case TextureDataObjectTypePAL4_RGBA_4444:

		case TextureDataObjectTypeRGB_888:

		case TextureDataObjectTypeRGB_565:

		case TextureDataObjectTypeRGBA_4444:

		case TextureDataObjectTypePAL4_RGBA_8888:

		case TextureDataObjectTypePAL8_RGB_888:

		case TextureDataObjectTypePAL8_RGBA_5551:

		case TextureDataObjectTypePAL8_RGB_565:

		case TextureDataObjectTypePAL8_RGBA_4444:

		case TextureDataObjectTypePAL4_RGB_888:

		case TextureDataObjectTypePAL4_RGBA_5551:

		case TextureDataObjectTypePAL4_RGB_565:

		default:
			error = ImageDataBuffer::ImageDataBufferErrorUnsupported;
			break;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TextureDataObject::convert(TextureDataObject& result, TextureDataObject::TextureDataObjectType new_type)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;

	switch (_type)
	{
		case TextureDataObjectTypeRGBA_8888:
			error = _convertRGBA_8888(result, new_type);
			break;

		case TextureDataObjectTypeRGBA_5551:
			error = _convertRGBA_5551(result, new_type);
			break;

		case TextureDataObjectTypePAL8_RGBA_8888:

		case TextureDataObjectTypeRGB_888:

		case TextureDataObjectTypeRGB_565:

		case TextureDataObjectTypeRGBA_4444:

		case TextureDataObjectTypePAL8_RGB_888:

		case TextureDataObjectTypePAL8_RGBA_5551:

		case TextureDataObjectTypePAL8_RGB_565:

		case TextureDataObjectTypePAL8_RGBA_4444:

		case TextureDataObjectTypePAL4_RGBA_8888:

		case TextureDataObjectTypePAL4_RGB_888:

		case TextureDataObjectTypePAL4_RGBA_5551:

		case TextureDataObjectTypePAL4_RGB_565:

		case TextureDataObjectTypePAL4_RGBA_4444:

		default:
			error = ImageDataBuffer::ImageDataBufferErrorUnsupported;
			break;
	}

	return error;
}

