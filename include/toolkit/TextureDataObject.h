/******************************************************************************
 *
 * File: TextureDataObject.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * Texture Data Object
 * 
 *****************************************************************************/

#ifndef __TEXTURE_DATA_OBJECT_H__
#define __TEXTURE_DATA_OBJECT_H__

#include "Platform.h"
#include "toolkit/TextureDataImage.h"
#include "toolkit/TextureDataPalette.h"

class TextureDataObject
{
public:
	typedef enum
	{
		TextureDataObjectTypeRGBA_8888,
		TextureDataObjectTypeRGB_888,
		TextureDataObjectTypeRGBA_5551,
		TextureDataObjectTypeRGB_565,
		TextureDataObjectTypeRGBA_4444,

		TextureDataObjectTypePAL8_RGBA_8888,
		TextureDataObjectTypePAL8_RGB_888,
		TextureDataObjectTypePAL8_RGBA_5551,
		TextureDataObjectTypePAL8_RGB_565,
		TextureDataObjectTypePAL8_RGBA_4444,

		TextureDataObjectTypePAL4_RGBA_8888,
		TextureDataObjectTypePAL4_RGB_888,
		TextureDataObjectTypePAL4_RGBA_5551,
		TextureDataObjectTypePAL4_RGB_565,
		TextureDataObjectTypePAL4_RGBA_4444,

		TextureDataObjectTypeUnsupported = -1
	} TextureDataObjectType;

private:
	friend class TextureDataObjectConverter;

	TextureDataObjectType	_type;
	TextureDataImage		_image;
	TextureDataPalette		_palette;

	ImageDataBuffer::ImageDataBufferError			_convertRGBA_8888(TextureDataObject& result, TextureDataObjectType new_type);
	ImageDataBuffer::ImageDataBufferError			_convertRGBA_8888ToRGBA_5551(TextureDataObject& result);
	ImageDataBuffer::ImageDataBufferError			_convertRGBA_8888ToRGBA_4444(TextureDataObject& result);
	ImageDataBuffer::ImageDataBufferError			_convertRGBA_8888ToRGB_565(TextureDataObject& result);

	ImageDataBuffer::ImageDataBufferError			_convertRGBA_8888ToPAL8_RGBA_8888(TextureDataObject& result);
	ImageDataBuffer::ImageDataBufferError			_convertRGBA_8888ToPAL4_RGBA_4444(TextureDataObject& result);

	ImageDataBuffer::ImageDataBufferError			_convertRGBA_5551(TextureDataObject& result, TextureDataObjectType new_type);
	ImageDataBuffer::ImageDataBufferError			_convertRGBA_5551ToRGBA_8888(TextureDataObject& result);

public:
	TextureDataObject();
	TextureDataObject(TextureDataObjectType type);

	~TextureDataObject();

	TextureDataObject&							operator=(const TextureDataObject& to);

	inline TextureDataObjectType				getTextureDataObjectType(void) const { return _type; }

	inline Uint32							getPaletteNumColors(void) const { return _palette.getNumColors(); }
	inline ImageDataBuffer::ImageDataBufferEncoding	getPaletteEncoding(void) const { return _palette.getBufferEncoding(); }

	inline ImageDataBuffer::ImageDataBufferEncoding	getImageEncoding(void) const { return _image.getBufferEncoding(); }
	inline Uint32							getImageWidth(void) const { return _image.getWidth(); }
	inline Uint32							getImageHeight(void) const { return _image.getHeight(); }

	const TextureDataImage*					getTextureDataImage(void) const { return &_image; }
	const TextureDataPalette*				getTextureDataPalette(void) const { return &_palette; }

	ImageDataBuffer::ImageDataBufferError			configure(TextureDataObjectType type, Uint32 width, Uint32 height);

	Uint8*									getPaletteChannel(ImageDataBuffer::ImageDataBufferChannel channel);
	Uint8*									setPaletteChannel(ImageDataBuffer::ImageDataBufferChannel channel, Uint8* buffer);

	Uint8*									getImageChannel(ImageDataBuffer::ImageDataBufferChannel channel);
	Uint8*									setImageChannel(ImageDataBuffer::ImageDataBufferChannel channel, Uint8* buffer);

	ImageDataBuffer::ImageDataBufferError			convert(TextureDataObject& result, TextureDataObjectType new_type);
};

#endif /* __TEXTURE_DATA_OBJECT_H__*/
