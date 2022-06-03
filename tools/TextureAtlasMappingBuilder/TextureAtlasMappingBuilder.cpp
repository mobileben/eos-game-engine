/******************************************************************************
 *
 * File: TextureAtlasMappingBuilder.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Texture Atlas Mapping Builder
 * 
 *****************************************************************************/

#include "Platform.h"
#include "TexAtlasMappingXML.h"
#include "toolkit/TextureAtlasXML.h"
#include "File.h"
#include "SpriteSet.h"

char*	_app_name = "texatlasmappingbuilder";

TexAtlasMappingXMLParser	_xmlParser;
Sint32	_versionMajor = 1;
Sint32	_versionMinor = 0;

TextureAtlasXMLParser	_master;
Uint32					_numAtlases = 0;
TextureAtlasXMLParser*	_atlases = NULL;
Uint32					_mappingsSize = 0;
Uint8*					_mappings = NULL;
Uint32*					_newSubTexID = NULL;

void copyright(void)
{
	fprintf(stdout, "%s\n", _app_name);
	fprintf(stdout, "Version %d.%d\n", _versionMajor, _versionMinor);
	fprintf(stdout, "Copyright (C) 2009 by 2n Productions, All Rights Reserved.\n");
	fprintf(stdout, "EOS Engine\n");
}

void usage(void)
{
	fprintf(stdout, "Usage: %s xml output\n", _app_name);
}

Uint32	findSubTextureInAtlasPool(Char* subtex, Uint32& newID)
{
	Uint32					atlas = 0xFFFFFFFF;
	Uint32					i, j;

	for (i=0;i<_numAtlases;i++)
	{
		for (j=0;j<_atlases[i].getNumSubTex();j++)
		{
			if (!strcmp(_atlases[i].getSubTexAtIndex(j)->getName()->getString(), subtex))
			{
				atlas = i;
				newID = j;
				break;
			}
		}
	}

	return atlas;
}

EOSError buildMappings(void)
{
	EOSError 	error = EOSErrorNone;
	Uint32		i;
	Uint32		atlas;

	_mappings = new Uint8[_master.getNumSubTex()];
	_newSubTexID = new Uint32[_master.getNumSubTex()];

	if (_mappings && _newSubTexID)
	{
		_mappingsSize = _master.getNumSubTex();

		for (i=0;i<_master.getNumSubTex();i++)
		{
			atlas = findSubTextureInAtlasPool(_master.getSubTexAtIndex(i)->getName()->getString(), _newSubTexID[i]);

			if (atlas != 0xFFFFFFFF)
				_mappings[i] = (Uint8) atlas;
			else
			{
				fprintf(stderr, "Could not find matching subtex %s\n", _master.getSubTexAtIndex(i)->getName()->getString());
				error = EOSErrorResourceDoesNotExist;
			}
		}
	}
	else
		error = EOSErrorNone;

	return error;
}

int main(int argc, char* argv[])
{
	int		error = 0;
	Uint32	i;

	copyright();

	if (argc == 3)
	{
		error = _xmlParser.parse(argv[1]);

		if (error == EOSErrorNone)
		{
			error = _master.parse(_xmlParser.getMaster()->getString());

			if (error == EOSErrorNone)
			{
				if (_xmlParser.getNumAtlases())
				{
					_atlases = new TextureAtlasXMLParser[_xmlParser.getNumAtlases()];

					if (_atlases)
					{
						_numAtlases = _xmlParser.getNumAtlases();

						for (i=0;i<_xmlParser.getNumAtlases();i++)
						{
							error = _atlases[i].parse(_xmlParser.getAtlasAtIndex(i)->getString());

							if (error != EOSErrorNone)
								break;
						}

						if (error == EOSErrorNone)
							error = buildMappings();

						//	Export our table
						if (error == EOSErrorNone)
						{
							FileDescriptor 	desc;
							File			file;

							desc.setFilename(argv[2]);
							desc.setFileAccessType(FileAccessTypeWriteOnly);

							file.setFileDescriptor(desc);

							error = file.open();

							if (error == EOSErrorNone)
							{
								Uint32	totalsize = 0;
								Uint8*	image;

								totalsize = _mappingsSize + _mappingsSize * sizeof(Uint32) + sizeof(SpriteSetTextureAtlasMappingHeader);
								totalsize += strlen(_xmlParser.getName()->getString()) + 1;

								image = new Uint8[totalsize];

								if (image)
								{
									SpriteSetTextureAtlasMappingHeader*	header = (SpriteSetTextureAtlasMappingHeader*) image;

									memset(image, 0, totalsize);

									header->endian = 0x01020304;
									header->version = 0;
									header->nameOffset = _mappingsSize + _mappingsSize * sizeof(Uint32) + sizeof(SpriteSetTextureAtlasMappingHeader);
									header->numAtlasMappings = _mappingsSize + _mappingsSize * sizeof(Uint32);
									header->atlasMappings = sizeof(SpriteSetTextureAtlasMappingHeader);
									header->subTexIDMappings = header->atlasMappings + _mappingsSize;

									memcpy(&image[header->atlasMappings], _mappings, _mappingsSize);
									memcpy(&image[header->subTexIDMappings], _newSubTexID, _mappingsSize * sizeof(Uint32));

									strcpy((Char*) &image[header->nameOffset], _xmlParser.getName()->getString());

									file.writeUint8(image, totalsize);
								}
								else
									error = EOSErrorNoMemory;

								file.close();
							}
						}
					}
					else
						error = EOSErrorNoMemory;
				}
				else
					error = -2;
			}
		}
	}
	else
	{
		error = -1;
		usage();
	}

	if (_mappings)
		delete _mappings;

	if (_newSubTexID)
		delete _newSubTexID;

	if (_atlases)
		delete [] _atlases;

	return error;
}

