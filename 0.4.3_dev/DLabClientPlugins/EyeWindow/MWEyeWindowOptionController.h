//
//  MWEyeWindowOptionController.h
//  MonkeyWorksEyeWindow
//
//  Created by labuser on 11/1/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MonkeyWorksCore/GenericData.h"


@interface MWEyeWindowOptionController : NSWindowController {
	NSString * h;
	NSString * v;
	NSString * eye_state;
	
	NSTimeInterval time_of_tail;
	float update_rate;
	

}


- (id)init;
- (IBAction)updateVariables:(id)sender;
- (void)openSheet;
- (void)closeSheet;

- (NSTimeInterval)timeOfTail;
- (void)setTimeOfTail:(NSTimeInterval)new_time_of_tail;

- (float)updateRate;
- (void)setUpdateRate:(float)new_update_rate;

- (NSString *)v;
- (void)setV:(NSString *)_v;

- (NSString *)h;
- (void)setH:(NSString *)_h;

- (NSString *)eyeState;
- (void)setEyeState:(NSString *)_eye_state;

@end
