/******************************************************************************
 *
 * File: FontSetFileLoadTask.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Font Set File Load Task Base Class
 * 
 *****************************************************************************/

#ifndef __FONTSETFILELOADTASK_H__
#define __FONTSETFILELOADTASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "Fonts.h"
#include "FontManager.h"

class FontSetFileLoadTask : public FileLoadTask
{
private:
	FontManager*	_fontMgr;

public:
	FontSetFileLoadTask();
	~FontSetFileLoadTask();

	void					setFontManager(FontManager* mgr);

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
};

#endif /* __FONTSETFILELOADTASK_H__ */
