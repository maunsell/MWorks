//
//  MWDebuggerWindowController.m
//  MonkeyWorksDebugger
//
//  Created by David Cox on 2/12/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "RatBehaviorControlPanelController.h"

#import "MonkeyWorksCocoa/MWClientServerBase.h"
//#import "MonkeyWorksCore/Client.h"
#import "MonkeyWorksCocoa/MWCocoaEvent.h"


@implementation RatBehaviorControlPanelController

@synthesize staircaseStates;

- (void)awakeFromNib {
//- (void)setDelegate:(id)new_delegate{
//	delegate = new_delegate;
	
	#define RAT_CONTROL_PANEL_CALLBACK_KEY	@"ratcontrolpanelcallback"
	#define RESERVED_CODEC_CODE	0
	
	self.staircaseStates = [NSArray arrayWithObjects:ACTIVE_STRING, DISABLED_STRING, FROZEN_STRING, Nil];
	
	[delegate registerEventCallbackWithReceiver:self 
					  andSelector:@selector(codecUpdated:)
						   andKey:RAT_CONTROL_PANEL_CALLBACK_KEY
				  forVariableCode:RESERVED_CODEC_CODE];
}


- (void) codecUpdated:(MWCocoaEvent *)event {
	//int code = [event code];
	//if(code == RESERVED_CODEC_CODE){
	
	NSMutableArray *names = [delegate variableNames];

	for(int i = 0; i < [names count]; i++){
		[self willChangeValueForKey:[names objectAtIndex:i]];
	}
	
	for(int i = 0; i < [names count]; i++){
		[self didChangeValueForKey:[names objectAtIndex:i]];
	}


}


/*- (id) valueForKey:(NSString *)key{
	return [delegate valueForKey:key];
}

- (void) setValue:(id)val forKey:(NSString *)key{
	[delegate setValue:val forKey:key];
}*/

- (IBAction) deliverLeft:(id)sender {
	
	[delegate setValue:[delegate valueForKeyPath:@"variables.minimal_reward_duration"]
			forKeyPath:@"variables.LickOutput1"];
}

- (IBAction) deliverRight:(id)sender {
	[delegate setValue:[delegate valueForKeyPath:@"variables.minimal_reward_duration"]
			forKeyPath:@"variables.LickOutput3"];
}


@end
