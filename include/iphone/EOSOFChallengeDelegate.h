//
//  EOSOFChallengeDelegate.h
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#ifdef _SUPPORT_OPEN_FEINT

#pragma once

#import "OFChallengeDelegate.h"
#include "App.h"

@interface EOSOFChallengeDelegate : NSObject< OFChallengeDelegate >

- (void)userLaunchedChallenge:(OFChallengeToUser*)challengeToLaunch withChallengeData:(NSData*)challengeData;
- (void)userRestartedChallenge;

@end

#endif /* _SUPPORT_OPEN_FEINT */