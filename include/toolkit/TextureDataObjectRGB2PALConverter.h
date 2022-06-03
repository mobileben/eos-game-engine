/******************************************************************************
 *
 * File: TextureDataObjectRGB2PALConverter.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * Texture Data Object Converter
 * 
 *****************************************************************************/

#ifndef __TEXTURE_DATA_OBJECT_RGB_2_PAL_CONVERTER_H__
#define __TEXTURE_DATA_OBJECT_RGB_2_PAL_CONVERTER_H__

#include "Platform.h"
#include "toolkit/TextureDataObjectConverter.h"

class TextureDataObjectRGB2PALConverter : public TextureDataObjectConverter
{
private:
	vector<ImagePixel>						_basic_pixel_profile;

	Uint8									_findColorIndex(Uint8 a, Uint8 r, Uint8 g, Uint8 b);
	bool 									_addUniquePixelToBasicProfile(ImagePixel& pixel);
	void									_buildPixelProfile(TextureDataObject& src, Uint8 alpha_shift, Uint8 shift_op = 0);

	void									_convertToP8RGBA8888(TextureDataObject& result, TextureDataObject& source);
	void									_convertToP4RGBA4444(TextureDataObject& result, TextureDataObject& source);

public:
	TextureDataObjectRGB2PALConverter();
	~TextureDataObjectRGB2PALConverter();

	void convert(TextureDataObject& result, TextureDataObject& source, TextureDataObject::TextureDataObjectType new_type);
};

#endif /* __TEXTURE_DATA_OBJECT_RGB_2_PAL_CONVERTER_H__ */
