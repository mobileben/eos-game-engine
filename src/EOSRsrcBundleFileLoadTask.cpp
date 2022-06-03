/******************************************************************************
 *
 * File: EOSRsrcBundleFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * EOS Rsrc Bundle File Load Task Base Class
 * 
 *****************************************************************************/

#include "Platform.h"
#include "EOSRsrcBundleFileLoadTask.h"
#include "TaskManager.h"
#include "Texture.h"
#include "App.h"

EOSRsrcBundleFileLoadTask::EOSRsrcBundleFileLoadTask()
{
}

EOSRsrcBundleFileLoadTask::~EOSRsrcBundleFileLoadTask()
{
}

EOSError EOSRsrcBundleFileLoadTask::prologue(void)
{
	return FileLoadTask::prologue();
}

EOSError EOSRsrcBundleFileLoadTask::epilogue(void)
{
	EOSError							error = EOSErrorNone;
	Uint32								i, j;
	EOSRsrcBundleDatabaseHeader*		header;
	Endian								endian;
	Char*								names;
	Char*								name;
	Uint8*								dataPool;
	Uint8*								data;
	EOSRsrcBundleTextureExport*			textures;
	EOSRsrcBundleTextureAtlasExport*	atlases;
	EOSRsrcBundleSpriteExport*			sprites;
	EOSRsrcBundleSpriteAnimExport*		anims;
	EOSRsrcBundleSpriteTextureAtlasMappingExport*	mappings;
	Uint32*								nameOffsets;
	Texture*							tex;
	TextureAtlas*						atlas;
	SpriteSet*							sprite;
	SpriteAnimSet*						anim;
	SpriteSetTextureAtlasMapping*		mapping;
#ifdef _USE_PVR_OPENGLES
	PVRTHeader*							pvrtHeader;
#endif /* _USE_PVR_OPENGLES */

	AssertWDesc(getTaskManager() != NULL, "Task Manager is NULL.");

	if (getTaskManager())
	{
		AssertWDesc(getTaskManager()->getApp() != NULL, "App is NULL.");

		if (getTaskManager()->getApp())
		{
			header = (EOSRsrcBundleDatabaseHeader*) _buffer;

			if (header->endian == 0x04030201)
				endian.switchEndian();

			names = (Char*) (_buffer + endian.swapUint32(header->names));
			dataPool = (Uint8*) (_buffer + endian.swapUint32(header->data));

			textures = (EOSRsrcBundleTextureExport*) (_buffer + endian.swapUint32(header->textures));

			for (i=0;i<endian.swapUint32(header->numTextures);i++)
			{
				name = names + endian.swapUint32(textures[i].nameOffset);

				tex = _appRefPtr->getTextureManager()->findTextureFromName(name);

				if (tex == NULL)
					tex = _appRefPtr->getTextureManager()->getFreeTexture();

				if (tex)
				{
					data = (Uint8*) (dataPool + endian.swapUint32(textures[i].dataOffset));

					switch (endian.swapUint32(textures[i].colorFormat))
					{
						case TextureFormatTGA:
#ifdef _USE_OPENGL
							getTaskManager()->getApp()->getGfx()->lockGLContext();
#endif /* _USE_OPENGL */
							error = tex->createFromTGAImage((Uint8*) data, getFileSize(), endian.swapUint32(textures[i].texState));
#ifdef _USE_OPENGL
							getTaskManager()->getApp()->getGfx()->unlockGLContext();
#endif /* _USE_OPENGL */
							break;

						case TextureFormatGL_RGB_888:
						case TextureFormatGL_RGBA_8888:
						case TextureFormatGL_RGB_565:
						case TextureFormatGL_RGBA_5551:
						case TextureFormatGL_RGBA_4444:
#ifdef _USE_OPENGL
							getTaskManager()->getApp()->getGfx()->lockGLContext();
							error = tex->createFromGLImage((GLubyte*) data, (GLint) endian.swapUint32(textures[i].width), (GLint) endian.swapUint32(textures[i].height), tex->convertTextureFormatToGLFormat((TextureFormat) endian.swapUint32(textures[i].colorFormat)), tex->convertTextureFormatToGLType((TextureFormat) endian.swapUint32(textures[i].colorFormat)), endian.swapUint32(textures[i].texState));
							getTaskManager()->getApp()->getGfx()->unlockGLContext();
#endif /* _USE_OPENGL */
							break;

#ifdef _USE_OPENGL
						case TextureFormatGL_P8_RGBA_8888:
							getTaskManager()->getApp()->getGfx()->lockGLContext();

							error = tex->createPALTextureFromGLImage((GLubyte*) data, (GLint) endian.swapUint32(textures[i].width), (GLint) endian.swapUint32(textures[i].height), 256, tex->convertTextureFormatToGLFormat((TextureFormat) endian.swapUint32(textures[i].colorFormat)), endian.swapUint32(textures[i].texState));

							getTaskManager()->getApp()->getGfx()->unlockGLContext();
							break;

						case TextureFormatGL_P4_RGBA_4444:
							getTaskManager()->getApp()->getGfx()->lockGLContext();

							error = tex->createPALTextureFromGLImage((GLubyte*) data, (GLint) endian.swapUint32(textures[i].width), (GLint) endian.swapUint32(textures[i].height), 16, tex->convertTextureFormatToGLFormat((TextureFormat) endian.swapUint32(textures[i].colorFormat)), endian.swapUint32(textures[i].texState));

							getTaskManager()->getApp()->getGfx()->unlockGLContext();
							break;

#ifdef _USE_PVR_OPENGLES
						case TextureFormatRAWPVRTC_4BPP_RGBA:
						case TextureFormatRAWPVRTC_4BPP_RGB:
							pvrtHeader = (PVRTHeader*) data;

							getTaskManager()->getApp()->getGfx()->lockGLContext();

							error = tex->createPALTextureFromGLImage((GLubyte*) data, (GLint) pvrtHeader->width, (GLint) pvrtHeader->height, 16, tex->convertTextureFormatToGLFormat((TextureFormat) endian.swapUint32(textures[i].colorFormat)), endian.swapUint32(textures[i].texState));

							getTaskManager()->getApp()->getGfx()->unlockGLContext();
							break;

						case TextureFormatPVRTC_4BPP_RGBA:
						case TextureFormatPVRTC_4BPP_RGB:
							pvrtHeader = (PVRTHeader*) data;

							getTaskManager()->getApp()->getGfx()->lockGLContext();

							error = tex->createPALTextureFromGLImage((GLubyte*) data + sizeof(PVRTHeader), (GLint) pvrtHeader->width, (GLint) pvrtHeader->height, 16, tex->convertTextureFormatToGLFormat((TextureFormat) endian.swapUint32(textures[i].colorFormat)), endian.swapUint32(textures[i].texState));

							getTaskManager()->getApp()->getGfx()->unlockGLContext();
							break;

						case TextureFormatPVRTC_2BPP_RGBA:
						case TextureFormatPVRTC_2BPP_RGB:
							pvrtHeader = (PVRTHeader*) data;

							getTaskManager()->getApp()->getGfx()->lockGLContext();

							error = tex->createPALTextureFromGLImage((GLubyte*) data + sizeof(PVRTHeader), (GLint) pvrtHeader->width, (GLint) pvrtHeader->height, 4, tex->convertTextureFormatToGLFormat((TextureFormat) endian.swapUint32(textures[i].colorFormat)), endian.swapUint32(textures[i].texState));

							getTaskManager()->getApp()->getGfx()->unlockGLContext();
							break;

						case TextureFormatRAWPVRTC_2BPP_RGBA:
						case TextureFormatRAWPVRTC_2BPP_RGB:
							pvrtHeader = (PVRTHeader*) data;

							getTaskManager()->getApp()->getGfx()->lockGLContext();

							error = tex->createPALTextureFromGLImage((GLubyte*) data, (GLint) pvrtHeader->width, (GLint) pvrtHeader->height, 4, tex->convertTextureFormatToGLFormat((TextureFormat) endian.swapUint32(textures[i].colorFormat)), endian.swapUint32(textures[i].texState));

							getTaskManager()->getApp()->getGfx()->unlockGLContext();
							break;
#endif /* _USE_PVR_OPENGLES */

#endif /* _USE_OPENGL */

						default:
							AssertWDesc(1 == 0, "Unsupported color format.");
							error = EOSErrorUnsupported;
							break;

					}

					if (error == EOSErrorNone)
					{
						tex->setRefID(InvalidObjectID);
						tex->setName(name);
					}
				}
				else
					error = EOSErrorResourceNotAvailable;

				if (error != EOSErrorNone)
					break;
			}

			if (error == EOSErrorNone)
			{
				atlases = (EOSRsrcBundleTextureAtlasExport*) (_buffer + endian.swapUint32(header->textureAtlases));

				for (i=0;i<endian.swapUint32(header->numTextureAtlases);i++)
				{
					name = names + endian.swapUint32(atlases[i].nameOffset);

					atlas = _appRefPtr->getTextureAtlasManager()->getFreeTextureAtlas();
					data = (Uint8*) (dataPool + endian.swapUint32(atlases[i].dataOffset));

					AssertWDesc(atlas != NULL, "Cloud not allocate TextureAtlas.");

					if (atlas)
					{
						if (atlases[i].texNameOffset != 0xFFFFFFFF)
							tex = _appRefPtr->getTextureManager()->findTextureFromName(names + endian.swapUint32(atlases[i].texNameOffset));
						else
							tex = NULL;

						error = atlas->create(InvalidObjectID, name, tex, (Uint8*) data, endian.swapUint32(atlases[i].dataSize));
					}
					else
						error = EOSErrorResourceNotAvailable;

					if (error != EOSErrorNone)
						break;
				}
			}

			if (error == EOSErrorNone)
			{
				mappings = (EOSRsrcBundleSpriteTextureAtlasMappingExport*) (_buffer + endian.swapUint32(header->spriteMappings));

				for (i=0;i<endian.swapUint32(header->numSpriteMappings);i++)
				{
					name = names + endian.swapUint32(mappings[i].nameOffset);

					mapping = _appRefPtr->getSpriteSetManager()->getFreeSpriteSetTextureAtlasMapping();
					data = (Uint8*) (dataPool + endian.swapUint32(mappings[i].dataOffset));

					AssertWDesc(mapping != NULL, "Could not allocate SpriteSetTextureAtlasMapping");

					if (mapping)
					{
						error = mapping->create(InvalidObjectID, name, data, endian.swapUint32(mappings[i].dataSize));
					}
					else
						error = EOSErrorResourceNotAvailable;

					if (error != EOSErrorNone)
						break;
				}
			}

			if (error == EOSErrorNone)
			{
				sprites = (EOSRsrcBundleSpriteExport*) (_buffer + endian.swapUint32(header->sprites));

				for (i=0;i<endian.swapUint32(header->numSprites);i++)
				{
					name = names + endian.swapUint32(sprites[i].nameOffset);

					sprite = _appRefPtr->getSpriteSetManager()->getFreeSpriteSet();
					data = (Uint8*) (dataPool + endian.swapUint32(sprites[i].dataOffset));

					AssertWDesc(sprite != NULL, "Could not allocate SpriteSet.");

					if (sprite)
					{
						error = sprite->create(InvalidObjectID, name, NULL, (Uint8*) data, endian.swapUint32(sprites[i].dataSize));

						//	If needed, now bind texture atlases
						if (sprites[i].texAtlasNameListOffset != 0xFFFFFFFF)
						{
							nameOffsets = (Uint32*) (_buffer + endian.swapUint32(header->nameOffsets) + endian.swapUint32(sprites[i].texAtlasNameListOffset));

							for (j=0;j<endian.swapUint32(sprites[i].numTexAtlasNames);j++)
							{
								atlas = getTaskManager()->getApp()->getTextureAtlasManager()->findTextureAtlasFromName(names + endian.swapUint32(nameOffsets[j]));

								AssertWDesc(atlas != NULL, "Could not find TextureAtlas");

								if (atlas)
									sprite->addTextureAtlas(atlas);
								else
									error = EOSErrorNULL;
							}
						}

						if (sprites[i].sprTexAtlasNameOffset != 0xFFFFFFFF)
						{
							mapping = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetTextureAtlasMappingFromName(names + endian.swapUint32(sprites[i].sprTexAtlasNameOffset));

							AssertWDesc(mapping != NULL, "Could not find TextureAtlasMapping");

							if (mapping)
							{
								sprite->setTextureAtlasMapping(mapping);
							}
							else
								error = EOSErrorNULL;
						}
					}
					else
						error = EOSErrorResourceNotAvailable;

					if (error != EOSErrorNone)
						break;
				}
			}

			if (error == EOSErrorNone)
			{
				anims = (EOSRsrcBundleSpriteAnimExport*) (_buffer + endian.swapUint32(header->spriteAnims));

				for (i=0;i<endian.swapUint32(header->numSpriteAnims);i++)
				{
					name = names + endian.swapUint32(anims[i].nameOffset);

					anim = getTaskManager()->getApp()->getSpriteAnimSetManager()->getFreeSpriteAnimSet();
					data = (Uint8*) (dataPool + endian.swapUint32(anims[i].dataOffset));

					AssertWDesc(anim != NULL, "Could not allocate SpriteAnimSet.");

					if (anim)
					{
						if (anims[i].sprNameOffset != 0xFFFFFFFF)
							sprite = getTaskManager()->getApp()->getSpriteSetManager()->findSpriteSetFromName(names + endian.swapUint32(anims[i].sprNameOffset));
						else
							sprite = NULL;

						error = anim->create(InvalidObjectID, name, sprite, (Uint8*) data, endian.swapUint32(anims[i].dataSize));
					}
					else
						error = EOSErrorResourceNotAvailable;

					if (error != EOSErrorNone)
						break;
				}
			}
		}
		else
			error = EOSErrorNULL;
	}
	else
		error = EOSErrorNULL;

	return error;
}
