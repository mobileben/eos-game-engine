/******************************************************************************
 *
 * File: Fonts.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Fonts
 * 
 *****************************************************************************/

#include "Platform.h"
#include "Fonts.h"
#include "Endian.h"
#include "App.h"

FontSetScript::FontSetScript() : _numSymbols(0), _symbols(NULL), _texRef(NULL), _name(NULL), _texRefName(NULL), _minRange(0), _maxRange(0), _height(0), _nextLine(0), _spaceWidth(0)
{
}

FontSetScript::~FontSetScript()
{
	if (_symbols)
		delete _symbols;

	if (_name)
		delete _name;

	if (_texRefName)
		delete _texRefName;
}

EOSError FontSetScript::setSymbols(Uint32 num, FontSymbol* symbols)
{
	_numSymbols = num;
	_symbols = symbols;

	return EOSErrorNone;
}

void FontSetScript::setName(const Char* name)
{
	if (_name)
		delete _name;

	_name = new Char[strlen(name) + 1];

	if (_name)
		strcpy(_name, name);
}

void FontSetScript::setTexRefName(const Char* name)
{
	if (_texRefName)
		delete _texRefName;

	_texRefName = new Char[strlen(name) + 1];

	if (_texRefName)
		strcpy(_texRefName, name);
}

void FontSetScript::bindTexture(void)
{
	_texRef = _appRefPtr->getTextureManager()->findTextureFromName(_texRefName);
}

FontSet::FontSet() : _used(false), _name(NULL), _numFontScripts(0), _fontScripts(NULL), _cachedScript(NULL), _vertices(NULL), _uvs(NULL)
{
}

FontSet::~FontSet()
{
	if (_name)
		delete _name;

	if (_fontScripts)
		delete [] _fontScripts;

	if (_vertices)
		delete _vertices;

	if (_uvs)
		delete _uvs;
}

Boolean	FontSet::charSupported(UTF16 chr)
{	
	FontSetScript*	currScript = findScriptFromSymbol(chr);
	FontSymbol*	symbol;
	
	if (currScript)
	{
		symbol = &currScript->getSymbols()[chr - currScript->getMinRange()];
		
		if (symbol->valid)
		{
			return true;
		}
	}
	
	return false;
}

FontSetScript* FontSet::findScriptFromSymbol(Uint32 symbol)
{
	FontSetScript*	script = NULL;
	Uint32		i;

	if (_cachedScript)
	{
		if (symbol >= _cachedScript->getMinRange() && symbol <= _cachedScript->getMaxRange())
			script = _cachedScript;
	}

	if (script == NULL)
	{
		for (i=0;i<_numFontScripts;i++)
		{
			if (symbol >= _fontScripts[i].getMinRange() && symbol <= _fontScripts[i].getMaxRange())
			{
				script = &_fontScripts[i];
				_cachedScript = script;
				break;
			}
		}
	}

	return script;
}

EOSError FontSet::create(FontSetHeader* header)
{
	EOSError			error = EOSErrorNone;
	Endian				endian;
	Char*				name;
	Char*				names;
	Uint8*				data = (Uint8*) header;
	FontSymbol*			symbols;
	FontSymbolRAW*		raws;
	FontSetScriptHeader*	scripts;
	Point2D*			inPoint2D;
	Uint32				i, j;

	if (header->endian == 0x04030201)
		endian.switchEndian();

	names = (Char*) (data + endian.swapUint32(header->names));

	_vertices = new Point2D[endian.swapUint32(header->numVertices)];
	_uvs = new Point2D[endian.swapUint32(header->numUVs)];
	_fontScripts = new FontSetScript[endian.swapUint32(header->numFontScripts)];

	if (_vertices && _uvs && _fontScripts)
	{
		name = &names[endian.swapUint32(header->nameOffset)];

		if (_name)
			delete _name;

		_name = new Char[strlen(name) + 1];

		if (_name)
			strcpy(_name, name);

		_numFontScripts = endian.swapUint32(header->numFontScripts);

		scripts = (FontSetScriptHeader*) (data + endian.swapUint32(header->fontScripts));

		inPoint2D = (Point2D*) (data + endian.swapUint32(header->vertices));

		for (i=0;i<endian.swapUint32(header->numVertices);i++)
		{
			_vertices[i].x = endian.swapFloat32(inPoint2D[i].x);
			_vertices[i].y = endian.swapFloat32(inPoint2D[i].y);
		}

		inPoint2D = (Point2D*) (data + endian.swapUint32(header->uvs));

		for (i=0;i<endian.swapUint32(header->numUVs);i++)
		{
			_uvs[i].x = endian.swapFloat32(inPoint2D[i].x);
			_uvs[i].y = endian.swapFloat32(inPoint2D[i].y);
		}

		for (i=0;i<_numFontScripts;i++)
		{
			symbols = new FontSymbol[endian.swapUint32(scripts[i].numSymbols)];

			if (symbols)
			{
				_fontScripts[i].setSymbols(endian.swapUint32(scripts[i].numSymbols), symbols);
				_fontScripts[i].setName(&names[endian.swapUint32(scripts[i].nameOffset)]);
				_fontScripts[i].setTexRefName(&names[endian.swapUint32(scripts[i].texNameOffset)]);
				_fontScripts[i].setMinRange(endian.swapUint32(scripts[i].min));
				_fontScripts[i].setMaxRange(endian.swapUint32(scripts[i].max));
				_fontScripts[i].setHeight(endian.swapUint32(scripts[i].height));
				_fontScripts[i].setNextLine(endian.swapUint32(scripts[i].nextLine));
				_fontScripts[i].setSpaceWidth(endian.swapUint32(scripts[i].spaceWidth));

				memset(symbols, 0, sizeof(FontSymbol) * _fontScripts[i].getNumSymbols());

				raws = (FontSymbolRAW*) (data + endian.swapUint32(header->symbols));
				raws = &raws[endian.swapUint32(scripts[i].symbols)];

				for (j=0;j<_fontScripts[i].getNumSymbols();j++)
				{
					symbols[j].valid = endian.swapUint32(raws[j].valid);
					symbols[j].trailing = endian.swapFloat32(raws[j].trailing);
					symbols[j].dy = endian.swapFloat32(raws[j].dy);

					if (symbols[j].valid)
					{
						symbols[j].vertices = &_vertices[endian.swapUint32(scripts[i].vertices) + endian.swapUint32(raws[j].vertices)];
						symbols[j].uvs = &_uvs[endian.swapUint32(scripts[i].uvs) + endian.swapUint32(raws[j].uvs)];
					}
				}

				if (error == EOSErrorNone)
					_fontScripts[i].bindTexture();
			}
			else
				error = EOSErrorNoMemory;

			if (error != EOSErrorNone)
				break;
		}
	}
	else
		error = EOSErrorNoMemory;

	return error;
}
