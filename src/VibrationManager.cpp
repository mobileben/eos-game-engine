/******************************************************************************
 *
 * File: VibrationManager.cpp
 * Author: Benjamin Lee
 * Developed by: 2n Productions
 *
 * Copyright (C) 2009 2n Productions, All Rights Reserved.
 *
 * Vibration Manager
 * 
 *****************************************************************************/

#include "Platform.h"
#include "VibrationManager.h"

#ifdef _PLATFORM_IPHONE

#include "AudioToolbox/AudioServices.h"

#endif /* _PLATFORM_IPHONE */

VibrationManager::VibrationManager() : _enabled(false)
{
}

VibrationManager::~VibrationManager()
{
}

void VibrationManager::setEnabled(Boolean enabled)
{
	if (_enabled)
		vibrateOff();
	
	_enabled = enabled;
}

void VibrationManager::vibrateOn(MicroSeconds duration)
{
	if (_enabled)
	{
#ifdef _PLATFORM_IPHONE
		AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
#endif /* _PLATFORM_IPHONE */
	}		
}

void VibrationManager::vibrateOff(void)
{
}

void VibrationManager::update(MicroSeconds delta_t)
{
}
