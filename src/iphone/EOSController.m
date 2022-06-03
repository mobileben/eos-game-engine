//
//  EOSController.m
//  eosTest
//
//  Created by Benjamin Lee on 2/1/09.
//  Copyright 2009 2n Productions. All rights reserved.
//

#import "EOSController.h"
#include "App.h"

@implementation EOSController

@synthesize window;
@synthesize glView;

/*
// Override initWithNibName:bundle: to load the view using a nib file then perform additional customization that is not appropriate for viewDidLoad.
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    if (self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil]) {
        // Custom initialization
    }
    return self;
}
*/

/*
// Implement loadView to create a view hierarchy programmatically.
- (void)loadView {
}
*/

/*
// Implement viewDidLoad to do additional setup after loading the view.
- (void)viewDidLoad {
    [super viewDidLoad];
}
*/


-(BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation) interfaceOrientation
{
#ifdef _DEBUG
	NSLog(@"should AutoRotate");
#endif /* _DEBUG */
	
	if (_appRefPtr)
		return _appRefPtr->applicationShouldAutorotateToInterfaceOrientation(interfaceOrientation);
	else
		return NO;
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
#ifdef _DEBUG
	NSLog(@"will rotate");
#endif /* _DEBUG */

	if (_appRefPtr)
		_appRefPtr->applicationWillRotateToInterfaceOrientation(toInterfaceOrientation, duration);	
}

-(void) didRotateFromInterfaceOrientation:(UIInterfaceOrientation) from
{
#ifdef _DEBUG
	NSLog(@"did");
#endif /* _DEBUG */

	if (_appRefPtr)
		_appRefPtr->applicationDidRotateFromInterfaceOrientation(from);
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning]; // Releases the view if it doesn't have a superview
    // Release anything that's not essential, such as cached data
	
#ifdef _DEBUG
	NSLog(@"didReceiveMemoryWarning");
#endif /* _DEBUG */
	
	if (_appRefPtr)
		_appRefPtr->didReceiveMemoryWarning();
}

- (void)applicationDidFinishLaunching:(UIApplication *)application {
	CGRect					rect = [[UIScreen mainScreen] bounds];

#ifdef _DEBUG
	NSLog(@"didfinishLaunching");
#endif /* _DEBUG */
	
	AssertWDesc(_appRefPtr != NULL, "applicationDidFinishLaunching() NULL app");
	
//	window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	
	glView = [[OpenGLView alloc] initWithFrame:CGRectMake(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height)]; // - kPaletteHeight 

	[window addSubview:glView];
	[window makeKeyAndVisible];
	
	if (_appRefPtr != NULL)
	{
		_appRefPtr->initializeEOSFramework(self);
		_appRefPtr->init();
		_appRefPtr->getHeartbeat()->setRunning(true);
		_appRefPtr->getHeartbeat()->reset();
		
		_appRefPtr->applicationDidFinishLaunching();
		
		timerBasedLoopTimer = [NSTimer scheduledTimerWithTimeInterval:1.0F / 60.0F target:self selector:@selector(timerBasedLoop) userInfo:nil repeats:YES];
	}
	
	
#ifdef _DEBUG
	
	NSArray* array = [window subviews];
	
	NSLog(@"SUBVIEWS %d", [array count]);
	
	UIView* tmp = [array objectAtIndex:0];
	rect = [tmp frame];
	
	NSLog(@"RECT %f %f %f %f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
	
	if (tmp == self.view)
		NSLog(@"YEP");
	
	rect = [self.view frame];
	NSLog(@"RECT %f %f %f %f", rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);

	if (window == nil)
		NSLog(@"NILL");
#endif /* _DEBUG */
	
//	if (_appRefPtr->getGfx()->isLandscape())
//		[[UIDevice currentDevice] setOrientation:UIInterfaceOrientationLandscapeRight];
}


- (void)applicationWillResignActive:(UIApplication *)application {
#ifdef _DEBUG
	NSLog(@"applicationWillResignActive");
#endif /* _DEBUG */
	
	if (_appRefPtr)
		_appRefPtr->applicationWillResignActive();

}

- (void)applicationDidBecomeActive:(UIApplication *)application {
#ifdef _DEBUG
	NSLog(@"applicationDidBecomeActive");
#endif /* _DEBUG */
	
	if (_appRefPtr)
		_appRefPtr->applicationDidBecomeActive();
}

- (void)applicationWillTerminate:(UIApplication *) application
{
	if (_appRefPtr)
		_appRefPtr->applicationWillTerminate();
}

- (BOOL)application:(UIApplication *)application handleOpenURL:(NSURL *)url 
{
	return _appRefPtr->applicationHandleOpenURL(url);
}

#ifdef _SUPPORT_ACCELEROMETER
-(void) accelerometer:(UIAccelerometer* ) accelerometer didAccelerate:(UIAcceleration*) acceleration
{
	if (_appRefPtr)
		_appRefPtr->getEOSEventManager()->interceptAccelerometerEventUpdate(acceleration);
}

-(void) turnOnAccelerometer:(Float32) rate
{
	accelerometerUpdateRate = rate;
	accelerometer = [UIAccelerometer sharedAccelerometer];
	accelerometer.updateInterval = accelerometerUpdateRate;
	accelerometer.delegate = self;
}

-(void) turnOffAccelerometer
{
	accelerometer.delegate = nil;
}

#endif /* _SUPPORT_ACCELEROMETER */
-(void) orientationChange:(NSNotification*) notification
{
	if (_appRefPtr)
		_appRefPtr->applicationDidRotateFromInterfaceOrientation((UIInterfaceOrientation) [[UIDevice currentDevice] orientation]);
}

-(void) notifyOnOrientationChangeOn
{
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(orientationChange:)
												 name:@"UIDeviceOrientationDidChangeNotification" object:nil];
}

-(void) notifyOnOrientationChangeOff
{
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:@"UIDeviceOrientationDidChangeNotification" object:nil];
	
	[[UIDevice currentDevice] endGeneratingDeviceOrientationNotifications];
}

-(void) timerBasedLoop
{
	_appRefPtr->getHeartbeat()->platformHeartbeat();
	
	if (_appRefPtr->updateAssertHitCheck() == false)
		_appRefPtr->update(_appRefPtr->getHeartbeat()->getDeltaMicroSecondsFree());
	
	if (_appRefPtr->renderAssertHitCheck() == false)
		_appRefPtr->render();
}


- (void)dealloc {
	[window release];
	[glView release];
	
	[timerBasedLoopTimer invalidate];
	timerBasedLoopTimer = nil;
	
    [super dealloc];
}


@end
