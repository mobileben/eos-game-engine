//
//  EOSOFDelegate.m
//  eosTest
//
//  Created by Benjamin Lee on 10/22/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#ifdef _SUPPORT_OPEN_FEINT

#import "EOSOFDelegate.h"
#import "OpenFeint+UserOptions.h"
#import "OFHighScoreService.h"
#import "OFHighScore.h"
#import "OFLeaderboardService.h"
#import "OFLeaderboard.h"
#import "OFPaginatedSeries.h"
#import "OFAchievementService+Private.h"

@implementation EOSOFDelegate

- (void) initSelectors
{
	_successSetHighScore = OFDelegate(self, @selector(setHighScoreSuccess));
	_failureSetHighScore = OFDelegate(self, @selector(setHighScoreFailure));
	_successGetLocalHighScores = OFDelegate(self, @selector(getLocalHighScoresSuccess));
	_failureGetLocalHighScores = OFDelegate(self, @selector(getLocalHighScoresFailure));
	_successUnlockAchievements = OFDelegate(self, @selector(unlockAchievementsSuccess));
	_failureUnlockAchievements = OFDelegate(self, @selector(unlockAchievementsFailure));
}

- (void)dashboardWillAppear
{
	_appRefPtr->dashboardWillAppear();
}

- (void)dashboardDidAppear
{
	_appRefPtr->dashboardDidAppear();
}

- (void)dashboardWillDisappear
{
	_appRefPtr->dashboardWillDisappear();
}

- (void)dashboardDidDisappear
{
	_appRefPtr->dashboardDidDisappear();
}

- (void)userLoggedIn:(NSString*)userId
{
	_appRefPtr->userLoggedIn(userId);
}

- (BOOL)showCustomOpenFeintApprovalScreen
{
	return (BOOL) _appRefPtr->showCustomOpenFeintApprovalScreen();
}

- (bool)canReceiveCallbacksNow 
{ 
	return YES; 
}

- (void) setHighScore:(Sint32)score withLeaderboardID:(NSString*)leaderboardID isSilent:(BOOL) silent
{
	[OFHighScoreService setHighScore:score forLeaderboard:leaderboardID silently:silent onSuccess:_successSetHighScore onFailure:_failureSetHighScore];
}

- (void) setHighScoreWithDisplayText:(Sint32)score withDisplayText:text withLeaderboardID:(NSString*)leaderboardID isSilent:(BOOL) silent
{
	[OFHighScoreService setHighScore:score withDisplayText:text forLeaderboard:leaderboardID silently:silent onSuccess:_successSetHighScore onFailure:_failureSetHighScore];
}
- (void) setHighScoreSuccess
{
}

- (void) setHighScoreFailure
{
}

- (void) getLocalHighScores : (NSString*)leaderboardID
{
	[OFHighScoreService getLocalHighScores:leaderboardID onSuccess:_successGetLocalHighScores onFailure:_failureGetLocalHighScores];
}

- (void) getLocalHighScoresSuccess : (OFPaginatedSeries*) loadedLeaderboards
{	
	OFHighScore*	highscore;
	NSString*		leaderboardID;

	//	This has been disabled in 1.1.22
	if (loadedLeaderboards)
	{	
		if ([loadedLeaderboards count] > 0)
		{
			highscore = [loadedLeaderboards objectAtIndex:0];
			
			if (highscore)
			{
				leaderboardID = [NSString stringWithFormat:@"%d", highscore.leaderboardId];
				
				_appRefPtr->localUserHighScoreSuccess(leaderboardID, (Sint32) [highscore score], (Sint32) [highscore rank]);
			}
			else
				_appRefPtr->localUserHighScoreFailure(nil);
		}
		else
			_appRefPtr->localUserHighScoreFailure(nil);
	}
	else
		_appRefPtr->localUserHighScoreFailure(nil);
}

- (void) unlockAchievements : (NSArray*) achievements
{
	[OFAchievementService unlockAchievements:achievements onSuccess:_successUnlockAchievements onFailure:_failureUnlockAchievements];	
}

- (void) unlockAchievementsSuccess
{
	_appRefPtr->achievementUnlocksSuccess();
}

- (void) unlockAchievementsFailure
{
	_appRefPtr->achievementUnlocksFailure();
}

@end

#endif /* _SUPPORT_OPEN_FEINT */