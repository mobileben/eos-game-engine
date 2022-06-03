/******************************************************************************
 *
 * File: TGATextureDataFormat.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * TGA TextureData Format
 * 
 *****************************************************************************/

#ifndef __TGA_TEXTURE_DATA_FORMAT_H__
#define __TGA_TEXTURE_DATA_FORMAT_H__

#include "Platform.h"
#include "toolkit/TextureDataFormat.h"

#define TGA_IMAGE_TYPE_NONE		0
#define TGA_IMAGE_TYPE_INDEXED	1
#define TGA_IMAGE_TYPE_RGB		2
#define TGA_IMAGE_TYPE_GRAY		4
#define TGA_IMAGE_TYPE_RLE		8

typedef struct
{
	Uint8	ident_size;
	Uint8	paletted;
	Uint8	image_type;
	Sint16	palette_start;
	Sint16	palette_length;
	Uint8	palette_color_depth;
	Sint16	x_start;
	Sint16	y_start;
	Sint16	width;
	Sint16	height;
	Uint8	bpp;
	Uint8	bit_descriptor;
} TGATextureDataFormatHeader;

class TGATextureDataFormat : public TextureDataFormat
{
private:
	TGATextureDataFormatHeader	_header;

	ImageDataBuffer::ImageDataBufferError	_readRGBA_8888IntoTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_readRGB_888IntoTextureDataObject(Uint8* buffer);
	ImageDataBuffer::ImageDataBufferError	_readRGBA_5551IntoTextureDataObject(Uint8* buffer);

	Uint32							_writeHeader(Uint8* buffer);	

public:
	TGATextureDataFormat();
	~TGATextureDataFormat();

	Uint32									length(void);

	//	Case of image buffer (for file and memory images)
	ImageDataBuffer::ImageDataBufferError			importFormat(const Uint8* buffer, Uint32 len);

	//	Case of TextureDataObject
	ImageDataBuffer::ImageDataBufferError			importFormat(TextureDataObject& tex);

	//	Case of image buffer (for file and memory images)
	ImageDataBuffer::ImageDataBufferError			exportFormat(Uint8* buffer, Uint32 len);

	//	Case of TextureDataObject
	inline ImageDataBuffer::ImageDataBufferError	exportFormat(TextureDataObject& tex) { return TextureDataFormat::exportFormat(tex); }
};

#endif /* __TGA_TEXTURE_DATA_FORMAT_H__ */
