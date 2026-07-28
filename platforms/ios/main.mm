#ifdef MCPE_IOS
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <AVFoundation/AVFoundation.h>
#include <sys/time.h>

#include <_types.h>
#include <utils.h>
#include <AppContext.hpp>
#include <NinecraftApp.hpp>
#include <input/Mouse.hpp>
#include <input/Multitouch.hpp>
#include <input/Keyboard.hpp>

#include "EAGLView.h"
#include "AppPlatform_iOS.hpp"

static void MCPEConfigureAudioSession(void) {
	Class sessionClass = NSClassFromString(@"AVAudioSession");
	if (!sessionClass) {
		return;
	}

	id session = [sessionClass sharedInstance];
	if (!session) {
		return;
	}

	if ([session respondsToSelector:@selector(setCategory:error:)]) {
		NSError* error = nil;
		[session setCategory:AVAudioSessionCategoryPlayback error:&error];
		if (error) {
			NSLog(@"[MCPE] audio session category setup failed: %@", error);
		}
	}

	if ([session respondsToSelector:@selector(setActive:error:)]) {
		NSError* error = nil;
		[session setActive:YES error:&error];
		if (error) {
			NSLog(@"[MCPE] audio session activation failed: %@", error);
		}
	} else if ([session respondsToSelector:@selector(setActive:)]) {
		[session setActive:YES];
	}
}

// ---------------------------------------------------------------------------
// Globals shared with AppPlatform_iOS.mm (keyboard bridge).
// ---------------------------------------------------------------------------
static AppPlatform_iOS* g_platform = nullptr;
static NinecraftApp* g_app = nullptr;

@class MCPEViewController;
static MCPEViewController* g_viewController = nil;

// ===========================================================================
// View controller: owns the EAGLView, the CADisplayLink loop, touch input,
// and a hidden keyboard field for the soft keyboard.
// ===========================================================================
@interface MCPEViewController : UIViewController <UITextFieldDelegate> {
	UIView* _rootView;
	EAGLView* _glView;
	CADisplayLink* _displayLink;
	BOOL _hasInit;
	BOOL _paused;
	UITextField* _keyboardField;
	UITouch* _mouseTouch;
}
- (void)showKeyboardWithText:(NSString*)text;
- (void)hideKeyboard;
- (void)pauseRendering;
- (void)resumeRendering;
@end

@implementation MCPEViewController

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
	return UIInterfaceOrientationMaskLandscape;
}

- (BOOL)shouldAutorotate {
	return YES;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
	return UIInterfaceOrientationLandscapeLeft;
}

- (BOOL)prefersStatusBarHidden { return YES; }

- (void)loadView {
	CGRect bounds = [UIScreen mainScreen].bounds;
	_rootView = [[UIView alloc] initWithFrame:bounds];
	_rootView.autoresizingMask = UIViewAutoresizingFlexibleWidth |
	                             UIViewAutoresizingFlexibleHeight;
	_rootView.backgroundColor = [UIColor blackColor];
	_rootView.multipleTouchEnabled = YES;
	self.view = _rootView;

	_glView = [[EAGLView alloc] initWithFrame:bounds];
	_glView.autoresizingMask = UIViewAutoresizingFlexibleWidth |
	                           UIViewAutoresizingFlexibleHeight;
	_glView.multipleTouchEnabled = YES;
	[_rootView addSubview:_glView];

	_keyboardField = [[UITextField alloc] initWithFrame:CGRectMake(0, 0, 280.0f, 40.0f)];
	_keyboardField.center = CGPointMake(bounds.size.width * 0.5f, bounds.size.height * 0.5f);
	_keyboardField.autoresizingMask = UIViewAutoresizingFlexibleTopMargin |
	                                  UIViewAutoresizingFlexibleBottomMargin |
	                                  UIViewAutoresizingFlexibleLeftMargin |
	                                  UIViewAutoresizingFlexibleRightMargin;
	_keyboardField.delegate = self;
	_keyboardField.backgroundColor = [UIColor clearColor];
	_keyboardField.opaque = NO;
	_keyboardField.alpha = 0.01f;
	_keyboardField.borderStyle = UITextBorderStyleNone;
	_keyboardField.clearsOnBeginEditing = NO;
	_keyboardField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	_keyboardField.autocorrectionType = UITextAutocorrectionTypeNo;
	_keyboardField.spellCheckingType = UITextSpellCheckingTypeNo;
	_keyboardField.keyboardType = UIKeyboardTypeDefault;
	_keyboardField.keyboardAppearance = UIKeyboardAppearanceDefault;
	_keyboardField.returnKeyType = UIReturnKeyDefault;
	_keyboardField.enablesReturnKeyAutomatically = NO;
	[_rootView addSubview:_keyboardField];
}

- (CGFloat)scale {
	if ([_glView respondsToSelector:@selector(contentScaleFactor)])
		return _glView.contentScaleFactor;
	return 1.0f;
}

- (void)viewDidLoad {
	[super viewDidLoad];
	[_glView createFramebuffer];

	g_platform->setScreenSize(_glView.backingWidth, _glView.backingHeight,
	                          [self scale]);

	_displayLink = [CADisplayLink displayLinkWithTarget:self
	                                           selector:@selector(tick:)];
	[_displayLink addToRunLoop:[NSRunLoop currentRunLoop]
	                   forMode:NSDefaultRunLoopMode];
}

- (void)tick:(CADisplayLink*)link {
	if (_paused) {
		return;
	}

	[_glView setFramebuffer];

	if (!_hasInit) {
		_hasInit = YES;
		g_app->init();
		g_app->setSize(_glView.backingWidth, _glView.backingHeight);
	}

	g_app->update();

	[_glView presentFramebuffer];

	if (g_app->wantToQuit()) {
		[_displayLink invalidate];
		_displayLink = nil;
	}
}

- (void)pauseRendering {
	_paused = YES;
	_mouseTouch = nil;
	[_displayLink setPaused:YES];
}

- (void)resumeRendering {
	[_glView makeCurrent];
	[_glView destroyFramebuffer];
	[_glView createFramebuffer];
	[_displayLink setPaused:NO];
	_paused = NO;
}

- (void)feedTouches:(NSSet*)touches pressed:(BOOL)pressed moved:(BOOL)moved {
	CGFloat scale = [self scale];
	UITouch* primaryTouch = _mouseTouch;
	if (!primaryTouch && [touches count] > 0) {
		primaryTouch = [touches anyObject];
		if (pressed && !moved) {
			_mouseTouch = primaryTouch;
		}
	}

	for (UITouch* t in touches) {
		CGPoint p = [t locationInView:_glView];
		int16_t x = (int16_t)(p.x * scale);
		int16_t y = (int16_t)(p.y * scale);
		int8_t pid = (int8_t)(((intptr_t)t >> 4) & 0x7);
		if (moved) {
			Multitouch::feed(0, 0, x, y, pid);
		} else {
			Multitouch::feed(1, pressed ? 1 : 0, x, y, pid);
		}
	}

	if (primaryTouch && [touches containsObject:primaryTouch]) {
		CGPoint p = [primaryTouch locationInView:_glView];
		int16_t x = (int16_t)(p.x * scale);
		int16_t y = (int16_t)(p.y * scale);
		if (moved) {
			Mouse::feed(0, 0, x, y);
		} else {
			Mouse::feed(1, pressed ? 1 : 0, x, y);
			if (!pressed) {
				_mouseTouch = nil;
			}
		}
	}
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)e {
	[self feedTouches:touches pressed:YES moved:NO];
}
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)e {
	[self feedTouches:touches pressed:YES moved:YES];
}
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)e {
	[self feedTouches:touches pressed:NO moved:NO];
}
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)e {
	if (_mouseTouch && [touches containsObject:_mouseTouch]) {
		_mouseTouch = nil;
	}
	[self feedTouches:touches pressed:NO moved:NO];
}

- (void)showKeyboardWithText:(NSString*)text {
	_keyboardField.text = text ? text : @"";
	[_keyboardField becomeFirstResponder];
}
- (void)hideKeyboard {
	[_keyboardField resignFirstResponder];
}

- (BOOL)textField:(UITextField*)textField
shouldChangeCharactersInRange:(NSRange)range
replacementString:(NSString*)text {
	if (!text) {
		return NO;
	}

	if ([text isEqualToString:@"\n"] || [text isEqualToString:@"\r"]) {
		Keyboard::feed(13, 1);
		Keyboard::feed(13, 0);
		return NO;
	}

	// Let the UITextField own its own text and cursor. Returning YES keeps the
	// native buffer authoritative so the range iOS reports for the next edit
	// stays correct; the game buffer is a mirror driven by the feed() calls
	// below. (Returning NO froze the native text and desynced the two buffers,
	// which is what made the last remaining character impossible to delete.)
	if ([text length] == 0) {
		NSUInteger count = range.length > 0 ? range.length : 1;
		for (NSUInteger i = 0; i < count; ++i) {
			Keyboard::feed(8, 1);
			Keyboard::feed(8, 0);
		}
		return YES;
	}

	Keyboard::feedText(std::string([text UTF8String]), 0);
	return YES;
}

- (void)updateTextBoxText:(NSString*)text {
	_keyboardField.text = text ? text : @"";
}

- (BOOL)textFieldShouldReturn:(UITextField*)textField {
	Keyboard::feed(13, 1);
	Keyboard::feed(13, 0);
	return NO;
}

@end

// ===========================================================================
// App delegate.
// ===========================================================================
@interface MCPEAppDelegate : NSObject <UIApplicationDelegate> {
	UIWindow* _window;
}
@end

@implementation MCPEAppDelegate

- (BOOL)application:(UIApplication*)application
didFinishLaunchingWithOptions:(NSDictionary*)options {
	[application setStatusBarHidden:YES];
	[application setIdleTimerDisabled:YES];
	MCPEConfigureAudioSession();

	_window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];

	g_viewController = [[MCPEViewController alloc] init];
	if ([_window respondsToSelector:@selector(setRootViewController:)]) {
		_window.rootViewController = g_viewController;
	} else {
		[_window addSubview:g_viewController.view];
	}
	[_window makeKeyAndVisible];
	return YES;
}

- (UIInterfaceOrientationMask)application:(UIApplication*)application
	supportedInterfaceOrientationsForWindow:(UIWindow*)window {
	return UIInterfaceOrientationMaskLandscape;
}

- (void)applicationWillResignActive:(UIApplication*)application {
	if (g_viewController) {
		[g_viewController pauseRendering];
	}
	if (g_app) {
		g_app->pauseGame(1);
		if (g_app->level) {
			g_app->cancelLocateMultiplayer();
		}
	}
}

- (void)applicationDidEnterBackground:(UIApplication*)application {
	if (g_viewController) {
		[g_viewController pauseRendering];
	}
}

- (void)applicationDidBecomeActive:(UIApplication*)application {
	if (g_viewController) {
		[g_viewController resumeRendering];
	}
}

- (void)applicationWillEnterForeground:(UIApplication*)application {
	if (g_viewController) {
		[g_viewController resumeRendering];
	}
}

@end

void MCPE_iOS_ShowKeyboard(const std::string& current) {
	NSString* s = [NSString stringWithUTF8String:current.c_str()];
	[g_viewController performSelectorOnMainThread:@selector(showKeyboardWithText:)
	                                   withObject:s
	                                waitUntilDone:NO];
}
void MCPE_iOS_HideKeyboard(void) {
	[g_viewController performSelectorOnMainThread:@selector(hideKeyboard)
	                                   withObject:nil
	                                waitUntilDone:NO];
}
void MCPE_iOS_UpdateTextBoxText(const std::string& current) {
	NSString* s = [NSString stringWithUTF8String:current.c_str()];
	[g_viewController performSelectorOnMainThread:@selector(updateTextBoxText:)
	                                   withObject:s
	                                waitUntilDone:NO];
}

int main(int argc, char* argv[]) {
	@autoreleasepool {
		struct timeval start;
		gettimeofday(&start, 0);
		startedAtSec = start.tv_sec;

		static AppPlatform_iOS platform;
		g_platform = &platform;

		NinecraftApp* app = new NinecraftApp();
		g_app = app;

		AppContext ctx;
		ctx.platform = &platform;
		app->context = ctx;

		@autoreleasepool {
			NSArray* dirs = NSSearchPathForDirectoriesInDomains(
				NSDocumentDirectory, NSUserDomainMask, YES);
			if ([dirs count] > 0) {
				std::string docs([[dirs objectAtIndex:0] UTF8String]);
				app->dataPathMaybe = docs;
				app->field_CC4 = docs;
			}
		}

		return UIApplicationMain(argc, argv, nil, @"MCPEAppDelegate");
	}
}
#endif
