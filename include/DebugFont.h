/******************************************************************************
 *
 * File: DebugFont.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Debug Font
 * 
 *****************************************************************************/

#ifndef __DEBUG_FONT_H__
#define __DEBUG_FONT_H__

#include "Platform.h"
#include "EOSError.h"
#include "TextureAtlas.h"

class DebugFont: public EOSFrameworkComponent
{
private:
	TextureAtlas	_fontAtlas;
	Texture			_texture;

public:
	DebugFont();
	~DebugFont();

	EOSError	init(void);
	Uint32		drawString(Sint32 x, Sint32 y, const Char* str, Boolean linewrap = false);
};

#endif /* __DEBUG_FONT_H__ */
