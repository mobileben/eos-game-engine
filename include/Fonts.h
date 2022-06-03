/******************************************************************************
 *
 * File: Fonts.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Fonts
 * 
 *****************************************************************************/

#ifndef __FONTS_H__
#define __FONTS_H__

#include "Platform.h"
#include "EOSError.h"
#include "Texture.h"
#include "Graphics.h"

typedef struct
{
	Uint32	   		valid; 
	Float32			leading;
	Float32			trailing;
	Float32			dy;
	AddressOffset	vertices;
	AddressOffset	uvs;
} FontSymbolRAW;

typedef struct
{
	Uint32			min;
	Uint32			max;
	Uint32			height;
	Uint32			nextLine;
	Uint32			spaceWidth;
	Uint32			nameOffset;
	Uint32			texNameOffset;
	Uint32			numSymbols;
	AddressOffset	symbols;
	AddressOffset	vertices;
	AddressOffset	uvs;
} FontSetScriptHeader;

typedef struct
{
	Uint32			endian;
	Uint32			version;
	Uint32			nameOffset;
	Uint32			numFontScripts;
	Uint32			numVertices;
	Uint32			numUVs;
	AddressOffset	fontScripts;
	AddressOffset	symbols;
	AddressOffset	vertices;
	AddressOffset	uvs;
	AddressOffset	names;
} FontSetHeader;

typedef struct
{
	Uint32		valid;
	Float32		trailing;
	Float32		dy;
	Point2D*	vertices;
	Point2D*	uvs;
} FontSymbol;

class FontSetScript : public EOSObject
{
protected:
	Char*		_name;
	Char*		_texRefName;

	Uint32		_numSymbols;
	FontSymbol*	_symbols;
	Texture*	_texRef;

	Uint32		_minRange;
	Uint32		_maxRange;

	Uint32		_height;
	Uint32		_nextLine;
	Uint32		_spaceWidth;	

public:
	FontSetScript();
	~FontSetScript();

	inline Uint32		getMinRange(void) { return _minRange; }
	inline Uint32		getMaxRange(void) { return _maxRange; }
	inline Uint32		getHeight(void) { return _height; }
	inline Uint32		getNextLine(void) { return _nextLine; }
	inline Uint32		getSpaceWidth(void) { return _spaceWidth; }
	inline Uint32		getNumSymbols(void) { return _numSymbols; }
	inline FontSymbol*	getSymbols(void) { return _symbols; }
	inline Texture*		getTexture(void) { return _texRef; }
	
	EOSError		setSymbols(Uint32 num, FontSymbol* symbols);
	inline void		setMinRange(Uint32 val) { _minRange = val; }
	inline void		setMaxRange(Uint32 val) { _maxRange = val; }
	inline void		setHeight(Uint32 val) { _height = val; }
	inline void		setNextLine(Uint32 val) { _nextLine = val; }
	inline void		setSpaceWidth(Uint32 val) { _spaceWidth = val; }
	void			setName(const Char* name);
	void			setTexRefName(const Char* name);

	void			bindTexture(void);
};

class FontSet : public EOSObject
{
protected:
	Boolean		_used;

	Char*		_name;

	Uint32		_numFontScripts;
	FontSetScript*	_fontScripts;

	FontSetScript*	_cachedScript;

	Point2D*	_vertices;
	Point2D*	_uvs;

public:
	FontSet();
	~FontSet();

	inline Boolean	isUsed(void) { return _used; }
	inline void		setAsUsed(void) { _used = true; }
	inline void		setAsUnused(void) { _used = false; }
	inline Char*	getName(void) { return _name; }
	
	inline Boolean		charSupported(Char chr) { return charSupported((UTF16) chr); }
	Boolean				charSupported(UTF16 chr);
	
	inline Uint32	getHeight(void) { return _fontScripts[0].getHeight(); }
	inline Uint32	getNextLine(void) { return _fontScripts[0].getNextLine(); }
	inline Uint32	getSpaceWidth(void) { return _fontScripts[0].getSpaceWidth(); }

	FontSetScript*		findScriptFromSymbol(Uint32 symbol);

	EOSError		create(FontSetHeader* header);
};

#endif /* __FONTS_H__*/
