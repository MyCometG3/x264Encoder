/*
 *  UIController.h
 *  UIbundle
 *
 *  Created by Takashi Mochizuki on 07/12/02.
 *  Copyright 2007-2010 MyCometG3. All rights reserved.
 *
 */

#ifndef UIController_h
#define UIController_h

#import "UIModel.h"

@interface kUIControllerNAME : NSObject
{
    IBOutlet id Dialog;
    IBOutlet id ObjectCtrl;
	IBOutlet id popupCoreVF;
	NSString* bundleID;
	
	IBOutlet id paspclapSheet;
	IBOutlet id paspclapCtrl;
	
	IBOutlet id presetSheet;
	IBOutlet id presetCtrl;
}

- (void)resetToParams:(params*)inParams;
- (params*)currentParams;
- (int)showModal;

- (NSString*)fourCC;
- (void)setFourCC:(NSString*)fourCC;
- (NSString*)bundleID;
- (void)setBundleID:(NSString*)newBundleID;

- (IBAction)buttonCancel:(id)sender;
- (IBAction)buttonOK:(id)sender;

- (IBAction)buttonPush:(id)sender;
- (IBAction)buttonPop:(id)sender;

- (IBAction)buttonAspect:(id)sender;
- (IBAction)buttonAspectOK:(id)sender;
- (IBAction)buttonAspectCancel:(id)sender;
- (void)didEndPaspclapSheet:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void*)contextInfo;

- (IBAction)buttonPreset:(id)sender;
- (IBAction)buttonPresetOK:(id)sender;
- (IBAction)buttonPresetCancel:(id)sender;
- (void)didEndPresetSheet:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void*)contextInfo;

- (IBAction)launchPrefPane:(id)sender;

- (void)updateCoreVFMenu:(NSNotification *)note;

@end

#endif // UIController_h