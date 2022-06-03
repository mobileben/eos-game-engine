//
//  EOSOFChallengeDelegate.m
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#ifdef _SUPPORT_OPEN_FEINT

#import "EOSOFChallengeDelegate.h"

#import "OFChallengeToUser.h"
#import "OFChallenge.h"
#import "OFChallengeDefinition.h"
#import "OFControllerLoader.h"

@implementation EOSOFChallengeDelegate

- (void)userLaunchedChallenge:(OFChallengeToUser*)challengeToLaunch withChallengeData:(NSData*)challengeData
{
	_appRefPtr->userLaunchedChallenge(challengeToLaunch, challengeData);
}

- (void)userRestartedChallenge
{
	_appRefPtr->userRestartedChallenge();
}

@end

#endif /* _SUPPORT_OPEN_FEINT */
