/******************************************************************************
 *
 * File: FontManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Font Manager
 * 
 *****************************************************************************/

#include "Platform.h"
#include "FontManager.h"
#include "App.h"
#include "File.h"
#include "TextureFileLoadTask.h"
#include "FontSetFileLoadTask.h"
#include "TaskManager.h"

FontManager::FontManager() : _numFontSets(0), _fontSets(NULL), _currFontSet(NULL)
{
#ifdef _USE_OPENGL
	_poolSize = 0;
	_vertexPool = NULL;
	_uvPool = NULL;
	_white = NULL;
	_colorPool = NULL;
#endif /* _USE_OPENGL */
}

FontManager::~FontManager()
{
	deallocateFontSetPool();
}

EOSError FontManager::allocateFontSetPool(Uint32 num, Uint32 cacheSize)
{
	EOSError	error = EOSErrorNone;

	deallocateFontSetPool();

	_fontSets = new FontSet[num];

	if (_fontSets)
		_numFontSets = num;
	else
		error = EOSErrorNoMemory;

#ifdef _USE_OPENGL
	_vertexPool = new GLfloat[cacheSize * Renderer::RendererBasicPrimitiveNumVertices * Renderer::RendererBasicPrimitivesPerQuad * 2];
	_uvPool = new GLfloat[cacheSize * Renderer::RendererBasicPrimitiveNumVertices * Renderer::RendererBasicPrimitivesPerQuad * 2];
	_white = new GLfloat[cacheSize * Renderer::RendererBasicPrimitiveNumVertices * Renderer::RendererBasicPrimitivesPerQuad * 2];
	_colorPool = new GLfloat[cacheSize * Renderer::RendererBasicPrimitiveNumVertices * Renderer::RendererBasicPrimitivesPerQuad * 2];

	if (_vertexPool && _uvPool && _white && _colorPool)
	{
		Uint32	i;

		_poolSize = cacheSize;

		for (i=0;i<cacheSize * Renderer::RendererBasicPrimitiveNumVertices * Renderer::RendererBasicPrimitivesPerQuad * 2;i++)
			_white[i] = 1.0F;
	}
	else
		error = EOSErrorNoMemory;
	
#endif /* _USE_OPENGL*/

	if (error != EOSErrorNone)
		deallocateFontSetPool();

	return error;
}

void FontManager::deallocateFontSetPool(void)
{
	if (_fontSets)
		delete [] _fontSets;

	_numFontSets = 0;
	_fontSets = NULL;

#ifdef _USE_OPENGL
	if (_vertexPool)
		delete _vertexPool;

	if (_uvPool)
		delete _uvPool;

	if (_white)
		delete _white;

	if (_colorPool)
		delete _colorPool;

	_poolSize = 0;
	_vertexPool = NULL;
	_uvPool = NULL;
	_white = NULL;
	_colorPool = NULL;
#endif /* _USE_OPENGL */
}

FontSet* FontManager::getFreeFontSet(void)
{
	FontSet*	fontSet = NULL;
	Uint32		i;

	for (i=0;i<_numFontSets;i++)
	{
		if (_fontSets[i].isUsed() == false)
		{
			fontSet = &_fontSets[i];
			break;
		}
	}

	return fontSet;
}

FontSet* FontManager::findFontSet(const Char* name)
{
	FontSet*	fontSet = NULL;
	Uint32		i;

	for (i=0;i<_numFontSets;i++)
	{
		if (_fontSets[i].isUsed())
		{
			if (!strcmp(name, _fontSets[i].getName()))
			{
				fontSet = &_fontSets[i];
				break;
			}
		}
	}

	return fontSet;
}

EOSError FontManager::createFontSet(Uint8* buffer, Uint32 buffersize)
{
	EOSError 	error = EOSErrorNone;
	FontSet*	fontSet;

	fontSet = getFreeFontSet();

	if (fontSet)
	{
		error = fontSet->create((FontSetHeader*) buffer);

		if (error == EOSErrorNone)
		{
			fontSet->setAsUsed();

			if (_currFontSet == NULL)
				_currFontSet = fontSet;
		}
	}
	else
		error = EOSErrorResourceNotAvailable;

	return error;
}

EOSError FontManager::queueLoadTasks(TaskManager* loader, Uint8* buffer, Uint32 buffersize, const Char* image, const Char* fontset)
{
	EOSError					error = EOSErrorNone;
	FileDescriptor				fdesc;
	FontSetFileLoadTask*		fonttask;
	TextureFileLoadTask*		textask;
	TextureInfo*				texinfo;

	texinfo = _appRefPtr->getTextureManager()->findTextureInfoByRefName(image);

	if (texinfo)
	{
		textask = new TextureFileLoadTask;

		fdesc.setFilename(texinfo->filename);
		textask->setLoadBuffer(buffer, buffersize, false);
		textask->setFileDescriptor(fdesc);
		textask->setTextureInfo(*texinfo);

		loader->addTask(textask);
	}
	else
		error = EOSErrorNoMemory;

	fonttask = new FontSetFileLoadTask;

	if (fonttask)
	{
		fdesc.setFilename(fontset);

		fonttask->setFontManager(this);
		fonttask->setFileDescriptor(fdesc);
		fonttask->setLoadBuffer(buffer, buffersize, false);

		loader->addTask(fonttask);
	}
	else
		error = EOSErrorNoMemory;

	return error;
}

void FontManager::setCurrentFontSet(const Char* name)
{
	_currFontSet = findFontSet(name);
}

void FontManager::setCurrentFontSet(FontSet* font)
{
	_currFontSet = font;
}

Boolean	FontManager::charSupported(Char chr)
{
	return _currFontSet->charSupported(chr);
}

Boolean	FontManager::charSupported(UTF16 chr)
{
	return _currFontSet->charSupported(chr);
}

Uint32 FontManager::getFontHeight(void)
{
	return _currFontSet->getHeight();
}

Uint32 FontManager::getFontNextLine(void)
{
	return _currFontSet->getNextLine();
}

#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
Uint32 FontManager::_strlen(UTF16* str)
{
	Uint32	count = 0;
	
	if (str)
	{
		while (str[count] != 0)
			count++;
	}
	
	return count;
}
#endif /* _PLATFORM_MAC || _PLATFORM_IPHONE */

#ifdef _USE_OPENGL

ColorRGBA _whiteColor = 
{
	1.0,
	1.0,
	1.0,
	1.0,
};

void FontManager::renderCharacters(Texture* texture, Point2D& pos, ColorRGBA* color, Uint32 numChars)
{
	ColorRGBA*	rgba;

	if (color)
		rgba = color;
	else
		rgba = &_whiteColor;

	_appRefPtr->getRenderer()->drawSubTextureArray(texture, pos, _vertexPool, _uvPool, rgba, numChars * Renderer::RendererBasicPrimitivesPerQuad);
}

Renderer::Renderer2DCoordinateSystem FontManager::getCoordinateSystem(void)
{
	return _appRefPtr->getRenderer()->get2DCoordinateSystem();
}

#endif /* _USE_OPENGL */


