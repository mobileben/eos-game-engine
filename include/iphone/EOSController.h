//
//  EOSController.h
//  eosTest
//
//  Created by Benjamin Lee on 2/1/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import <UIKit/UIKit.h>

@class OpenGLView;

#ifdef _SUPPORT_ACCELEROMETER
@interface EOSController : UIViewController <UIApplicationDelegate, UIAccelerometerDelegate> 
#else
@interface EOSController : UIViewController <UIApplicationDelegate> 
#endif /* _SUPPORT_ACCELEROMETER */
{
	UIWindow*	window;
	OpenGLView*	glView;
	
#ifdef _SUPPORT_ACCELEROMETER
	Float32				accelerometerUpdateRate;	
	UIAccelerometer*	accelerometer;
#endif /* _SUPPORT_ACCELEROMETER */
	
	NSTimer*		timerBasedLoopTimer;
	NSTimeInterval	timerBasedLoopTimerInterval;
}

-(void) timerBasedLoop;
#ifdef _SUPPORT_ACCELEROMETER
-(void) turnOnAccelerometer:(Float32) rate;
-(void) turnOffAccelerometer;
#endif /* _SUPPORT_ACCELEROMETER */

-(void) notifyOnOrientationChangeOn;
-(void) notifyOnOrientationChangeOff;

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet OpenGLView *glView;

@end
