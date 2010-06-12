/*
 *  UIController.m
 *  UIbundle
 *
 *  Created by Takashi Mochizuki on 07/12/02.
 *  Copyright 2007-2010 MyCometG3. All rights reserved.
 *
 */

#import "UIController.h"

@implementation kUIControllerNAME

- (id)init
{
	if(self = [super init]) {
		// Load nib file
		BOOL ret = [NSBundle loadNibNamed:kUIbundleNAME owner:self];
		if(!ret) {
			NSLog(@"ERROR: Failed to load nib.");
			goto bail;
		}
		
		kUIModelNAME *theModel = [[[kUIModelNAME alloc] init] autorelease];
		[ObjectCtrl setContent:theModel];
	}
	
bail:
	return self;
}

- (void)dealloc
{
	[ObjectCtrl setContent:nil];
	
	[super dealloc];
}

- (void)resetToParams:(params*)inParams
{
	kUIModelNAME *theModel = [ObjectCtrl content];
	[theModel resetToParams:(params*)inParams];
}

- (params*)currentParams
{
	kUIModelNAME *theModel = [ObjectCtrl content];
	params* outParams = [theModel currentParams];
	
	return outParams;
}

- (int)showModal
{
	int ret = NSCancelButton;
	
	[self updateCoreVFMenu:NULL];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(updateCoreVFMenu:)
		name:NSPopUpButtonWillPopUpNotification object:popupCoreVF];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(activateDialog:) 
		name:NSApplicationDidBecomeActiveNotification object:nil];	/* required for stupid StdVideoCompSettings Dialog */
	
#if 1
	// Using Carbon window manager approach ; better compatibility.
	
	WindowActivationScope parentScope = 0;	// UInt32
	WindowRef parentWindowRef = ActiveNonFloatingWindow();
	if(parentWindowRef) {
		GetWindowActivationScope(parentWindowRef, &parentScope);
		SetWindowActivationScope(parentWindowRef, kWindowActivationScopeNone);
	}
	
  #if 1
	[Dialog setLevel:kCGUtilityWindowLevel];
	[Dialog makeKeyAndOrderFront:self];
	ret = [NSApp runModalForWindow:Dialog];
	[Dialog orderOut:self];
  #endif
	
	if(parentWindowRef) {
		SetWindowActivationScope(parentWindowRef, parentScope);
		SelectWindow(parentWindowRef);
	}
	
#else
	// Using Cocoa NSWindow approach ; Crash with some Carbon app like JES Deinterlacer...
	
	NSWindow * parentWindow = NULL;
	WindowRef parentWindowRef = ActiveNonFloatingWindow();
	if(parentWindowRef) {
		//CFRetain(parentWindowRef);	/* This breaks most of all application... Why? */
		parentWindow = [[NSWindow alloc] initWithWindowRef:parentWindowRef];	/* it seems to be buggy... */
	}
	
  #if 1
	if(parentWindow) {
		[parentWindow addChildWindow:Dialog ordered:NSWindowAbove];
		[Dialog setParentWindow:parentWindow];
	}
	
	[Dialog setLevel:kCGUtilityWindowLevel];
	[Dialog makeKeyAndOrderFront:self];
	ret = [NSApp runModalForWindow:Dialog];
	[Dialog orderOut:self];
	
	if(parentWindow) {
		[Dialog setParentWindow:NULL];
		[parentWindow removeChildWindow:Dialog];
	}	
	if(parentWindow) {
		[parentWindow makeKeyAndOrderFront:self];
		[parentWindow display];
	}
  #endif
	
	if(parentWindow) {
		[parentWindow release];		/* This releases parentWindowRef! */
	}
#endif
	
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	return ret;
}

- (NSString*)fourCC
{
	kUIModelNAME *theModel = [ObjectCtrl content];
	return [theModel valueForKey:@"fourCC"];
}

- (void)setFourCC:(NSString*)fourCC
{
	kUIModelNAME *theModel = [ObjectCtrl content];
	if([fourCC isEqualToString:@"avc1"]) {
		[theModel setValue:@"avc1" forKey:@"fourCC"];
		[theModel resetToDefault];
	} else if([fourCC isEqualToString:@"mp4v"]) {
		[theModel setValue:@"mp4v" forKey:@"fourCC"];
		[theModel resetToDefault];
	} else if([fourCC isEqualToString:@"xvid"]) {
		[theModel setValue:@"xvid" forKey:@"fourCC"];
		[theModel resetToDefault];
	} else {
		NSLog(@"ERROR: Unsupported codec:%@", fourCC);
	}
}

- (NSString*)bundleID
{
	return bundleID;
}

- (void)setBundleID:(NSString*)newBundleID
{
	if(newBundleID) {
		[bundleID autorelease];
		bundleID = [newBundleID retain];
	} else {
		NSLog(@"ERROR: Invalid BundleIdentifier:%@", newBundleID);
	}
}

#pragma mark -

- (IBAction)buttonCancel:(id)sender
{
	[NSApp stopModalWithCode:NSCancelButton];
}

- (IBAction)buttonOK:(id)sender
{
	[NSApp stopModalWithCode:NSOKButton];
}

- (IBAction)buttonDefault:(id)sender
{
	NSAlert* alert = [NSAlert alertWithMessageText:@"Reset to Default values..." 
					defaultButton:@"OK" alternateButton:@"Cancel" otherButton:NULL
					informativeTextWithFormat:@"Current settings will be lost.\n\n* Option+Click OK to keep Behavior/Tagging params."];
	[alert beginSheetModalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(alertDefaultDidEnd:returnCode:contextInfo:) 
					contextInfo:nil];
}

- (void)alertDefaultDidEnd:(NSAlert*)alert returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( returnCode != NSAlertDefaultReturn ) return;
	
	kUIModelNAME *theModel = [ObjectCtrl content];
	[theModel resetToDefault];
}

- (IBAction)buttoniPod:(id)sender
{
	NSAlert* alert = [NSAlert alertWithMessageText:@"Reset to iPod Default values..." 
					defaultButton:@"OK" alternateButton:@"Cancel" otherButton:NULL
					informativeTextWithFormat:@"Current settings will be lost.\n\n* Option+Click OK to keep Behavior/Tagging params.\n* Ensure: Turn off 'Frame reordering'\n* Ensure: Set Data Rate less then 1500kbps."];
	[alert beginSheetModalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(alertiPodDidEnd:returnCode:contextInfo:) 
					contextInfo:nil];
}

- (void)alertiPodDidEnd:(NSAlert*)alert returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( returnCode != NSAlertDefaultReturn ) return;
	
	kUIModelNAME *theModel = [ObjectCtrl content];
	[theModel resetToiPod];
}

- (IBAction)buttonDefaultTuned:(id)sender
{
	NSAlert* alert = [NSAlert alertWithMessageText:@"Reset to Default(Tuned) values..." 
					defaultButton:@"OK" alternateButton:@"Cancel" otherButton:NULL
					informativeTextWithFormat:@"Current settings will be lost.\n\n* Option+Click OK to keep Behavior/Tagging params."];
	[alert beginSheetModalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(alertDefaultTunedDidEnd:returnCode:contextInfo:) 
					contextInfo:nil];
}

- (void)alertDefaultTunedDidEnd:(NSAlert*)alert returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( returnCode != NSAlertDefaultReturn ) return;
	
	kUIModelNAME *theModel = [ObjectCtrl content];
	[theModel resetToDefaultTuned];
}

- (IBAction)buttoniPodTuned:(id)sender
{
	NSAlert* alert = [NSAlert alertWithMessageText:@"Reset to iPod(Tuned) values..." 
					defaultButton:@"OK" alternateButton:@"Cancel" otherButton:NULL
					informativeTextWithFormat:@"Current settings will be lost.\n\n* Option+Click OK to keep Behavior/Tagging params.\n* Ensure: Turn off 'Frame reordering'\n* Ensure: Set Data Rate less then 1500kbps."];
	[alert beginSheetModalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(alertiPodTunedDidEnd:returnCode:contextInfo:) 
					contextInfo:nil];
}

- (void)alertiPodTunedDidEnd:(NSAlert*)alert returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( returnCode != NSAlertDefaultReturn ) return;
	
	kUIModelNAME *theModel = [ObjectCtrl content];
	[theModel resetToiPodTuned];
}

- (IBAction)buttonx264Preset:(id)sender
{
	NSAlert* alert = [NSAlert alertWithMessageText:@"Reset to x264 preset/tune values..." 
					defaultButton:@"OK" alternateButton:@"Cancel" otherButton:NULL
					informativeTextWithFormat:@"Current settings will be lost.\n\n* Option+Click OK to keep Behavior/Tagging params."];
	[alert beginSheetModalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(alertx264PresetDidEnd:returnCode:contextInfo:) 
					contextInfo:nil];
}

- (void)alertx264PresetDidEnd:(NSAlert*)alert returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( returnCode != NSAlertDefaultReturn ) return;
	
	kUIModelNAME *theModel = [ObjectCtrl content];
	{
		NSNumber* X264PRESET = [theModel valueForKey:@"X264PRESET"];
		NSNumber* X264TUNE = [theModel valueForKey:@"X264TUNE"];
		[theModel resetToDefault];
		[theModel setValue:X264PRESET forKey:@"X264PRESET"];
		[theModel setValue:X264TUNE forKey:@"X264TUNE"];
	}
	[theModel resetTox264Preset];
}

#pragma mark -

- (IBAction)buttonPush:(id)sender
{
	int tag = [sender tag];
	tag = (tag ? tag : 1);
	NSString* theKey = [NSString stringWithFormat:@"Setting(%d)", tag];
	
	if(!bundleID || ![bundleID length]) {
		NSLog(@"ERROR: bundleID is not availale.");
		return;
	}
	
	kUIModelNAME *theModel = [ObjectCtrl content];
	NSData* theData = [NSKeyedArchiver archivedDataWithRootObject:theModel];
	if(!theData) {
		NSLog(@"ERROR: Failed to archive UIModel object"); NSBeep();
		return;
	}
	
	// 
	NSAlert* alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:@"Save %@...", theKey] 
					defaultButton:@"OK" alternateButton:@"Cancel" otherButton:NULL
					informativeTextWithFormat:[NSString stringWithFormat:@"Current settings will be stored as %@.", theKey]];
	[alert beginSheetModalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(alertPushDidEnd:returnCode:contextInfo:) 
					contextInfo:[[NSArray arrayWithObjects:theKey,theData,nil] retain]];
}

- (void)alertPushDidEnd:(NSAlert*)alert returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( returnCode != NSAlertDefaultReturn ) return;
	
	NSString* theKey = (NSString*)[(NSArray*)contextInfo objectAtIndex:0];
	NSData* theData = (NSData*)[(NSArray*)contextInfo objectAtIndex:1];
	CFPreferencesSetAppValue( (CFStringRef)theKey, (CFDataRef)theData, (CFStringRef)bundleID );
	CFPreferencesAppSynchronize( (CFStringRef)bundleID );
	
	[(NSArray*)contextInfo release];
}

- (IBAction)buttonPop:(id)sender
{
	int tag = [sender tag];
	tag = (tag ? tag : 1);
	NSString* theKey = [NSString stringWithFormat:@"Setting(%d)", tag];
	
	if(!bundleID || ![bundleID length]) {
		NSLog(@"ERROR: bundleID is not availale.");
		return;
	}
	
	CFPropertyListRef value = NULL;
	value = CFPreferencesCopyAppValue( (CFStringRef)theKey, (CFStringRef)bundleID );
	if( !(value && CFGetTypeID(value) == CFDataGetTypeID()) ) {
		NSLog(@"ERROR: %@ is not available.", theKey); NSBeep();
		if( value ) CFRelease(value);
		return;
	}
	
	kUIModelNAME *newModel = NULL;
	if( value ) {
		newModel = [NSKeyedUnarchiver unarchiveObjectWithData:(NSData*)value];
		CFRelease(value);
	}
	if( !newModel ) {
		NSLog(@"ERROR: Corrupted Data detected."); NSBeep();
		return;
	}
	
	NSString* newFourCC = NULL;
	newFourCC = [newModel valueForKey:@"fourCC"];
	if([newFourCC compare:[self fourCC]] != NSOrderedSame) {
		NSLog(@"ERROR: Invalid Data detected."); NSBeep();
		return;
	}
	
#if 1
	// Check Level Value change from 1.2.7
	if( [[newModel valueForKey:@"LEVEL"] intValue] < 9 ) {
		switch( [[newModel valueForKey:@"LEVEL"] intValue] ) {
		case 1:
			[newModel setValue:[NSNumber numberWithInt:13] forKey:@"LEVEL"];
			break;
		case 2:
			[newModel setValue:[NSNumber numberWithInt:21] forKey:@"LEVEL"];
			break;
		case 3:
			[newModel setValue:[NSNumber numberWithInt:30] forKey:@"LEVEL"];
			break;
		case 4:
			[newModel setValue:[NSNumber numberWithInt:31] forKey:@"LEVEL"];
			break;
		case 5:
			[newModel setValue:[NSNumber numberWithInt:41] forKey:@"LEVEL"];
			break;
		case 6:
			[newModel setValue:[NSNumber numberWithInt:51] forKey:@"LEVEL"];
			break;
		case 9:
		default:
			[newModel setValue:[NSNumber numberWithInt:9] forKey:@"LEVEL"];
			break;
		}
	}
	
	// Check new values added from 1.2.8
	NSNumber *check128 = [newModel valueForKey:@"OVERRIDECRFQSCALE"];
	if( !check128 ) {
		if( [newModel isAvc1] ) {
			[newModel setValue:[NSNumber numberWithChar:0] forKey:@"OVERRIDECRFQSCALE"];
			[newModel setValue:[NSNumber numberWithChar:23] forKey:@"USERCRFQSCALE"];
			[newModel setValue:[NSNumber numberWithChar:1] forKey:@"OVERRIDEQMIN"];
			[newModel setValue:[NSNumber numberWithChar:10] forKey:@"USERQMIN"];
		} else {
			[newModel setValue:[NSNumber numberWithChar:0] forKey:@"OVERRIDECRFQSCALE"];
			[newModel setValue:[NSNumber numberWithChar:2] forKey:@"USERCRFQSCALE"];
			[newModel setValue:[NSNumber numberWithChar:0] forKey:@"OVERRIDEQMIN"];
			[newModel setValue:[NSNumber numberWithChar:2] forKey:@"USERQMIN"];
		}
	}
#endif
	
	// 
	NSAlert* alert = [NSAlert alertWithMessageText:[NSString stringWithFormat:@"Load %@...", theKey] 
					defaultButton:@"OK" alternateButton:@"Cancel" otherButton:NULL 
					informativeTextWithFormat:@"Current settings will be lost."];
	[alert beginSheetModalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(alertPopDidEnd:returnCode:contextInfo:) 
					contextInfo:[[NSArray arrayWithObjects:newModel,nil] retain]];
}

- (void)alertPopDidEnd:(NSAlert*)alert returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( returnCode != NSAlertDefaultReturn ) return;
	
	kUIModelNAME* newModel = (kUIModelNAME*)[(NSArray*)contextInfo objectAtIndex:0];
	
	[ObjectCtrl setContent:newModel];
	
	[(NSArray*)contextInfo release];
}

#pragma mark -

- (IBAction)buttonAspect:(id)sender
{
	// Build aspect model
	id objectModel = [ObjectCtrl selection];
	id paspclapModel = [paspclapCtrl selection];
	
	[paspclapModel setValue:[objectModel valueForKey:@"USEASPECTRATIO"]			forKey:@"USEASPECTRATIO"];
	[paspclapModel setValue:[objectModel valueForKey:@"USECLEANAPERTURE"]		forKey:@"USECLEANAPERTURE"];
	[paspclapModel setValue:[objectModel valueForKey:@"TAGASPECTRATIO"]			forKey:@"TAGASPECTRATIO"];
	[paspclapModel setValue:[objectModel valueForKey:@"TAGCLEANAPERTURE"]		forKey:@"TAGCLEANAPERTURE"];
	
	[paspclapModel setValue:[objectModel valueForKey:@"HSPACING"]				forKey:@"HSPACING"];
	[paspclapModel setValue:[objectModel valueForKey:@"VSPACING"]				forKey:@"VSPACING"];
	
	[paspclapModel setValue:[objectModel valueForKey:@"CLEANAPERTUREWIDTHN"]	forKey:@"CLEANAPERTUREWIDTHN"];
	[paspclapModel setValue:[objectModel valueForKey:@"CLEANAPERTUREWIDTHD"]	forKey:@"CLEANAPERTUREWIDTHD"];
	[paspclapModel setValue:[objectModel valueForKey:@"CLEANAPERTUREHEIGHTN"]	forKey:@"CLEANAPERTUREHEIGHTN"];
	[paspclapModel setValue:[objectModel valueForKey:@"CLEANAPERTUREHEIGHTD"]	forKey:@"CLEANAPERTUREHEIGHTD"];
	[paspclapModel setValue:[objectModel valueForKey:@"HORIZOFFN"]				forKey:@"HORIZOFFN"];
	[paspclapModel setValue:[objectModel valueForKey:@"HORIZOFFD"]				forKey:@"HORIZOFFD"];
	[paspclapModel setValue:[objectModel valueForKey:@"VERTOFFN"]				forKey:@"VERTOFFN"];
	[paspclapModel setValue:[objectModel valueForKey:@"VERTOFFD"]				forKey:@"VERTOFFD"];
	
	[NSApp beginSheet:paspclapSheet modalForWindow:Dialog modalDelegate:self 
					didEndSelector:@selector(didEndSheet:returnCode:contextInfo:) 
					contextInfo:nil];
}

- (IBAction)buttonAspectOK:(id)sender
{
	[NSApp endSheet:[sender window] returnCode:NSAlertDefaultReturn];
}

- (IBAction)buttonAspectCancel:(id)sender
{
	[NSApp endSheet:[sender window] returnCode:NSAlertAlternateReturn];
}

- (void)didEndSheet:(NSWindow*)sheet returnCode:(int)returnCode contextInfo:(void*)contextInfo
{
	if( sheet == paspclapSheet ) {
		[sheet orderOut:self];
		if( returnCode != NSAlertDefaultReturn ) return;
		
		// Retreive New values
		id objectModel = [ObjectCtrl selection];
		id paspclapModel = [paspclapCtrl selection];
		
		//
		[objectModel setValue:[paspclapModel valueForKey:@"USEASPECTRATIO"]		forKey:@"USEASPECTRATIO"];
		[objectModel setValue:[paspclapModel valueForKey:@"USECLEANAPERTURE"]	forKey:@"USECLEANAPERTURE"];
		[objectModel setValue:[paspclapModel valueForKey:@"TAGASPECTRATIO"]		forKey:@"TAGASPECTRATIO"];
		[objectModel setValue:[paspclapModel valueForKey:@"TAGCLEANAPERTURE"]	forKey:@"TAGCLEANAPERTURE"];
		
		// 
		switch ( [[objectModel valueForKey:@"TAGASPECTRATIO"] charValue] ) {
		case 11:	// 
			[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"VSPACING"];
			break;
		case 12:	// 
			[objectModel setValue:[NSNumber numberWithInt:10]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:11]	forKey:@"VSPACING"];
			break;
		case 13:	// 
			[objectModel setValue:[NSNumber numberWithInt:59]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:54]	forKey:@"VSPACING"];
			break;
		case 21:	// 
			[objectModel setValue:[NSNumber numberWithInt:4]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:3]	forKey:@"VSPACING"];
			break;
		case 22:	// 
			[objectModel setValue:[NSNumber numberWithInt:40]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:33]	forKey:@"VSPACING"];
			break;
		case 23:	// 
			[objectModel setValue:[NSNumber numberWithInt:118]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:81]	forKey:@"VSPACING"];
			break;
		case 24:	// 
			[objectModel setValue:[NSNumber numberWithInt:10]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:11]	forKey:@"VSPACING"];
			break;
		case 31:	// 
			[objectModel setValue:[NSNumber numberWithInt:113]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:118]	forKey:@"VSPACING"];
			break;
		case 32:	// 
			[objectModel setValue:[NSNumber numberWithInt:1018]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:1062]	forKey:@"VSPACING"];
			break;
		case 33:	// 
			[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"VSPACING"];
			break;
		case 34:	// 
			[objectModel setValue:[NSNumber numberWithInt:4]	forKey:@"HSPACING"];
			[objectModel setValue:[NSNumber numberWithInt:3]	forKey:@"VSPACING"];
			break;
		default:
			// use custom setting
			[objectModel setValue:[paspclapModel valueForKey:@"HSPACING"]	forKey:@"HSPACING"];
			[objectModel setValue:[paspclapModel valueForKey:@"VSPACING"]	forKey:@"VSPACING"];
			break;
		}
		
		// 
		[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"CLEANAPERTUREWIDTHD"];
		[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"CLEANAPERTUREHEIGHTD"];
		[objectModel setValue:[NSNumber numberWithInt:0]	forKey:@"HORIZOFFN"];
		[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"HORIZOFFD"];
		[objectModel setValue:[NSNumber numberWithInt:0]	forKey:@"VERTOFFN"];
		[objectModel setValue:[NSNumber numberWithInt:1]	forKey:@"VERTOFFD"];
		switch ( [[objectModel valueForKey:@"TAGCLEANAPERTURE"] charValue] ) {
		case 11:	// 
			[objectModel setValue:[NSNumber numberWithInt:640]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:480]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 12:	// 
			[objectModel setValue:[NSNumber numberWithInt:704]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:480]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 13:	// 
			[objectModel setValue:[NSNumber numberWithInt:2816]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:3]	forKey:@"CLEANAPERTUREWIDTHD"];
			[objectModel setValue:[NSNumber numberWithInt:480]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 21:	// 
			[objectModel setValue:[NSNumber numberWithInt:768]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:576]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 22:	// 
			[objectModel setValue:[NSNumber numberWithInt:41472]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:59]	forKey:@"CLEANAPERTUREWIDTHD"];
			[objectModel setValue:[NSNumber numberWithInt:576]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 31:	// 
			[objectModel setValue:[NSNumber numberWithInt:1888]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:1018]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 32:	// 
			[objectModel setValue:[NSNumber numberWithInt:1888]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:1017]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 41:	// 
			[objectModel setValue:[NSNumber numberWithInt:1888]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:1062]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		case 51:	// 
			[objectModel setValue:[NSNumber numberWithInt:1248]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[NSNumber numberWithInt:702]	forKey:@"CLEANAPERTUREHEIGHTN"];
			break;
		default:	// custom setting
			[objectModel setValue:[paspclapModel valueForKey:@"CLEANAPERTUREWIDTHN"]	forKey:@"CLEANAPERTUREWIDTHN"];
			[objectModel setValue:[paspclapModel valueForKey:@"CLEANAPERTUREWIDTHD"]	forKey:@"CLEANAPERTUREWIDTHD"];
			[objectModel setValue:[paspclapModel valueForKey:@"CLEANAPERTUREHEIGHTN"]	forKey:@"CLEANAPERTUREHEIGHTN"];
			[objectModel setValue:[paspclapModel valueForKey:@"CLEANAPERTUREHEIGHTD"]	forKey:@"CLEANAPERTUREHEIGHTD"];
			[objectModel setValue:[paspclapModel valueForKey:@"HORIZOFFN"]				forKey:@"HORIZOFFN"];
			[objectModel setValue:[paspclapModel valueForKey:@"HORIZOFFD"]				forKey:@"HORIZOFFD"];
			[objectModel setValue:[paspclapModel valueForKey:@"VERTOFFN"]				forKey:@"VERTOFFN"];
			[objectModel setValue:[paspclapModel valueForKey:@"VERTOFFD"]				forKey:@"VERTOFFD"];
		}
	}
}

#pragma mark -

- (IBAction)launchPrefPane:(id)sender
{
	int status = 0;
	{
		NSArray* param = [NSArray arrayWithObjects:@"-e"
			, @"tell application \"System Preferences\" to activate", nil];
		NSPipe *pipe = [[NSPipe alloc] init];
		NSTask *task = [[NSTask alloc] init];
		[task setStandardError: pipe];
		[task setStandardOutput:pipe];
		[task setLaunchPath:@"/usr/bin/osascript"];
		[task setArguments:param];
		[task launch];
		[task waitUntilExit];
		status = [task terminationStatus];
		[task release];
		[pipe release];
	}
	if(status) NSBeep();
	{
		NSArray* param = [NSArray arrayWithObjects:@"-e"
			, @"tell application \"System Preferences\" to reveal pane \"com.MyCometG3.CoreVF\" ", nil];
		NSPipe *pipe = [[NSPipe alloc] init];
		NSTask *task = [[NSTask alloc] init];
		[task setStandardError: pipe];
		[task setStandardOutput:pipe];
		[task setLaunchPath:@"/usr/bin/osascript"];
		[task setArguments:param];
		[task launch];
		[task waitUntilExit];
		status = [task terminationStatus];
		[task release];
		[pipe release];
	}
	if(status) NSBeep();
}

- (void)updateCoreVFMenu:(NSNotification *)note;
{
	// update popup menu item's title as is
	CFStringRef appID = CFSTR("com.MyCometG3.CoreVF");
	if(CFPreferencesAppSynchronize(appID)) {
		NSString *keyStr, *filterStr;
		CFIndex index;
		for(index = 1; index <= 8; index++) {
			NSString *titleStr = NULL;
			keyStr = (NSString*)CFStringCreateWithFormat(kCFAllocatorDefault, NULL, CFSTR("Preset%d"), index);
			filterStr = (NSString*)CFPreferencesCopyAppValue((CFStringRef)keyStr, appID);
			
			if(filterStr) 
				titleStr = [NSString stringWithFormat:@"%@: %@", keyStr, filterStr];
			else
				titleStr = [NSString stringWithFormat:@"%@: -", keyStr];
			if(keyStr) CFRelease(keyStr);
			if(filterStr) CFRelease(filterStr);
			
			NSMenuItem *menu = [popupCoreVF itemAtIndex:[popupCoreVF indexOfItemWithTag:index]];
			[menu setTitle:titleStr];
		}
	}
}

- (void)activateDialog:(NSNotification *)aNotification
{
	// make Dialog window active
	[Dialog setLevel:kCGUtilityWindowLevel];
	[Dialog makeKeyAndOrderFront:self];
}

@end


#pragma mark -
#pragma mark bundle wrapper functions


kUIControllerNAME* theUI;


void InitializeNib()
{
	NSAutoreleasePool* pool	= [[NSAutoreleasePool alloc] init];
    
	if(theUI) [theUI release];
	
	// Keep instance in class variable
	theUI = [[kUIControllerNAME alloc] init];
    
    [pool release];
}

void SetFourCC(CFStringRef fourCC)
{
	NSAutoreleasePool* pool	= [[NSAutoreleasePool alloc] init];
    
	// Update UI suitable for fourCC
	[theUI setFourCC:(NSString*)fourCC];
    
    [pool release];
}

void SetBundleID(CFStringRef newBundleID)
{
	NSAutoreleasePool* pool	= [[NSAutoreleasePool alloc] init];
    
	// set bundleIdentifier for Push/Pop Support
	[theUI setBundleID:(NSString*)newBundleID];
    
    [pool release];
}

params* ShowDialog(params* inParams)
{
	params* outParams = NULL;
	
	NSAutoreleasePool* pool	= [[NSAutoreleasePool alloc] init];
  	
	// Initial Parameters
	[theUI resetToParams:inParams];
	
	// Run Modal session
	int ret = [theUI showModal];
	
	// Retrieve updated parameters
	if(ret == NSOKButton) {
		outParams = [theUI currentParams];
    }
	
    [pool release];
    
	return outParams;
}

void ReleaseNib()
{
	NSAutoreleasePool* pool	= [[NSAutoreleasePool alloc] init];
    
	// Free instance
	[theUI release];
    theUI = NULL;
    
	[pool release];
}

#pragma mark -
// Following are come from Sample code: SpellChecker-CarbonCocoa-bundled

typedef BOOL (*NSApplicationLoadFuncPtr)( void );

static OSStatus LoadFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr)
{
	OSStatus 	err;
	FSRef 		frameworksFolderRef;
	CFURLRef	baseURL;
	CFURLRef	bundleURL;
	
	if ( bundlePtr == nil )	return( -1 );
	
	*bundlePtr = nil;
	
	baseURL = nil;
	bundleURL = nil;
	
	err = FSFindFolder(kOnAppropriateDisk, kFrameworksFolderType, true, &frameworksFolderRef);
	if (err == noErr) {
		baseURL = CFURLCreateFromFSRef(kCFAllocatorSystemDefault, &frameworksFolderRef);
		if (baseURL == nil) {
			err = coreFoundationUnknownErr;
		}
	}
	if (err == noErr) {
		bundleURL = CFURLCreateCopyAppendingPathComponent(kCFAllocatorSystemDefault, baseURL, framework, false);
		if (bundleURL == nil) {
			err = coreFoundationUnknownErr;
		}
	}
	if (err == noErr) {
		*bundlePtr = CFBundleCreate(kCFAllocatorSystemDefault, bundleURL);
		if (*bundlePtr == nil) {
			err = coreFoundationUnknownErr;
		}
	}
	if (err == noErr) {
	    if ( ! CFBundleLoadExecutable( *bundlePtr ) ) {
			err = coreFoundationUnknownErr;
	    }
	}

	// Clean up.
	if (err != noErr && *bundlePtr != nil) {
		CFRelease(*bundlePtr);
		*bundlePtr = nil;
	}
	if (bundleURL != nil) {
		CFRelease(bundleURL);
	}	
	if (baseURL != nil) {
		CFRelease(baseURL);
	}	
	
	return err;
}

void	InitializeCocoa()
{
	CFBundleRef 				appKitBundleRef;
	NSApplicationLoadFuncPtr	myNSApplicationLoad;
	OSStatus					err;
	
	theUI = NULL;
	
	//	Load the "AppKit.framework" bundl to locate NSApplicationLoad
	err = LoadFrameworkBundle( CFSTR("AppKit.framework"), &appKitBundleRef );
	if (err != noErr) {
		fprintf(stderr, "InitializeCocoa: Failed to LoadFrameworkBundle()\n");
		goto FallbackMethod;
	}
	
	//	Manually load the Mach-O function pointers for the routines we will be using.
	myNSApplicationLoad	= (NSApplicationLoadFuncPtr)CFBundleGetFunctionPointerForName(appKitBundleRef,CFSTR("NSApplicationLoad"));
	if ( myNSApplicationLoad == NULL ) {
		fprintf(stderr, "InitializeCocoa: myNSApplicationLoad == NULL\n");
		goto FallbackMethod;
	}
	
//	fprintf(stderr, "InitializeCocoa: myNSApplicationLoad\n");
	(void) myNSApplicationLoad();
	
	return;

FallbackMethod:
	{
		NSApplication *NSApp;
		fprintf(stderr, "InitializeCocoa: FallbackMethod\n");
		NSApp = [NSApplication sharedApplication];
	}
}
