/******************************************************************************
 *
 * File: ImageDataBuffer.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * Image Buffer
 * 
 *****************************************************************************/

#ifndef __IMAGE_DATA_BUFFER_H__
#define __IMAGE_DATA_BUFFER_H__

#include "Platform.h"

class ImageDataBuffer
{
public:
	typedef enum
	{
		ImageDataBufferErrorNone = 0,
		ImageDataBufferErrorMemory,
		ImageDataBufferErrorIllegalChannel,
		ImageDataBufferErrorIllegalDefinition,
		ImageDataBufferErrorUnsupported
	} ImageDataBufferError;

	typedef enum
	{
		ImageDataBufferTypeData = 0,
		ImageDataBufferTypeColor,
	} ImageDataBufferType;

	typedef enum
	{
		ImageDataBufferEncodingData8 = 0,
		ImageDataBufferEncodingData4,
		ImageDataBufferEncodingColorRGBA_8888,
		ImageDataBufferEncodingColorRGB_888,
		ImageDataBufferEncodingColorRGBA_5551,
		ImageDataBufferEncodingColorRGB_565,
		ImageDataBufferEncodingColorRGBA_4444,
	} ImageDataBufferEncoding;

	typedef enum
	{
		ImageDataBufferChannelInvalid = -1,

		ImageDataBufferChannelData = 0,

		ImageDataBufferChannelColorRed = 0,
		ImageDataBufferChannelColorGreen,
		ImageDataBufferChannelColorBlue,
		ImageDataBufferChannelColorAlpha,
		ImageDataBufferChannelColorLast,

		ImageDataBufferChannelMax = 4
	} ImageDataBufferChannel;

private:
	ImageDataBufferType		_type;
	ImageDataBufferEncoding	_encoding;

	Uint32				_channel_depth[ImageDataBufferChannelMax];
	ImageDataBufferChannel	_channel_output_order[ImageDataBufferChannelMax];
	Uint32				_channel_mask[ImageDataBufferChannelMax];

	Uint32				_width;
	Uint32				_height;

	Uint8*				_channels[ImageDataBufferChannelMax];

public:
	ImageDataBuffer();
	~ImageDataBuffer();

	ImageDataBuffer&		operator=(const ImageDataBuffer& ib);

	inline ImageDataBufferType		getBufferType(void) const { return _type; }
	inline ImageDataBufferEncoding	getBufferEncoding(void) const { return _encoding; }
	inline Uint32				getWidth(void) const { return _width; }
	inline Uint32				getHeight(void) const { return _height; }

	ImageDataBufferError	allocate(Uint32 width, Uint32 height, ImageDataBufferType type, ImageDataBufferEncoding encoding);

	void				deallocate(void);

	void				clear(void);

	inline Uint8*		getChannel(ImageDataBufferChannel channel) { return _channels[channel]; }
	Uint8*				setChannel(ImageDataBufferChannel channel, Uint8* buffer);
};

#endif /* __IMAGE_DATA_BUFFER_H__ */
