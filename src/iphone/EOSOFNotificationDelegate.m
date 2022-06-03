//
//  EOSOFNotificationDelegate.m
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#ifdef _SUPPORT_OPEN_FEINT

#import "EOSOFNotificationDelegate.h"

@implementation EOSOFNotificationDelegate

- (BOOL)isOpenFeintNotificationAllowed:(OFNotificationData*)notificationData
{
	return (BOOL) _appRefPtr->isOpenFeintNotificationAllowed(notificationData);
}

- (void)handleDisallowedNotification:(OFNotificationData*)notificationData
{
	_appRefPtr->handleDisallowedNotification(notificationData);
}

- (void)notificationWillShow:(OFNotificationData*)notificationData
{
	_appRefPtr->notificationWillShow(notificationData);
}

@end

#endif /* _SUPPORT_OPEN_FEINT */
