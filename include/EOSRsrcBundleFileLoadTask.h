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

#ifndef __EOSRSRCBUNDLEFILELOADTASK_H__
#define __EOSRSRCBUNDLEFILELOADTASK_H__

#include "Platform.h"
#include "FileLoadTask.h"
#include "SpriteAnimSet.h"
#include "EOSRsrcBundle.h"

class EOSRsrcBundleFileLoadTask : public FileLoadTask
{
private:

public:
	EOSRsrcBundleFileLoadTask();
	~EOSRsrcBundleFileLoadTask();

	virtual EOSError		prologue(void);
	virtual EOSError		epilogue(void);
};

#endif /* __EOSRSRCBUNDLEFILELOADTASK_H__ */

