/******************************************************************************
 *
 * File: TextureManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Texture Manager
 * 
 *****************************************************************************/

#include "TextureManager.h"
#include "File.h"
#include "App.h"

Uint8	_colorBlendTextureData[] = 
{
	0xFF, 0xFF, 0xFF, 0xFF,
};

TextureManager::TextureManager()
{
	_maxTextures = 0;
	_maxRefIDs = 0;
	_numUsedTextures = 0;

	_textures = NULL;
	_refIDToTextureID = NULL;
	_refIDToHWTextureID = NULL;

	_colorBlendTexture = NULL;

	_textureInfoNeedsFree = false;
	_numTextureInfo = 0;
	_textureInfo = NULL;
}

TextureManager::~TextureManager()
{
	deallocateTexturePool();
	destroyTextureInfoDatabase();
}

EOSError TextureManager::allocateTexturePool(Uint32 texmax, Uint32 refmax)
{
	EOSError	error = EOSErrorNone;

	AssertWDesc(texmax != 0, "texmax must be != 0");
	AssertWDesc(refmax != 0, "refmax must be != 0");
	AssertWDesc(refmax >= texmax, "refmax must be >= texmax");

	//	Currently only one pool can be allcoated, so destroy the previous pool
	deallocateTexturePool();

	_textures = new Texture[texmax];
	_refIDToTextureID = new Uint32[refmax];
	_refIDToHWTextureID = new Uint32[refmax];

	AssertWDesc(_textures != NULL, "_textures is NULL");
	AssertWDesc(_refIDToTextureID != NULL, "_refIDToTextureID is NULL");
	AssertWDesc(_refIDToHWTextureID != NULL, "_refIDToHWTextureID is NULL");

	if (_textures != NULL && _refIDToTextureID != NULL && _refIDToHWTextureID != NULL)
	{

		_maxTextures = texmax;
		_maxRefIDs = refmax;
		_numUsedTextures = 0;

		//	Now create an empty texture that later on will be used for color blending
		_colorBlendTexture = getFreeTexture();

		if (_colorBlendTexture)
		{
#ifdef _USE_OPENGL
			_appRefPtr->getGfx()->lockGLContext();
			error = _colorBlendTexture->createFromGLImage((GLubyte*) _colorBlendTextureData, (GLint) 1, (GLint) 1, GL_RGBA, GL_UNSIGNED_BYTE, 0);
			_appRefPtr->getGfx()->unlockGLContext();
#endif /* _USE_OPENGL */
		}
	}
	else
	{
		deallocateTexturePool();

		error = EOSErrorNoMemory;
	}

	return error;
}

Texture* TextureManager::getFreeTexture(void)
{
	Texture*	texture = NULL;
	Uint32		i;

	for (i=0;i<_maxTextures;i++)
	{
		if (_textures[i].isUsed() == false)
		{
			texture = &_textures[i];
			texture->setAsUsed();

			//	Rebuild any necessary tables to stay in sync with faster access structures
			invalidate();
			break;
		}
	}

	return texture;
}

Texture* TextureManager::findTextureFromRefID(ObjectID refID)
{
	Texture*	texture = NULL;
	Uint32		i;

	if (refID == InvalidObjectID)
		return texture;

	for (i=0;i<_maxTextures;i++)
	{
		if (_textures[i].isUsed() && _textures[i].getRefID() == refID)
		{
			texture = &_textures[i];
			break;
		}
	}

	return texture;
}

Texture* TextureManager::findTextureFromName(const Char* name)
{
	Texture*	texture = NULL;
	Uint32		i;

	if (name)
	{
		for (i=0;i<_maxTextures;i++)
		{
			if (_textures[i].isUsed() && _textures[i].getName())
			{
				if (!strcmp(_textures[i].getName(), name))
				{
					texture = &_textures[i];
					break;
				}
			}
		}
	}

	return texture;
}

void TextureManager::invalidate(void)
{
	Uint32	i;

	for (i=0;i<_maxRefIDs;i++)
	{
		_refIDToTextureID[i] = InvalidTextureID;
		_refIDToHWTextureID[i] = InvalidHWTextureID;
	}

	for (i=0;i<_maxTextures;i++)
	{
		if (_textures[i].isUsed() && _textures[i].isInstantiated())
		{
#ifdef _NOT_YET
			//	This is broken, and not used yet, since we occasionally assign ref ids to negative numbers or other ones out of bounds
			_refIDToTextureID[_textures[i].getRefID()] = i;
			_refIDToHWTextureID[_textures[i].getRefID()] = _textures[i].getHWTextureID();
#endif /* _NOT_YET */
		}
	}
}

void TextureManager::destroyTexture(Texture& texture)
{
	if (texture.isUsed())
	{
		texture.setAsUnused();

		invalidate();
	}
}

void TextureManager::deallocateTexturePool(void)
{
	Uint32	i;

	if (_textures)
	{
		for (i=0;i<_maxTextures;i++)
		{
			if (_textures[i].isUsed())
			{
				_textures[i].setAsUnused();
			}
		}

		delete [] _textures;
		_textures = NULL;
	}

	if (_refIDToTextureID)
	{
		delete _refIDToTextureID;
		_refIDToTextureID = NULL;
	}

	if (_refIDToHWTextureID)
	{
		delete _refIDToHWTextureID;
		_refIDToHWTextureID = NULL;
	}

	_maxTextures = _numUsedTextures = 0;
	_maxRefIDs = 0;
}

EOSError TextureManager::createTextureInfoDatabase(Uint32 num, TextureInfo* info, Boolean persistent)
{
	EOSError	error = EOSErrorNone;
	Uint32		i;

	AssertWDesc(info != NULL, "TextureManager::createTextureInfoDatabase() NULL info");
	AssertWDesc(num != 0, "TextureManager::createTextureInfoDatabase() num != 0");

	destroyTextureInfoDatabase();

	_textureInfoNeedsFree = !persistent;

	_numTextureInfo = num;

	if (persistent)
		_textureInfo = info;
	else
	{
		_textureInfo = new TextureInfo[num];

		memset(_textureInfo, 0, sizeof(TextureInfo) * num);

		for (i=0;i<num;i++)
		{
			_textureInfo[i].refID = info[i].refID;

			if (info[i].refName)
			{
				_textureInfo[i].refName = new Char[strlen(info[i].refName) + 1];

				if (_textureInfo[i].refName)
				{
					strcpy(_textureInfo[i].refName, info[i].refName);
				}
				else
					error = EOSErrorNoMemory;
			}

			if (info[i].filename)
			{
				_textureInfo[i].filename = new Char[strlen(info[i].filename) + 1];

				if (_textureInfo[i].filename)
				{
					strcpy(_textureInfo[i].filename, info[i].filename);
				}
				else
					error = EOSErrorNoMemory;
			}

			_textureInfo[i].width = info[i].width;
			_textureInfo[i].height = info[i].height;
			_textureInfo[i].colorFormat = info[i].colorFormat;
			_textureInfo[i].state = info[i].state;
			_textureInfo[i].blending = info[i].blending;

			if (error != EOSErrorNone)
				break;
		}
	}

	if (error != EOSErrorNone)
		destroyTextureInfoDatabase();

	return error;
}

EOSError TextureManager::createTextureInfoDatabase(const Char* filename, Uint8* buffer, Uint32 buffersize)
{
	EOSError			error = EOSErrorNone;
	Uint32				i;
	FileDescriptor		fdesc;
	File				file;
	Endian				endian;
	TexInfoDBHeader*	header;
	TexInfoRAW*			raws;
	Char*				names;
	Char*				str;

	AssertWDesc(filename != NULL, "TextureManager::createTextureInfoDatabase() NULL name");
	
	destroyTextureInfoDatabase();

	fdesc.setFileAccessType(FileAccessTypeReadOnly);

	fdesc.setFilename(filename);

	file.setFileDescriptor(fdesc);

	error = file.open();

	if (error == EOSErrorNone)
	{
		if (file.length() < (Sint32) buffersize)
		{
			_textureInfoNeedsFree = true;

			error = file.readUint8(buffer, file.length());

			if (error == EOSErrorNone)
			{
				header = (TexInfoDBHeader*) buffer;

				if (header->endian == 0x04030201)
					endian.switchEndian();

				_numTextureInfo = endian.swapUint32(header->numTexInfo);;

				raws = (TexInfoRAW*) (buffer + endian.swapUint32(header->texInfo));
				names = (Char*) (buffer + endian.swapUint32(header->names));

				_textureInfo = new TextureInfo[_numTextureInfo];
	
				memset(_textureInfo, 0, sizeof(TextureInfo) * _numTextureInfo);
	
				for (i=0;i<_numTextureInfo;i++)
				{
					_textureInfo[i].refID = endian.swapUint32(raws[i].refID);

					str = &names[endian.swapUint32(raws[i].nameOffset)];

					_textureInfo[i].refName = new Char[strlen(str) + 1];

					if (_textureInfo[i].refName)
					{
						strcpy(_textureInfo[i].refName, str);
					}
					else
						error = EOSErrorNoMemory;

					str = &names[endian.swapUint32(raws[i].filenameOffset)];

					_textureInfo[i].filename = new Char[strlen(str) + 1];

					if (_textureInfo[i].filename)
					{
						strcpy(_textureInfo[i].filename, str);
					}
					else
						error = EOSErrorNoMemory;
	
					_textureInfo[i].width = endian.swapUint32(raws[i].width);
					_textureInfo[i].height = endian.swapUint32(raws[i].height);
					_textureInfo[i].colorFormat = endian.swapUint32(raws[i].colorFormat);
					_textureInfo[i].state = endian.swapUint32(raws[i].texState);
					_textureInfo[i].blending = endian.swapUint32(raws[i].texBlend);
	
					if (error != EOSErrorNone)
						break;
				}
			}
			else
				error = EOSErrorResourceRead;
		}
		else
			error = EOSErrorBufferSize;
	}

	file.close();

	if (error != EOSErrorNone)
		destroyTextureInfoDatabase();

	return error;
}

void TextureManager::destroyTextureInfoDatabase(void)
{
	Uint32	i;

	if (_textureInfoNeedsFree)
	{
		for (i=0;i<_numTextureInfo;i++)
		{
			if (_textureInfo[i].refName)
			{
				delete _textureInfo[i].refName;
			}

			if (_textureInfo[i].filename)
			{
				delete _textureInfo[i].filename;
			}
		}

		if (_textureInfo)
		{
			delete _textureInfo;
		}
	}

	_numTextureInfo = 0;
	_textureInfo = NULL;
}

Uint32 TextureManager::getNumUsedTextures(void)
{
	updateNumUsedTextures();
	
	return _numUsedTextures;
}

void TextureManager::updateNumUsedTextures(void)
{
	Uint32	i;
	
	_numUsedTextures = 0;
	_totalMemUsage = 0;
	
	for (i=0;i<_maxTextures;i++)
	{
		if (_textures[i].isUsed())
		{
			_numUsedTextures++;
			_totalMemUsage += _textures[i].getMemoryUsage();
		}
	}
}

Uint32 TextureManager::getTotalMemoryUsage(void)
{
	updateNumUsedTextures();

	return _totalMemUsage;
}

TextureInfo* TextureManager::findTextureInfoByRefID(ObjectID objid)
{
	Uint32			i;
	TextureInfo*	info = NULL;

	for (i=0;i<_numTextureInfo;i++)
	{
		if (objid == _textureInfo[i].refID)
		{
			info = &_textureInfo[i];
			break;
		}
	}

	return info;
}

TextureInfo* TextureManager::findTextureInfoByRefName(const Char* name)
{
	Uint32			i;
	TextureInfo*	info = NULL;

	if (name)
	{
		for (i=0;i<_numTextureInfo;i++)
		{
			if (_textureInfo[i].refName)
			{
				if (!strcmp(_textureInfo[i].refName, name))
				{
					info = &_textureInfo[i];
					break;
				}
			}
		}
	}

	return info;
}

