/******************************************************************************
 *
 * File: GLTextureDataFormat.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * GL TextureData Format
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/GLTextureDataFormat.h"

GLTextureDataFormat::GLTextureDataFormat()
{
}

GLTextureDataFormat::~GLTextureDataFormat()
{
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::_writeRGBA_8888FromTextureDataObject(Uint8* buffer)
{
	Uint32	x, y;
	Uint8*	r;
	Uint8*	g;
	Uint8*	b;
	Uint8*	a;
	Sint32	offset;

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	a = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (r && g && b && a)
	{
		for (y=0;y<_texture_object.getImageHeight();y++)
		{
			offset = y * _texture_object.getImageWidth();

			for (x=0;x<_texture_object.getImageWidth();x++)
			{
				buffer[(offset + x) * 4 + 0] = r[offset+x];
				buffer[(offset + x) * 4 + 1] = g[offset+x];
				buffer[(offset + x) * 4 + 2] = b[offset+x];
				buffer[(offset + x) * 4 + 3] = a[offset+x];
			}
		}

		return ImageDataBuffer::ImageDataBufferErrorNone;
	}
	else
		return ImageDataBuffer::ImageDataBufferErrorIllegalDefinition;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::_writeRGB_888FromTextureDataObject(Uint8* buffer)
{
	Uint32	x, y;
	Uint8*	r;
	Uint8*	g;
	Uint8*	b;
	Sint32	offset;

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);

	if (r && g && b)
	{
		for (y=0;y<_texture_object.getImageHeight();y++)
		{
			offset = y * _texture_object.getImageWidth();

			for (x=0;x<_texture_object.getImageWidth();x++)
			{
				buffer[(offset + x) * 3 + 0] = r[offset+x];
				buffer[(offset + x) * 3 + 1] = g[offset+x];
				buffer[(offset + x) * 3 + 2] = b[offset+x];
			}
		}

		return ImageDataBuffer::ImageDataBufferErrorNone;
	}
	else
		return ImageDataBuffer::ImageDataBufferErrorIllegalDefinition;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::_writeRGBA_4444FromTextureDataObject(Uint8* buffer)
{
	Uint32	x, y;
	Uint8*	r;
	Uint8*	g;
	Uint8*	b;
	Uint8*	a;
	Uint16	pixel;
	Sint32	offset;

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	a = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (r && g && b && a)
	{
		for (y=0;y<_texture_object.getImageHeight();y++)
		{
			offset = y * _texture_object.getImageWidth();

			for (x=0;x<_texture_object.getImageWidth();x++)
			{
				pixel = (r[offset + x] & 0xF) << 12 | (g[offset + x] & 0xF) << 8 | (b[offset + x] & 0xF) << 4 | (a[offset + x] & 0xF);
				writeUint16(&buffer[(offset + x) * 2], pixel);
			}
		}

		return ImageDataBuffer::ImageDataBufferErrorNone;
	}
	else
		return ImageDataBuffer::ImageDataBufferErrorIllegalDefinition;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::_writeRGB_565FromTextureDataObject(Uint8* buffer)
{
	Uint32	x, y;
	Uint16	pixel;
	Uint8*	r;
	Uint8*	g;
	Uint8*	b;
	Sint32	offset;

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);

	if (r && g && b)
	{
		for (y=0;y<_texture_object.getImageHeight();y++)
		{
			offset = y * _texture_object.getImageWidth();
	
			for (x=0;x<_texture_object.getImageWidth();x++)
			{
				pixel = (r[offset + x] & 0x1F) << 11 | (g[offset + x] & 0x3F) << 5 | (b[offset + x] & 0x1F);
				writeUint16(&buffer[(offset + x) * 2], pixel);
			}
		}

		return ImageDataBuffer::ImageDataBufferErrorNone;
	}
	else
		return ImageDataBuffer::ImageDataBufferErrorIllegalDefinition;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::_writeRGBA_5551FromTextureDataObject(Uint8* buffer)
{
	Uint32	x, y;
	Uint16	pixel;
	Uint8*	r;
	Uint8*	g;
	Uint8*	b;
	Uint8*	a;
	Sint32	offset;

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	a = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (r && g && b && a)
	{
		for (y=0;y<_texture_object.getImageHeight();y++)
		{
			offset = y * _texture_object.getImageWidth();
	
			for (x=0;x<_texture_object.getImageWidth();x++)
			{
				pixel = r[offset + x] << 11 | g[offset + x] << 6 | b[offset + x] << 1 | a[offset + x];
				writeUint16(&buffer[(offset + x) * 2], pixel);
			}
		}

		return ImageDataBuffer::ImageDataBufferErrorNone;
	}
	else
		return ImageDataBuffer::ImageDataBufferErrorIllegalDefinition;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::_writePAL8_RGBA_8888FromTextureDataObject(Uint8* buffer)
{
	Uint8*	r;
	Uint8*	g;
	Uint8*	b;
	Uint8*	a;
	Uint8*	data;
	Sint32	offset;
	Uint32	i;

	r = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	a = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (r && g && b && a)
	{
		offset = 0;
	
		for (i=0;i<_texture_object.getPaletteNumColors();i++)
		{
			//	Export
			buffer[offset] = r[i];
			offset++;
	
			buffer[offset] = g[i];
			offset++;
	
			buffer[offset] = b[i];
			offset++;
	
			buffer[offset] = a[i];
			offset++;
		}
	
		//	Now export the index map
		data = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelData);

		if (data)
		{
			memcpy(&buffer[offset], data, _texture_object.getImageWidth() * _texture_object.getImageHeight());
			return ImageDataBuffer::ImageDataBufferErrorNone;
		}
	}

	return ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::_writePAL4_RGBA_4444FromTextureDataObject(Uint8* buffer)
{
	Uint8*	r;
	Uint8*	g;
	Uint8*	b;
	Uint8*	a;
	Uint8*	data;
	Sint32	offset;
	Uint32	i;

	r = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	a = _texture_object.getPaletteChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (r && g && b && a)
	{
		offset = 0;
	
		for (i=0;i<_texture_object.getPaletteNumColors();i++)
		{
			//	Export
			buffer[offset] = (b[i] & 0x0F) << 4 | (a[i] & 0x0F);
			offset++;

			buffer[offset] = (r[i] & 0x0F) << 4 | (g[i] & 0x0F);
			offset++;
		}
	
		//	Now export the index map
		data = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelData);

		if (data)
		{
			for (i=0;i<_texture_object.getImageWidth() * _texture_object.getImageHeight() / 2;i++)
			{
				buffer[offset] = (data[i * 2] & 0x0F) << 4 | data[i * 2 + 1];
				offset++;
			}
			
			return ImageDataBuffer::ImageDataBufferErrorNone;
		}
	}

	return ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
}

Uint32 GLTextureDataFormat::length(void)
{
	Uint32	len = 0;
	Float32	width = 0.0F;

	//	Length is dependent up on various elements
	if (_texture_object.getPaletteNumColors() > 0)
	{
		switch (_texture_object.getPaletteEncoding())
		{

			case ImageDataBuffer::ImageDataBufferEncodingData8:
				width = 1.0F;
				break;

			case ImageDataBuffer::ImageDataBufferEncodingData4:
				width = 0.5F;
				break;

			case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_8888:
				width = 4.0F;
				break;

			case ImageDataBuffer::ImageDataBufferEncodingColorRGB_888:
				width = 3.0F;
				break;

			case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_5551:
				width = 2.0F;
				break;

			case ImageDataBuffer::ImageDataBufferEncodingColorRGB_565:
				width = 2.0F;
				break;

			case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_4444:
				width = 2.0F;
				break;
		}

		len += (Uint32) (width * _texture_object.getPaletteNumColors());
	}

	switch (_texture_object.getImageEncoding())
	{

		case ImageDataBuffer::ImageDataBufferEncodingData8:
			width = 1.0F;
			break;

		case ImageDataBuffer::ImageDataBufferEncodingData4:
			width = 0.5F;
			break;

		case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_8888:
			width = 4.0F;
			break;

		case ImageDataBuffer::ImageDataBufferEncodingColorRGB_888:
			width = 3.0F;
			break;

		case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_5551:
			width = 2.0F;
			break;

		case ImageDataBuffer::ImageDataBufferEncodingColorRGB_565:
			width = 2.0F;
			break;

		case ImageDataBuffer::ImageDataBufferEncodingColorRGBA_4444:
			width = 2.0F;
			break;
	}

	len += (Uint32) (_texture_object.getImageWidth() * _texture_object.getImageHeight() * width);

	return len;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::importFormat(TextureDataObject& tex)
{
	//	GL is pure data, so no needs for any headers, so this is enuf
	_texture_object = tex;

	return ImageDataBuffer::ImageDataBufferErrorNone;
}

ImageDataBuffer::ImageDataBufferError GLTextureDataFormat::exportFormat(Uint8* buffer, Uint32 len)
{
	if (buffer && len >= length())
	{
		switch (_texture_object.getTextureDataObjectType())
		{
			case TextureDataObject::TextureDataObjectTypeRGBA_8888:
				_writeRGBA_8888FromTextureDataObject(buffer);
				break;

			case TextureDataObject::TextureDataObjectTypeRGB_888:
				_writeRGB_888FromTextureDataObject(buffer);
				break;

			case TextureDataObject::TextureDataObjectTypeRGBA_5551:
				_writeRGBA_5551FromTextureDataObject(buffer);
				break;

			case TextureDataObject::TextureDataObjectTypeRGB_565:
				_writeRGB_565FromTextureDataObject(buffer);
				break;

			case TextureDataObject::TextureDataObjectTypeRGBA_4444:
				_writeRGBA_4444FromTextureDataObject(buffer);
				break;

			case TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888:
				_writePAL8_RGBA_8888FromTextureDataObject(buffer);
				break;

			case TextureDataObject::TextureDataObjectTypePAL8_RGB_888:
				break;

			case TextureDataObject::TextureDataObjectTypePAL8_RGBA_5551:
				break;

			case TextureDataObject::TextureDataObjectTypePAL8_RGB_565:
				break;

			case TextureDataObject::TextureDataObjectTypePAL8_RGBA_4444:
				break;

			case TextureDataObject::TextureDataObjectTypePAL4_RGBA_8888:
				break;

			case TextureDataObject::TextureDataObjectTypePAL4_RGB_888:
				break;

			case TextureDataObject::TextureDataObjectTypePAL4_RGBA_5551:
				break;

			case TextureDataObject::TextureDataObjectTypePAL4_RGB_565:
				break;

			case TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444:
				_writePAL4_RGBA_4444FromTextureDataObject(buffer);
				break;

			default:
				return ImageDataBuffer::ImageDataBufferErrorUnsupported;
		}

		return ImageDataBuffer::ImageDataBufferErrorNone;
	}
	else
		return ImageDataBuffer::ImageDataBufferErrorMemory;
}

