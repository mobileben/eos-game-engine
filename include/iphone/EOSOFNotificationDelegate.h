//
//  EOSOFNotificationDelegate.h
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#ifdef _SUPPORT_OPEN_FEINT

#pragma once

#import "OFNotificationDelegate.h"
#include "App.h"

@interface EOSOFNotificationDelegate : NSObject< OFNotificationDelegate >

- (BOOL)isOpenFeintNotificationAllowed:(OFNotificationData*)notificationData;
- (void)handleDisallowedNotification:(OFNotificationData*)notificationData;
- (void)notificationWillShow:(OFNotificationData*)notificationData;

@end

#endif /* _SUPPORT_OPEN_FEINT */