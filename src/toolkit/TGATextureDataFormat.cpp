/******************************************************************************
 *
 * File: TGATextureDataFormat.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * TGA TextureData Format
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/TGATextureDataFormat.h"

TGATextureDataFormat::TGATextureDataFormat()
{
	memset(&_header, 0, sizeof(TGATextureDataFormatHeader));
}

TGATextureDataFormat::~TGATextureDataFormat()
{
}

ImageDataBuffer::ImageDataBufferError TGATextureDataFormat::_readRGBA_8888IntoTextureDataObject(Uint8* buffer)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Sint32							x, y;
	Uint8*							r;
	Uint8*							g;
	Uint8*							b;
	Uint8*							a;
	Uint32							offset;

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	a = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (r && g && b && a)
	{
		for (y=0;y<_header.height;y++)
		{
			offset = y * _header.width;

			for (x=0;x<_header.width;x++)
			{
				b[offset + x] = buffer[(offset + x)* 4 + 0];
				g[offset + x] = buffer[(offset + x)* 4 + 1];
				r[offset + x] = buffer[(offset + x)* 4 + 2];
				a[offset + x] = buffer[(offset + x)* 4 + 3];
			}
		}
	}
	else
		error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;

	return error;
}

ImageDataBuffer::ImageDataBufferError TGATextureDataFormat::_readRGB_888IntoTextureDataObject(Uint8* buffer)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Sint32							x, y;
	Uint8*							r;
	Uint8*							g;
	Uint8*							b;
	Uint32							offset;

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);

	if (r && g && b)
	{
		for (y=0;y<_header.height;y++)
		{
			offset = y * _header.width;

			for (x=0;x<_header.width;x++)
			{
				b[offset + x] = buffer[(offset + x)* 3 + 0];
				g[offset + x] = buffer[(offset + x)* 3 + 1];
				r[offset + x] = buffer[(offset + x)* 3 + 2];
			}
		}
	}
	else
		error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;

	return error;
}

ImageDataBuffer::ImageDataBufferError TGATextureDataFormat::_readRGBA_5551IntoTextureDataObject(Uint8* buffer)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Sint32							x, y;
	Uint8*							r;
	Uint8*							g;
	Uint8*							b;
	Uint8*							a;
	Uint32							offset;
	Uint16							pixel;   

	r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
	g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
	b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
	a = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

	if (r && g && b && a)
	{
		for (y=0;y<_header.height;y++)
		{
			offset = y * _header.width;

			for (x=0;x<_header.width;x++)
			{
				pixel = readUint16(&buffer[(offset + x) * 2]);

				a[offset + x] = (pixel & 0x8000) >> 15;
				r[offset + x] = (pixel & 0x7C00) >> 10;
				g[offset + x] = (pixel & 0x03E0) >> 5;
				b[offset + x] = (pixel & 0x001F) >> 0;
			}
		}
	}
	else
		error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;

	return error;
}

Uint32 TGATextureDataFormat::length(void)
{
	Uint32	len = 0;

	switch (_texture_object.getTextureDataObjectType())
	{
		case TextureDataObject::TextureDataObjectTypeRGBA_8888:
		case TextureDataObject::TextureDataObjectTypeRGBA_5551:
			//	For now, the required header size is 18, since we are not running in packed mode
			len += 18 + _header.width * _header.height * _header.bpp / 8;
			break;

		case TextureDataObject::TextureDataObjectTypeRGB_888:
		case TextureDataObject::TextureDataObjectTypeRGB_565:
		case TextureDataObject::TextureDataObjectTypeRGBA_4444:
		case TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888:
		case TextureDataObject::TextureDataObjectTypePAL8_RGB_888:
		case TextureDataObject::TextureDataObjectTypePAL8_RGBA_5551:
		case TextureDataObject::TextureDataObjectTypePAL8_RGB_565:
		case TextureDataObject::TextureDataObjectTypePAL8_RGBA_4444:
		case TextureDataObject::TextureDataObjectTypePAL4_RGBA_8888:
		case TextureDataObject::TextureDataObjectTypePAL4_RGB_888:
		case TextureDataObject::TextureDataObjectTypePAL4_RGBA_5551:
		case TextureDataObject::TextureDataObjectTypePAL4_RGB_565:
		case TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444:
		default:
			break;
	}

	return len;
}

Uint32 TGATextureDataFormat::_writeHeader(Uint8* buffer)
{
	Uint8*	tbuf = buffer;
	Uint32	index = 0;

	tbuf[index] = _header.ident_size;
	index++;

	tbuf[index] = _header.paletted;
	index++;

	tbuf[index] = _header.image_type;
	index++;

	writeSint16(&tbuf[index], _header.palette_start);
	index += 2;

	writeSint16(&tbuf[index], _header.palette_length);
	index += 2;

	tbuf[index] = _header.palette_color_depth;
	index++;

	writeSint16(&tbuf[index], _header.x_start);
	index += 2;

	writeSint16(&tbuf[index], _header.y_start);
	index += 2;

	writeSint16(&tbuf[index], _header.width);
	index += 2;

	writeSint16(&tbuf[index], _header.height);
	index += 2;

	tbuf[index] = _header.bpp;
	index++;

	tbuf[index] = _header.bit_descriptor;
	index++;

	return index;
}

ImageDataBuffer::ImageDataBufferError TGATextureDataFormat::importFormat(const Uint8* buffer, Uint32 len)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Uint8*							tbuf;
	Uint32							index = 0;

	tbuf = (Uint8*) buffer;

	//	Step through
	_header.ident_size = tbuf[index];
	index++;

	_header.paletted = tbuf[index];
	index++;

	_header.image_type = tbuf[index];
	index++;

	_header.palette_start = readSint16(&tbuf[index]);
	index += 2;

	_header.palette_length = readSint16(&tbuf[index]);
	index += 2;

	_header.palette_color_depth = tbuf[index];
	index++;

	_header.x_start = readSint16(&tbuf[index]);
	index += 2;

	_header.y_start = readSint16(&tbuf[index]);
	index += 2;

	_header.width = readSint16(&tbuf[index]);
	index += 2;

	_header.height = readSint16(&tbuf[index]);
	index += 2;

	_header.bpp = tbuf[index];
	index++;

	_header.bit_descriptor = tbuf[index];
	index++;

	index += _header.ident_size;

	//	Now create our TextureDataObject
	switch (_header.image_type)
	{
		case TGA_IMAGE_TYPE_RGB:
			switch (_header.bpp)
			{
				case 32:
					error = _texture_object.configure(TextureDataObject::TextureDataObjectTypeRGBA_8888, _header.width, _header.height);
					
					if (error == ImageDataBuffer::ImageDataBufferErrorNone)
						error = _readRGBA_8888IntoTextureDataObject(&tbuf[index]);
					break;

				case 24:
					error = _texture_object.configure(TextureDataObject::TextureDataObjectTypeRGB_888, _header.width, _header.height);

					if (error == ImageDataBuffer::ImageDataBufferErrorNone)
						error = _readRGB_888IntoTextureDataObject(&tbuf[index]);
					break;

				case 16:
					error = _texture_object.configure(TextureDataObject::TextureDataObjectTypeRGBA_5551, _header.width, _header.height);

					if (error == ImageDataBuffer::ImageDataBufferErrorNone)
						error = _readRGBA_5551IntoTextureDataObject(&tbuf[index]);
					break;

				default:
					error = ImageDataBuffer::ImageDataBufferErrorUnsupported;
					break;
			}
			break;

		default:
			error = ImageDataBuffer::ImageDataBufferErrorUnsupported;
			break;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TGATextureDataFormat::importFormat(TextureDataObject& tex)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;

	_texture_object = tex;

	//	Now build header file based on this
	_header.ident_size = 0;

	switch (_texture_object.getTextureDataObjectType())
	{
		case TextureDataObject::TextureDataObjectTypeRGBA_8888:
			_header.paletted = 0;
			_header.image_type = TGA_IMAGE_TYPE_RGB;
			_header.palette_start = 0;
			_header.palette_length = 0;
			_header.palette_color_depth = 0;
			_header.x_start = 0;
			_header.y_start = 0;
			_header.width = _texture_object.getImageWidth();
			_header.height = _texture_object.getImageHeight();
			_header.bpp = 32;
			_header.bit_descriptor = 8;
			break;

		case TextureDataObject::TextureDataObjectTypeRGBA_5551:
			_header.paletted = 0;
			_header.image_type = TGA_IMAGE_TYPE_RGB;
			_header.palette_start = 0;
			_header.palette_length = 0;
			_header.palette_color_depth = 0;
			_header.x_start = 0;
			_header.y_start = 0;
			_header.width = _texture_object.getImageWidth();
			_header.height = _texture_object.getImageHeight();
			_header.bpp = 16;
			_header.bit_descriptor = 1;
			break;

		case TextureDataObject::TextureDataObjectTypeRGB_888:
		case TextureDataObject::TextureDataObjectTypeRGB_565:
		case TextureDataObject::TextureDataObjectTypeRGBA_4444:
		case TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888:
		case TextureDataObject::TextureDataObjectTypePAL8_RGB_888:
		case TextureDataObject::TextureDataObjectTypePAL8_RGBA_5551:
		case TextureDataObject::TextureDataObjectTypePAL8_RGB_565:
		case TextureDataObject::TextureDataObjectTypePAL8_RGBA_4444:
		case TextureDataObject::TextureDataObjectTypePAL4_RGBA_8888:
		case TextureDataObject::TextureDataObjectTypePAL4_RGB_888:
		case TextureDataObject::TextureDataObjectTypePAL4_RGBA_5551:
		case TextureDataObject::TextureDataObjectTypePAL4_RGB_565:
		case TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444:
		default:
			error = ImageDataBuffer::ImageDataBufferErrorUnsupported;
			break;
	}

	return error;
}

ImageDataBuffer::ImageDataBufferError TGATextureDataFormat::exportFormat(Uint8* buffer, Uint32 len)
{
	ImageDataBuffer::ImageDataBufferError	error = ImageDataBuffer::ImageDataBufferErrorNone;
	Uint32							index = 0;
	Sint32							i;
	Sint32							width;
	Uint8*							r;
	Uint8*							g;
	Uint8*							b;
	Uint8*							a;
	Uint16							pixel16;

	if (buffer && len >= length())
	{
		r = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorRed);
		g = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorGreen);
		b = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorBlue);
		a = _texture_object.getImageChannel(ImageDataBuffer::ImageDataBufferChannelColorAlpha);

		switch (_texture_object.getTextureDataObjectType())
		{
			case TextureDataObject::TextureDataObjectTypeRGBA_8888:
				if (r && g && b && a)
				{
					index += _writeHeader(buffer);

					width = _header.bpp / 8;

					for (i=0;i<_header.width * _header.height;i++)
					{
						buffer[index + (i * 4) + 0] = b[i];
						buffer[index + (i * 4) + 1] = g[i];
						buffer[index + (i * 4) + 2] = r[i];
						buffer[index + (i * 4) + 3] = a[i];
					}
				}
				else
					error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
				break;

			case TextureDataObject::TextureDataObjectTypeRGBA_5551:
				if (r && g && b && a)
				{
					index += _writeHeader(buffer);

					width = _header.bpp / 8;

					for (i=0;i<_header.width * _header.height;i++)
					{
						pixel16 = r[i] << 10 | g[i] << 5 | b[i] << 0 | a[i] << 15;

						writeUint16(&buffer[index + i * 2], pixel16);
					}
				}
				else
					error = ImageDataBuffer::ImageDataBufferErrorIllegalChannel;
				break;

			case TextureDataObject::TextureDataObjectTypeRGB_888:
			case TextureDataObject::TextureDataObjectTypeRGB_565:
			case TextureDataObject::TextureDataObjectTypeRGBA_4444:
			case TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888:
			case TextureDataObject::TextureDataObjectTypePAL8_RGB_888:
			case TextureDataObject::TextureDataObjectTypePAL8_RGBA_5551:
			case TextureDataObject::TextureDataObjectTypePAL8_RGB_565:
			case TextureDataObject::TextureDataObjectTypePAL8_RGBA_4444:
			case TextureDataObject::TextureDataObjectTypePAL4_RGBA_8888:
			case TextureDataObject::TextureDataObjectTypePAL4_RGB_888:
			case TextureDataObject::TextureDataObjectTypePAL4_RGBA_5551:
			case TextureDataObject::TextureDataObjectTypePAL4_RGB_565:
			case TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444:
			default:
				error = ImageDataBuffer::ImageDataBufferErrorUnsupported;
				break;
		}
	}
	else
		error = ImageDataBuffer::ImageDataBufferErrorMemory;

	return error;
}

