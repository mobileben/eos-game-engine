/******************************************************************************
 *
 * File: FileManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * File. Basic file for the particular platform.
 * 
 *****************************************************************************/

#ifndef __FILEMANAGER_H__
#define __FILEMANAGER_H__

#include "Platform.h"
#include "File.h"

class FileManager : public EOSFrameworkComponent
{
private:
public:
	FileManager();
	~FileManager();
};

#endif /* __FILEMANAGER_H__ */

