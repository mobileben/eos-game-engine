/******************************************************************************
 *
 * File: tga2gl.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * � 2008 2n Productions, All Rights Reserved.
 *
 * Converts TGA files to gl format. This will either be BIN or TEXT depending
 * on the flags 
 * 
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "File.h"
#include "toolkit/TGATextureDataFormat.h"
#include "toolkit/GLTextureDataFormat.h"

typedef unsigned char 	GLubyte;
typedef int 			GLint;
typedef unsigned short 	GLushort;

typedef struct
{
    unsigned char	ident_size;
    unsigned char	paletted;
    unsigned char	image_type;
    short			palette_start;
    short			palette_length;
    unsigned char	palette_color_depth;
    short			x_start;
    short			y_start;
    short			width;
    short			height;
    unsigned char	bpp;
    unsigned char	bit_descriptor;
} TGAHeader;

typedef enum
{
	ConversionArgRGBA8888 = 0,
	ConversionArgRGB888,
	ConversionArgRGBA4444,
	ConversionArgRGBA5551,
	ConversionArgRGB565,
	ConversionArgP8RGBA8888,
	ConversionArgP4RGBA4444,
	ConversionArgLast
} ConversionArg;

char*	_conversion_name[ConversionArgLast] = 
{
	"-rgba8888",
	"-rgb888",
	"-rgba4444",
	"-rgba5551",
	"-rgb565",
	"-p8rgba8888",
	"-p4rgba4444",
};

TextureDataObject::TextureDataObjectType	_conversion_texture_object_types[ConversionArgLast] = 
{
	TextureDataObject::TextureDataObjectTypeRGBA_8888,
	TextureDataObject::TextureDataObjectTypeRGB_888,
	TextureDataObject::TextureDataObjectTypeRGBA_4444,
	TextureDataObject::TextureDataObjectTypeRGBA_5551,
	TextureDataObject::TextureDataObjectTypeRGB_565,
	TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888,
	TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444,
};

char* _app_name = "tga2gl";

void copyright(void)
{
	fprintf(stdout, "%s\n", _app_name);
	fprintf(stdout, "Copyright (C) 2008 by 2n Productions, All Rights Reserved.\n");
}

void usage(void)
{
	fprintf(stdout, "Usage: %s -rgba8888|-rgb888|-rgba5551|-rgb565|-rgba4444|-p8rgba8888|-p4rgba4444 [-text] infile outfile.\n", _app_name);
}

int readByte(unsigned char* dst, unsigned char* src, size_t num)
{
	int	i;

	for(i=0;i<(int) num;i++)
	{
		dst[i] = src[i];
	}

	return (int) num;
}

int readShort(unsigned short* dst, unsigned char* src, size_t num)
{
	int	i;

	for(i=0;i< (int)num * 2;i += 2)
	{
		dst[i/2] = (src[i]) | (src[i+1] << 8);
	}

	return (int) (num * 2);
}

int readByte(unsigned char* buffer)
{
	return *buffer;
}

int readUInt(unsigned char* buffer)
{
	unsigned int	val = 0;

	val = buffer[3] | (buffer[2] << 8) | (buffer[1] << 16) | (buffer[0] << 24);

	return val;
}

void flipRB(unsigned char* out_buffer, unsigned char* in_buffer)
{
	out_buffer[3] = in_buffer[0];
	out_buffer[2] = in_buffer[1];
	out_buffer[1] = in_buffer[2];
	out_buffer[0] = in_buffer[3];

}

unsigned char* instantiateTextureDataTGAFromFileImage(unsigned char* image, short& width, short& height)
{
	char 		headerLength = 0;                  
	char 		imageType = 0;             
	char 		bits = 0;                      
	int 		format= 0;                  
	int			type = 0;
	int 		lineWidth = 0;                          
	int			curr_ptr = 0;
	
	headerLength = image[curr_ptr];
	curr_ptr++;

	curr_ptr++;

	imageType = image[curr_ptr];
	curr_ptr++;

	curr_ptr += 9;

	//	This may need to be fixed for endianess
	curr_ptr += readShort((unsigned short*) &width, &image[curr_ptr], 1);
	curr_ptr += readShort((unsigned short*) &height, &image[curr_ptr], 1);

	bits = image[curr_ptr];
	curr_ptr++;

	curr_ptr += headerLength + 1;

	GLubyte *buffer = NULL;

	if(imageType != 10)
	{
		// Support for LUMINANCE and RGBA textures
		if((bits == 24)||(bits == 32)) 
		{
			format = bits >> 3; 
			lineWidth = format * width;

			buffer = (GLubyte*) malloc(sizeof(GLubyte) * lineWidth * height);

			if(buffer)
			{
				for(int y = 0; y < height; y++)
				{
					GLubyte *line = &buffer[lineWidth * y];             

					curr_ptr += readByte(line, &image[curr_ptr], lineWidth);

					if(format!= 1)
					{
						for(int i=0;i<lineWidth ; i+=format) //swap R and B because TGA are stored in BGR format
						{
							int temp  = line[i];
							line[i]   = line[i+2];
							line[i+2] = temp;
						}
					}
				}
			}
			else
				return NULL;
		}
		else if(bits == 16)
		{
			format = bits >> 3; 
			lineWidth = format * width;

			buffer = (GLubyte*) malloc(sizeof(GLubyte) * lineWidth * height);

			if(buffer)
			{
				for(int y = 0; y < height; y++)
				{
					GLushort *line = (GLushort*) &buffer[lineWidth * y];             

					curr_ptr += readShort((unsigned short*) line, &image[curr_ptr], lineWidth / 2);

					if(format!= 1)
					{
						for(int i=0;i<lineWidth / 2 ; i+=1) //swap R and B because TGA are stored in BGR format
						{
							GLushort a = (line[i] & 0x8000) >> 15;
							GLushort r = (line[i] & 0x001F) >> 0;
							GLushort g = (line[i] & 0x03E0) >> 5;
							GLushort b = (line[i] & 0x7C00) >> 10;

							line[i] = (a << 0) | (b << 11) | (g << 6) | (r << 1);
						}
					}
				}
			}
		}
		else
		{
			if(buffer != NULL)
			{
				free(buffer);
			}

			return NULL;
		}
	}


	return buffer;
}

TextureDataObject::TextureDataObjectType _conversion_op = TextureDataObject::TextureDataObjectTypeUnsupported;

bool parseForTextureDataObjectType(char* type)
{
	bool	found = false;
	int		i;

	for (i=0;i<ConversionArgLast;i++)
	{
		if (!strcmp(type, _conversion_name[i]))
		{
			_conversion_op = _conversion_texture_object_types[i];
			found = true;
			break;
		}
	}

	return found;
}

int main(int argc, char* argv[])
{
#ifdef _PLATFORM_MAC
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
#endif /* _PLATFORM_MAC */
	
    int					result = 0;
    long				filesize;
	bool				ok = false;
	bool				export_binary = true;
	char*				infilename;
	char*				outfilename;
	FileDescriptor		desc;
	File				file;
	TGATextureDataFormat	tga;

	copyright();

    if(argc == 4)
    {
		infilename = argv[2];
		outfilename = argv[3];

		if (parseForTextureDataObjectType(argv[1]))
		{
			ok = true;
		}
	}
    else if(argc == 5)
    {
		infilename = argv[3];
		outfilename = argv[4];
		export_binary = false;

		if(!strcmp("-text", argv[1]))
		{
			if (parseForTextureDataObjectType(argv[2]))
			{
				ok = true;
			}
		}
		else if (!strcmp("-text", argv[2]))
		{
			if (parseForTextureDataObjectType(argv[1]))
			{
				ok = true;
			}
		}
	}

	if(ok)
	{
		TextureDataObject tex;
		Uint8*	new_buffer;

		desc.setFilename(infilename);
		desc.setFileAccessType(FileAccessTypeReadOnly);

		file.setFileDescriptor(desc);

		if (file.open() == EOSErrorNone)
		{
			filesize = file.length();

			new_buffer = new Uint8[filesize];

			if (new_buffer)
			{
				if (file.readUint8(new_buffer, filesize) == EOSErrorNone)
				{
					GLTextureDataFormat	gl;
					TextureDataObject new_tex;
					Uint8*			tbuf;

					if (_conversion_op == TextureDataObject::TextureDataObjectTypePAL8_RGBA_8888 || _conversion_op == TextureDataObject::TextureDataObjectTypePAL4_RGBA_4444)
					{
						fprintf(stderr, "WARNING: This tool does not perform any real color reduction!!!\nThis should be an already reduced image!\n");
					}

					tga.importFormat(new_buffer, filesize);
					tga.exportFormat(tex);

					tex.convert(new_tex, _conversion_op);

					gl.importFormat(new_tex);

#ifdef _PLATFORM_MAC
					tbuf = (Uint8*) malloc(gl.length());
#else
					tbuf = new Uint8[gl.length()];
#endif
					
					if (tbuf)
					{
						file.close();

						desc.setFilename(outfilename);
						desc.setFileAccessType(FileAccessTypeWriteOnly);

						file.setFileDescriptor(desc);

						gl.exportFormat(tbuf, gl.length());

						if (file.open() == EOSErrorNone)
						{
							file.writeUint8(tbuf, gl.length());
						}
						
#ifndef _PLATFORM_MAC
						delete tbuf;
#endif /* _PLATFORM_MAC */
					}
				}

				delete [] new_buffer;
			}
			else
			{
				result = -2;
				fprintf(stderr, "Could not open file %s\n", outfilename);
			}

			file.close();
		}
		else
		{
			result = -2;
			fprintf(stderr, "Could not open file %s\n", infilename);
		}
	}
	else
	{
		result = -1;
		usage();
	}

#ifdef _PLATFORM_MAC
	[pool release];
#endif /* _PLATFORM_MAC */
	
    return result;
}


