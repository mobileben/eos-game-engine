/******************************************************************************
 *
 * File: TextureDataImage.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * © 2008 2n Productions, All Rights Reserved.
 *
 * Texture Data Image
 * 
 *****************************************************************************/

#include "Platform.h"
#include "toolkit/ImageDataBuffer.h"

class TextureDataImage : public ImageDataBuffer
{
public:

private:

public:
	TextureDataImage();
	~TextureDataImage();

	TextureDataImage&		operator=(const TextureDataImage& ti);

	ImageDataBufferError	configure(ImageDataBufferEncoding encoding, Uint32 width, Uint32 height);
};
