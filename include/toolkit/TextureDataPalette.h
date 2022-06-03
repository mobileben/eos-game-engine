/******************************************************************************
 *
 * File: TexturePalette.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * Texture Palette
 * 
 *****************************************************************************/

#ifndef __TEXTURE_DATA_PALETTE_H__
#define __TEXTURE_DATA_PALETTE_H__

#include "Platform.h"
#include "toolkit/ImageDataBuffer.h"

class TextureDataPalette : public ImageDataBuffer
{
public:

private:
	Uint32			_num_colors;

public:
	TextureDataPalette();
	~TextureDataPalette();

	TextureDataPalette&		operator=(const TextureDataPalette& tp);

	inline Uint32		getNumColors(void) const { return _num_colors; }

	ImageDataBufferError	configure(ImageDataBufferEncoding encoding, Uint32 num_color);
};

#endif /* __TEXTURE_DATA_PALETTE_H__ */
