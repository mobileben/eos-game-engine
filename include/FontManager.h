/******************************************************************************
 *
 * File: FontManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Font Manager
 * 
 *****************************************************************************/

#ifndef __FONTMANAGER_H__
#define __FONTMANAGER_H__

#include "Platform.h"
#include "Fonts.h"
#include "Renderer.h"

class TaskManager;

class FontManager : public EOSFrameworkComponent
{
public:
	typedef enum
	{
		JustifyLeft = 0,
		JustifyLeftVerticalCentered,
		JustifyRight,
		JustifyRightVerticalCentered,
		JustifyCentered,
	} Justify;

private:
	Uint32		_numFontSets;
	FontSet*	_fontSets;

	FontSet*	_currFontSet;

#ifdef _USE_OPENGL
	Uint32			_poolSize;
	GLfloat*		_vertexPool;
	GLfloat*		_uvPool;
	GLfloat*		_white;
	GLfloat*		_colorPool;
#endif /* _USE_OPENGL */

	inline Uint32	_strlen(Char* str) { return (Uint32) strlen(str); }
	inline Uint32	_strlen(const Char* str) { return (Uint32) strlen(str); }
#if defined(_PLATFORM_MAC) || defined(_PLATFORM_IPHONE)
	Uint32			_strlen(UTF16* str);
	inline Uint32	_strlen(const UTF16* str) { return _strlen((UTF16*) str); }
#else
	inline Uint32	_strlen(UTF16* str) { return (Uint32) wcslen((wchar_t*) str); }
	inline Uint32	_strlen(const UTF16* str) { return (Uint32) wcslen((wchar_t*) str); }
#endif /* _PLATFORM_MAC || _PLATFORM_IPHONE */
	
#ifdef _USE_OPENGL
	void			renderCharacters(Texture* texture, Point2D& pos, ColorRGBA* color, Uint32 numChars);
#endif /* _USE_OPENGL */
	
	Renderer::Renderer2DCoordinateSystem	getCoordinateSystem(void);
	
public:
	FontManager();
	~FontManager();

	EOSError		allocateFontSetPool(Uint32 num, Uint32 cacheSize);
	void			deallocateFontSetPool(void);

	FontSet*		getFreeFontSet(void);
	FontSet*		findFontSet(const Char* name);

	EOSError		createFontSet(Uint8* buffer, Uint32 buffersize);

	void			setCurrentFontSet(const Char* name);
	void			setCurrentFontSet(FontSet* font);

	Boolean			charSupported(Char chr);
	Boolean			charSupported(UTF16 chr);
	
	Uint32			getFontHeight(void);
	Uint32			getFontNextLine(void);

	EOSError		queueLoadTasks(TaskManager* loader, Uint8* buffer, Uint32 buffersize, const Char* image, const Char* fontset);

	template<class T> void 	drawString(T* str, Sint32 x, Sint32 y, Justify justify = JustifyLeft);
	template<class T> void 	drawString(T* str, Sint32 x, Sint32 y, ColorRGBA* color, Justify justify = JustifyLeft);
	template<class T>Float32	getStringLength(T* str);
};

template <class T> void FontManager::drawString(T* str, Sint32 x, Sint32 y, Justify justify)
{
	drawString(str, x, y, NULL, justify);
}

template <class T> void FontManager::drawString(T* str, Sint32 x, Sint32 y, ColorRGBA* color, Justify justify)
{
	Point2D		pt;
	FontSetScript*	currScript;
	Uint32		length;
	Uint32		i;
	FontSymbol*	symbol;
	Float32		xoffset = 0.0F;
	Uint32		vcount = 0;
	Uint32		numChars = 0;
	Texture*	texture = NULL;
	Point2D*	vertices;
	Point2D*	uvs;
	Float32		dy;
#if defined(_USE_OPENGL) && !defined(_USE_OPENGL_ES_1_1)
	Uint32		idx;
	Uint32		offset; 
#endif /* _USE_OPENGL && !_USE_OPENGL_ES_1_1 */

	pt.x = (Float32) x;
	pt.y = (Float32) y;

	if (justify == JustifyCentered)
		pt.x -= (Float32) getStringLength(str) / 2.0F;
	else if (justify == JustifyRight)
	{
		pt.x -= (Float32) getStringLength(str);
		pt.y += (Float32) _currFontSet->getHeight() / 2.0F;
	}
	else if (justify == JustifyRightVerticalCentered)
		pt.x -= (Float32) getStringLength(str);
	else if (justify == JustifyLeft)
		pt.y += (Float32) _currFontSet->getHeight() / 2.0F;

	//	Snap to integers, since we get half-pixel render problems otherwise
	pt.x = (Float32) ((Sint32) pt.x);
	pt.y = (Float32) ((Sint32) pt.y);

	length = _strlen(str);

	for (i=0;i<length;i++)
	{
		currScript = _currFontSet->findScriptFromSymbol(str[i]);

		if (currScript)
		{
			if (texture != currScript->getTexture())
			{
				if (numChars > 0)
				{
#ifdef _USE_OPENGL
					renderCharacters(texture, pt, color, numChars);
#endif /* _USE_OPENGL */

					numChars = 0;
					vcount = 0;
				}

				texture = currScript->getTexture();
			}

			symbol = &currScript->getSymbols()[str[i] - currScript->getMinRange()];

			if (symbol->valid)
			{
				if (getCoordinateSystem() == Renderer::Renderer2DCoordinateSystemTraditional)
					dy = -symbol->dy;
				else
					dy = symbol->dy;
#ifdef _USE_OPENGL
				vertices = symbol->vertices;
				uvs = symbol->uvs;

				//	In case we are left justified, we need to advance here first
				if (xoffset == 0.0)
					xoffset += (vertices[2].x - vertices[1].x) / 2.0F;

#ifdef _USE_OPENGL_ES_1_1

				//	Typecast Sint32 used to snap to integer, since fonts will have half-pixel render errors
				_vertexPool[vcount + 0] = (Float32) ((Sint32) (vertices[0].x + xoffset));
				_vertexPool[vcount + 1] = (Float32) ((Sint32) (vertices[0].y + dy));
				_vertexPool[vcount + 2] = (Float32) ((Sint32) (vertices[1].x + xoffset));
				_vertexPool[vcount + 3] = (Float32) ((Sint32) (vertices[1].y + dy));
				_vertexPool[vcount + 4] = (Float32) ((Sint32) (vertices[2].x + xoffset));
				_vertexPool[vcount + 5] = (Float32) ((Sint32) (vertices[2].y + dy));
				
				_vertexPool[vcount + 6] = _vertexPool[vcount + 0];
				_vertexPool[vcount + 7] = _vertexPool[vcount + 1];
				
				_vertexPool[vcount + 8] = _vertexPool[vcount + 4];
				_vertexPool[vcount + 9] = _vertexPool[vcount + 5];
				
				_vertexPool[vcount + 10] = (Float32) ((Sint32) (vertices[3].x + xoffset));
				_vertexPool[vcount + 11] = (Float32) ((Sint32) (vertices[3].y + dy));
				
				_uvPool[vcount + 0] = uvs[0].x;
				_uvPool[vcount + 1] = uvs[0].y;
				_uvPool[vcount + 2] = uvs[1].x;
				_uvPool[vcount + 3] = uvs[1].y;
				_uvPool[vcount + 4] = uvs[2].x;
				_uvPool[vcount + 5] = uvs[2].y;
				
				_uvPool[vcount + 6] = _uvPool[vcount + 0];
				_uvPool[vcount + 7] = _uvPool[vcount + 1];
				
				_uvPool[vcount + 8] = _uvPool[vcount + 4];
				_uvPool[vcount + 9] = _uvPool[vcount + 5];
				
				_uvPool[vcount + 10] = uvs[3].x;
				_uvPool[vcount + 11] = uvs[3].y;
#else
				for (idx = 0;idx<4;idx++)
				{
					offset = vcount + idx * 2;

					//	Typecast Sint32 used to snap to integer, since fonts will have half-pixel render errors
					_vertexPool[offset + 0] = (Float32) ((Sint32) (vertices[idx].x + xoffset));
					_vertexPool[offset + 1] = (Float32) ((Sint32) (vertices[idx].y + dy));
					_uvPool[offset + 0] = uvs[idx].x;
					_uvPool[offset + 1] = uvs[idx].y;
				}
#endif /* _USE_OPENGL_ES_1_1 */
				
				vcount += Renderer::RendererBasicPrimitiveNumVertices * Renderer::RendererBasicPrimitivesPerQuad * 2;

				xoffset += (vertices[2].x - vertices[1].x) / 2.0F;

				if (i < (length - 1))
				{
					currScript = _currFontSet->findScriptFromSymbol(str[i+1]);

					if (currScript)
					{
						symbol = &currScript->getSymbols()[str[i+1] - currScript->getMinRange()];

						if (symbol->valid)
						{
							vertices = symbol->vertices;
							xoffset += symbol->trailing + (vertices[2].x - vertices[1].x) / 2.0F;
						}
						else
							xoffset += currScript->getSpaceWidth() / 2.0F;
					}
				}
#endif /* _USE_OPENGL */

				numChars++;
			}
			else
			{
				if (xoffset == 0)
					xoffset += (Float32) currScript->getSpaceWidth() / 2.0F;

				xoffset += (Float32) currScript->getSpaceWidth() / 2.0F;

#ifdef _USE_OPENGL
				if (i < (length - 1))
				{
					currScript = _currFontSet->findScriptFromSymbol(str[i+1]);

					if (currScript)
					{
						symbol = &currScript->getSymbols()[str[i+1] - currScript->getMinRange()];

						if (symbol->valid)
						{
							vertices = symbol->vertices;
							xoffset += (vertices[2].x - vertices[1].x) / 2.0F;
						}
						else
							xoffset += currScript->getSpaceWidth() / 2.0F;
					}
				}
#endif /* _USE_OPENGL */
			}
		}

#ifdef _USE_OPENGL
		if (numChars >= (_poolSize - 1))
		{
			renderCharacters(texture, pt, color, numChars);
			numChars = 0;
			vcount = 0;
		}
#endif /* _USE_OPENGL */
	}

	if (numChars > 0)
	{
#ifdef _USE_OPENGL
		renderCharacters(texture, pt, color, numChars);
#endif /* _USE_OPENGL */
	}
}

template <class T> Float32 FontManager::getStringLength(T* str)
{
	Float32			strLen = 0;
	FontSetScript*	currScript;
	Uint32			length;
	Uint32			i;
	FontSymbol*		symbol;
	Point2D*		vertices;

	length = _strlen(str);

	for (i=0;i<length;i++)
	{
		currScript = _currFontSet->findScriptFromSymbol(str[i]);

		if (currScript)
		{
			symbol = &currScript->getSymbols()[str[i] - currScript->getMinRange()];

			if (symbol->valid)
			{
				vertices = symbol->vertices;

				if (strLen == 0)
				{
					strLen += (vertices[2].x - vertices[1].x) / 2.0F;
				}

				strLen += (vertices[2].x - vertices[1].x) / 2.0F;

				if (i < (length - 1))
				{
					currScript = _currFontSet->findScriptFromSymbol(str[i+1]);

					if (currScript)
					{
						symbol = &currScript->getSymbols()[str[i+1] - currScript->getMinRange()];

						if (symbol->valid)
						{
							vertices = symbol->vertices;
							strLen += symbol->trailing + (vertices[2].x - vertices[1].x) / 2.0F;
						}
						else
							strLen += currScript->getSpaceWidth() / 2.0F;
					}
				}
			}
			else
			{
				if (strLen == 0)
					strLen += currScript->getSpaceWidth() / 2.0F;

				strLen += currScript->getSpaceWidth() / 2.0F;

#ifdef _USE_OPENGL
				if (i < (length - 1))
				{
					currScript = _currFontSet->findScriptFromSymbol(str[i+1]);

					if (currScript)
					{
						symbol = &currScript->getSymbols()[str[i+1] - currScript->getMinRange()];

						if (symbol->valid)
						{
							vertices = symbol->vertices;
							strLen += (vertices[2].x - vertices[1].x) / 2.0F;
						}
						else
							strLen += currScript->getSpaceWidth() / 2.0F;
					}
				}
#endif /* _USE_OPENGL */
			}
		}
	}

	return strLen;
}

#endif /* __FONTMANAGER_H__ */
