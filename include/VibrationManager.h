/******************************************************************************
 *
 * File: VibrationManager.h
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2008-2009 2n Productions, All Rights Reserved.
 *
 * Vibration Manager
 * 
 *****************************************************************************/

#ifndef __VIBRATION_MANAGER_H__
#define __VIBRATION_MANAGER_H__

#include "Platform.h"

class VibrationManager : public EOSFrameworkComponent
{
private:
	Boolean			_enabled;
	
public:
	VibrationManager();
	~VibrationManager();
	
	inline Boolean	isEnabled(void) { return _enabled; }
	void			setEnabled(Boolean enabled);
	
	//	Note not all platforms support a defined duration
	void			vibrateOn(MicroSeconds duration);
	void			vibrateOff(void);
	
	void			update(MicroSeconds delta_t);
};

#endif /* __VIBRATION_MANAGER_H__ */
