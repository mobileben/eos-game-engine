//
//  EOSOFDelegate.h
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#ifdef _SUPPORT_OPEN_FEINT

#pragma once

#import "OpenFeintDelegate.h"
#import "OFLeaderboardService.h"
#import "OFHighScoreService.h"
#import "OFHighScore.h"
#import "OFLeaderboard.h"
#import "OFPaginatedSeries.h"
#include "App.h"

@interface EOSOFDelegate : NSObject< OFCallbackable, OpenFeintDelegate >
{
	OFDelegate	_successSetHighScore;
	OFDelegate	_failureSetHighScore;
	OFDelegate	_successGetLocalHighScores;
	OFDelegate	_failureGetLocalHighScores;
	OFDelegate	_successUnlockAchievements;
	OFDelegate	_failureUnlockAchievements;
}

- (void)initSelectors;
- (void)dashboardWillAppear;
- (void)dashboardDidAppear;
- (void)dashboardWillDisappear;
- (void)dashboardDidDisappear;
- (void)userLoggedIn:(NSString*)userId;
- (BOOL)showCustomOpenFeintApprovalScreen;
- (bool)canReceiveCallbacksNow;
- (void) setHighScore:(Sint32)score withLeaderboardID:(NSString*)leaderboardID isSilent:(BOOL) silent;
- (void) setHighScoreWithDisplayText:(Sint32)score withDisplayText:(NSString*)text withLeaderboardID:(NSString*)leaderboardID isSilent:(BOOL) silent;
- (void) setHighScoreSuccess;
- (void) setHighScoreFailure;
- (void) getLocalHighScores : (NSString*)leaderboardID;
- (void) getLocalHighScoresSuccess : (OFPaginatedSeries*) loadedLeaderboards;
- (void) unlockAchievements : (NSArray*) achievements;

@end

#endif /* _SUPPORT_OPEN_FEINT */