/******************************************************************************
 *
 * File: TextureDataObjectConverter.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * Texture Data Object Converter
 * 
 *****************************************************************************/

#ifndef __TEXTURE_DATA_OBJECT_CONVERTER_H__
#define __TEXTURE_DATA_OBJECT_CONVERTER_H__

#include "Platform.h"
#include "toolkit/TextureDataObject.h"

#include <vector>

using namespace std;

typedef struct
{
	Uint8	a; 
	Uint8	r; 
	Uint8	g; 
	Uint8	b;
	Uint32	count;
} ImagePixel;

typedef struct
{
	Uint8	val;
	Uint32	count;
} ChannelCount;

typedef struct
{
	Uint8					r; 
	Uint8					g; 
	Uint8					b; 
	vector<ChannelCount>	a;
} ColorPixel;

class TextureDataObjectConverter
{
public:
	TextureDataObjectConverter() {}
	~TextureDataObjectConverter() {}

	virtual void convert(TextureDataObject& result, TextureDataObject& source, TextureDataObject::TextureDataObjectType new_type) =0;
};

#endif /* __TEXTURE_OBJECT_CONVERTER_H__ */


