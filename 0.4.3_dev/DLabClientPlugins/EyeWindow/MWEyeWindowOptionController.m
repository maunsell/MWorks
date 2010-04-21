//
//  MWEyeWindowOptionController.m
//  MonkeyWorksEyeWindow
//
//  Created by labuser on 11/1/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "MWEyeWindowOptionController.h"
#import "MWEyeWindowController.h"

#define MW_EYE_WINDOW_UPDATE_RATE @"MonkeyWorksClient - Eye Window - update_rate"
#define MW_EYE_WINDOW_TIME_OF_TAIL @"MonkeyWorksClient - Eye Window - time_of_tail"
#define MW_EYE_WINDOW_H_NAME @"MonkeyWorksClient - Eye Window - h"
#define MW_EYE_WINDOW_V_NAME @"MonkeyWorksClient - Eye Window - v"
#define MW_EYE_WINDOW_EYE_STATE_NAME @"MonkeyWorksClient - Eye Window - eye_state"

@implementation MWEyeWindowOptionController

- (id)init{
    self = [super initWithWindowNibName:@"OptionWindow"];
    if(self) {
        [self window];
    }
    return self;
}

- (void)dealloc {
	[h release];
	[v release];
	[eye_state release];
	[super dealloc];
}

- (void)awakeFromNib {
	NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
	
	[self setUpdateRate:[ud floatForKey:MW_EYE_WINDOW_UPDATE_RATE]];
	[self setTimeOfTail:[ud floatForKey:MW_EYE_WINDOW_TIME_OF_TAIL]];	
	[self setV:[ud stringForKey:MW_EYE_WINDOW_V_NAME]];
	[self setH:[ud stringForKey:MW_EYE_WINDOW_H_NAME]];
	[self setEyeState:[ud stringForKey:MW_EYE_WINDOW_EYE_STATE_NAME]];
	
	if(h == nil) {
		[self setH:@""];
	}
	if(v == nil) {
		[self setV:@""];
	}
	if(eye_state == nil) {
		[self setEyeState:@""];
	}
}	

- (NSTimeInterval)timeOfTail {
	return time_of_tail;
}
- (void)setTimeOfTail:(NSTimeInterval)new_time_of_tail {
	time_of_tail = new_time_of_tail;
}

- (NSString *)h {
	return h;
}
- (void)setH:(NSString *)_h {
	[h release];
	h = [_h copy];
}

- (NSString *)v {
	return v;	
}
- (void)setV:(NSString *)_v {
	[v release];
	v = [_v copy];
}

- (NSString *)eyeState {
	return eye_state;
}

- (void)setEyeState:(NSString *)_eye_state {
	[eye_state release];
	eye_state = [_eye_state copy];
}

- (void)setUpdateRate:(float)new_update_rate {
	update_rate = new_update_rate;
}
- (float)updateRate {
	return update_rate;
}

- (IBAction)updateVariables:(id)sender {
	[self closeSheet];
    [[NSNotificationCenter defaultCenter] 
		postNotificationName:MWEyeWindowVariableUpdateNotification 
		object:nil userInfo:nil];

	NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
	[ud setObject:[self eyeState] forKey:MW_EYE_WINDOW_EYE_STATE_NAME];
	[ud setObject:[self h] forKey:MW_EYE_WINDOW_H_NAME];
	[ud setObject:[self v] forKey:MW_EYE_WINDOW_V_NAME];
	[ud setFloat:[self timeOfTail] forKey:MW_EYE_WINDOW_TIME_OF_TAIL];
	[ud setFloat:[self updateRate] forKey:MW_EYE_WINDOW_UPDATE_RATE];
	[ud synchronize];
}


- (void)openSheet{
	[NSApp beginSheet:[self window] modalForWindow:[NSApp mainWindow]
                         modalDelegate:nil didEndSelector:nil contextInfo:nil];
}


- (void)closeSheet {
    [[self window] orderOut:self];
    [NSApp endSheet:[self window]];
}



@end