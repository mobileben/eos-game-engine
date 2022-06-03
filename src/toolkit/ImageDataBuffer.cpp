/******************************************************************************
 *
 * File: ImageDataBuffer.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * Image Buffer
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/ImageDataBuffer.h"

ImageDataBuffer::ImageDataBuffer()
{
	Sint32	i;

	_type = ImageDataBufferTypeData;
	_encoding = ImageDataBufferEncodingData8;

	_width = _height = 0;

	for (i=0;i<ImageDataBufferChannelMax;i++)
	{
		_channels[i] = NULL;
		_channel_depth[i] = 0;
		_channel_output_order[i] = ImageDataBufferChannelInvalid;
		_channel_mask[i] = 0;
	}
}

ImageDataBuffer::~ImageDataBuffer()
{
	Sint32	i;

	for (i=0;i<ImageDataBufferChannelMax;i++)
	{
		if (_channels[i])
		{
			delete [] _channels[i];
			_channels[i] = NULL;
		}
	}
}

ImageDataBuffer& ImageDataBuffer::operator=(const ImageDataBuffer& ib)
{
	if (this != &ib)
	{
		deallocate();

		if (allocate(ib._width, ib._height, ib._type, ib._encoding) == ImageDataBufferErrorNone)
		{
			Sint32	i;

			for (i=0;i<ImageDataBufferChannelMax;i++)
				setChannel((ImageDataBufferChannel) i, ib._channels[i]);
		}
	}

	return *this;
}

ImageDataBuffer::ImageDataBufferError ImageDataBuffer::allocate(Uint32 width, Uint32 height, ImageDataBuffer::ImageDataBufferType type, ImageDataBuffer::ImageDataBufferEncoding encoding)
{
	ImageDataBufferError 	error = ImageDataBufferErrorNone;
	Sint32				i;	

	//	Purge anything that currently exists
	deallocate();

	_type = type;
	_encoding = encoding;
	_width = width;
	_height = height;

	//	It is mostly the encoding that defines the params for this entity, type is just a informational
	switch (_encoding)
	{
		case ImageDataBufferEncodingData8:
			_channels[ImageDataBufferChannelData] = new Uint8[_width * _height];

			if (_channels[ImageDataBufferChannelData])
			{
				_channel_depth[ImageDataBufferChannelData] = 8;
				_channel_mask[ImageDataBufferChannelData] = 0xFF;
				_channel_output_order[0] = ImageDataBufferChannelData;
			}
			else
				error = ImageDataBufferErrorMemory;
			break;

		case ImageDataBufferEncodingData4:
			_channels[ImageDataBufferChannelData] = new Uint8[_width * _height];

			if (_channels[ImageDataBufferChannelData])
			{
				_channel_depth[ImageDataBufferChannelData] = 4;
				_channel_mask[ImageDataBufferChannelData] = 0x0F;
				_channel_output_order[0] = ImageDataBufferChannelData;
			}
			else
				error = ImageDataBufferErrorMemory;
			break;

		case ImageDataBufferEncodingColorRGBA_8888:
			for (i=0;i<ImageDataBufferChannelColorLast;i++)
			{
				_channels[i] = new Uint8[_width * _height];

				if (_channels[i] == NULL)
				{
					error = ImageDataBufferErrorMemory;
					break;
				}

				_channel_depth[i] = 8;
				_channel_mask[i] = 0xFF;
				_channel_output_order[i] = (ImageDataBufferChannel) i;
			}
			break;

		case ImageDataBufferEncodingColorRGB_888:
			for (i=0;i<ImageDataBufferChannelColorAlpha;i++)
			{
				_channels[i] = new Uint8[_width * _height];

				if (_channels[i] == NULL)
				{
					error = ImageDataBufferErrorMemory;
					break;
				}

				_channel_depth[i] = 8;
				_channel_mask[i] = 0xFF;
				_channel_output_order[i] = (ImageDataBufferChannel) i;
			}
			break;

		case ImageDataBufferEncodingColorRGBA_5551:
			for (i=0;i<ImageDataBufferChannelColorLast;i++)
			{
				_channels[i] = new Uint8[_width * _height];

				if (_channels[i] == NULL)
				{
					error = ImageDataBufferErrorMemory;
					break;
				}

				if (i == ImageDataBufferChannelColorAlpha)
				{
					_channel_depth[i] = 1;
					_channel_mask[i] = 0x01;
				}
				else
				{
					_channel_depth[i] = 8;
					_channel_mask[i] = 0xFF;
				}

				_channel_output_order[i] = (ImageDataBufferChannel) i;
			}
			break;

		case ImageDataBufferEncodingColorRGB_565:
			for (i=0;i<ImageDataBufferChannelColorAlpha;i++)
			{
				_channels[i] = new Uint8[_width * _height];

				if (_channels[i] == NULL)
				{
					error = ImageDataBufferErrorMemory;
					break;
				}

				if (i == ImageDataBufferChannelColorGreen)
				{
					_channel_depth[i] = 6;
					_channel_mask[i] = 0x3F;
				}
				else
				{
					_channel_depth[i] = 5;
					_channel_mask[i] = 0x1F;
				}

				_channel_output_order[i] = (ImageDataBufferChannel) i;
			}
			break;

		case ImageDataBufferEncodingColorRGBA_4444:
			for (i=0;i<ImageDataBufferChannelColorLast;i++)
			{
				_channels[i] = new Uint8[_width * _height];

				if (_channels[i] == NULL)
				{
					error = ImageDataBufferErrorMemory;
					break;
				}

				_channel_depth[i] = 4;
				_channel_mask[i] = 0x0F;
				_channel_output_order[i] = (ImageDataBufferChannel) i;
			}
			break;

		default:
			error = ImageDataBufferErrorIllegalDefinition;
			break;
	};

	if (error != ImageDataBufferErrorNone)
		deallocate();
	else
	{
		//	Clear out all our buffers
		for (i=0;i<ImageDataBufferChannelMax;i++)
		{
			if (_channels[i])
				memset(_channels[i], 0, _width * _height);
		}
	}

	return error;
}

void ImageDataBuffer::deallocate(void)
{
	Sint32	i;

	_type = ImageDataBufferTypeData;
	_encoding = ImageDataBufferEncodingData8;

	_width = _height = 0;

	for (i=0;i<ImageDataBufferChannelMax;i++)
	{
		if (_channels[i])
		{
			delete [] _channels[i];
			_channels[i] = NULL;
		}

		_channel_depth[i] = 0;
		_channel_output_order[i] = ImageDataBufferChannelInvalid;
		_channel_mask[i] = 0;
	}
}

Uint8* ImageDataBuffer::setChannel(ImageDataBuffer::ImageDataBufferChannel channel, Uint8* buffer)
{
	if (_width && _height && _channels[channel])
	{
		memcpy(_channels[channel], buffer, _width * _height);
	}

	return _channels[channel];
}

