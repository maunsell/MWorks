//
//  MWCalibratorWindow.m
//  MonkeyWorksCalibratorWindow
//
//  Created by Ben Kennedy on 5/3/08.
//  Copyright 2008 MIT. All rights reserved.
//

#import "MWCalibratorWindow.h"
#import "MWCalibratorWindowController.h"

// I'd love to not hard codes these in but I don't know where to find where these are defined
#define ARROW_KEY_LEFT 0x7b
#define ARROW_KEY_RIGHT 0x7c
#define ARROW_KEY_DOWN 0x7d
#define ARROW_KEY_UP 0x7e

@implementation MWCalibratorWindow

- (void)sendEvent:(NSEvent *)the_event {
	if([the_event type] == NSKeyDown) {
		float step = 0.1;
		
		if(([the_event modifierFlags] & NSShiftKeyMask) == NSShiftKeyMask) {
			step = 1;
		}
		
		if(([the_event modifierFlags] & NSControlKeyMask) == NSControlKeyMask) {
			switch([the_event keyCode]) {
				case ARROW_KEY_UP:
					[delegate setVOffset:[delegate vOffset]+step];
					[delegate updateCalibratorParams:self];
					break;				
				case ARROW_KEY_DOWN:
					[delegate setVOffset:[delegate vOffset]-step];
					[delegate updateCalibratorParams:self];
					break;				
				case ARROW_KEY_LEFT:
					[delegate setHOffset:[delegate hOffset]-step];
					[delegate updateCalibratorParams:self];
					break;				
				case ARROW_KEY_RIGHT:
					[delegate setHOffset:[delegate hOffset]+step];
					[delegate updateCalibratorParams:self];
					break;
				default:
					break;
			}
		} else if(([the_event modifierFlags] & NSCommandKeyMask) == NSCommandKeyMask) {
			switch([the_event keyCode]) {
				case ARROW_KEY_UP:
					[delegate setVGain:[delegate vGain]+step];
					[delegate updateCalibratorParams:self];
					break;				
				case ARROW_KEY_DOWN:
					[delegate setVGain:[delegate vGain]-step];
					[delegate updateCalibratorParams:self];
					break;				
				case ARROW_KEY_LEFT:
					[delegate setHGain:[delegate hGain]-step];
					[delegate updateCalibratorParams:self];
					break;				
				case ARROW_KEY_RIGHT:
					[delegate setHGain:[delegate hGain]+step];
					[delegate updateCalibratorParams:self];
					break;
				default:
					break;
			}
		}
	}
	
	[super sendEvent:the_event];
}

@synthesize delegate;

- (void)setDelegate:(id)new_delegate {
	if(![new_delegate respondsToSelector:@selector(updateCalibratorParams:)] ||
	   ![new_delegate respondsToSelector:@selector(hOffset)] ||
	   ![new_delegate respondsToSelector:@selector(hGain)] ||
	   ![new_delegate respondsToSelector:@selector(vOffset)] ||
	   ![new_delegate respondsToSelector:@selector(vGain)] ||
	   ![new_delegate respondsToSelector:@selector(setHOffset:)] ||
	   ![new_delegate respondsToSelector:@selector(setHGain:)] ||
	   ![new_delegate respondsToSelector:@selector(setVOffset:)] ||
	   ![new_delegate respondsToSelector:@selector(setVGain:)]) {
		[NSException raise:NSInternalInconsistencyException
					format:@"delegate for MWCalibratorWindow doesn't reqpond to required methods"];
	}
	   
	delegate = new_delegate;
}

@end
