#import "MWEyeWindowController.h"
#import "MWPlotView.h"

#import "MonkeyWorksCocoa/MWClientServerBase.h"
#import "MonkeyWorksCocoa/MWCocoaEvent.h"
#import "MonkeyWorksCore/GenericData.h"
#import "MonkeyWorksCore/VariableProperties.h"
#import "MonkeyWorksCore/StandardVariables.h"


#define	EYE_WINDOW_CALLBACK_KEY @"MWEyeWindowController callback key"
NSString * MWEyeWindowVariableUpdateNotification = @"MWEyeWindowVariableUpdateNotification";

@interface MWEyeWindowController(PrivateMethods)
- (void)cacheCodes;
- (void)serviceHEvent:(MWCocoaEvent *)event;
- (void)serviceVEvent:(MWCocoaEvent *)event;
- (void)serviceStmEvent:(MWCocoaEvent *)event;
- (void)serviceCalEvent:(MWCocoaEvent *)event;
- (void)serviceStateEvent:(MWCocoaEvent *)event;
- (void)codecReceived:(MWCocoaEvent *)codec_event;
@end

@implementation MWEyeWindowController


- (id) init {
	self = [super init];
	if (self != nil) {
		OptionWindow = [[MWEyeWindowOptionController alloc] init];
		
		
		eyeWindowStarted = NO;
	}
	return self;
}

- (void)awakeFromNib {
	[[NSNotificationCenter defaultCenter] addObserver:self 
											 selector:@selector(updateEyeVariableNames)
												 name:MWEyeWindowVariableUpdateNotification 
											   object:nil];
	
	[plotView setTimeOfTail:[OptionWindow timeOfTail]];
	[plotView setUpdateRate:[OptionWindow updateRate]];
	EYE_H = [[NSString alloc] initWithString:[OptionWindow h]];
	EYE_V = [[NSString alloc] initWithString:[OptionWindow v]];
	EYE_STATE = [[NSString alloc] initWithString:[OptionWindow eyeState]];
}


@synthesize delegate=delegate;

- (void)setDelegate:(id)new_delegate {
	if(![new_delegate respondsToSelector:@selector(unregisterCallbacksWithKey:)] ||
	   ![new_delegate respondsToSelector:@selector(registerEventCallbackWithReceiver:
												   andSelector:
												   andKey:)] ||
	   ![new_delegate respondsToSelector:@selector(registerEventCallbackWithReceiver:
												   andSelector:
												   andKey:
												   forVariableCode:)] ||
	   ![new_delegate respondsToSelector:@selector(codeForTag:)]) {
		[NSException raise:NSInternalInconsistencyException
					format:@"Delegate doesn't respond to required methods for MWEyeWindowController"];		
	}
	
	delegate = new_delegate;
	[delegate registerEventCallbackWithReceiver:self 
									andSelector:@selector(codecReceived:)
										 andKey:EYE_WINDOW_CALLBACK_KEY
								forVariableCode:[NSNumber numberWithInt:RESERVED_CODEC_CODE]];
}

-(IBAction)redrawPlot:(id)sender {
	[plotView setWidth:[scaleTextField intValue]];	
}

- (IBAction)acceptWidth:(id)sender {
	int width = [sender intValue];
	[plotView setWidth:width];
	[scaleSlider  setIntValue:width];
	[scaleTextField setIntValue:width];
}

- (IBAction)clear:(id)sender {
	[plotView clear];
}

- (IBAction)openOptionWin:(id)sender {
	[OptionWindow openSheet];
}



- (void)updateEyeVariableNames {
	[plotView setTimeOfTail:[OptionWindow timeOfTail]];
	[plotView setUpdateRate:[OptionWindow updateRate]];
	[EYE_H release];
	[EYE_V release];
	[EYE_STATE release];
	EYE_H = [[NSString alloc] initWithString:[OptionWindow h]];
	EYE_V = [[NSString alloc] initWithString:[OptionWindow v]];
	EYE_STATE = [[NSString alloc] initWithString:[OptionWindow eyeState]];
	[self cacheCodes];
}



/*******************************************************************
 *                Callback Methods
 *******************************************************************/
- (void)codecReceived:(MWCocoaEvent *)event {
	if(!eyeWindowStarted) {
		[NSThread detachNewThreadSelector:@selector(aggregateEvents:)
								 toTarget:plotView
							   withObject:nil];			
		eyeWindowStarted = YES;
	}
	[self cacheCodes];	
}

- (void)serviceHEvent:(MWCocoaEvent *)event {
	[plotView addEyeHEvent:event];		
}

- (void)serviceVEvent:(MWCocoaEvent *)event {
	[plotView addEyeVEvent:event];		
}

- (void)serviceStmEvent:(MWCocoaEvent *)event {
	mw::Data *stm_announce = [event data];
	
	if (stm_announce->isUndefined()) {					//stimulus announce should NEVER be NULL
		mwarning(M_NETWORK_MESSAGE_DOMAIN, "Received NULL for stimulus announce event.");
	} else {
		if(stm_announce->isDictionary()) {
			[plotView acceptStmAnnounce:stm_announce Time:[event time]];
		}
	}
}

- (void)serviceCalEvent:(MWCocoaEvent *)event {
	mw::Data *cal_announce = [event data];
	
	if (cal_announce->isUndefined()) {					//calibrator announce should NEVER be NULL
		mwarning(M_NETWORK_MESSAGE_DOMAIN, "Received NULL for calibrator announce event.");
	} else {
		if(cal_announce->isDictionary()) {
			[plotView acceptCalAnnounce:cal_announce];
		}
	}
}

- (void)serviceStateEvent:(MWCocoaEvent *)event {
	[plotView addEyeStateEvent:event];
}

/*******************************************************************
*                           Private Methods
*******************************************************************/
- (void)cacheCodes {
	int hCodecCode = -1;
	int vCodecCode = -1;
	int stmAnnounceCodecCode = -1;
	int calAnnounceCodecCode = -1;
	int eyeStateCodecCode = -1;
	
	if(delegate != nil) {
		hCodecCode = [[delegate codeForTag:EYE_H] intValue];
		vCodecCode = [[delegate codeForTag:EYE_V] intValue];
		stmAnnounceCodecCode = [[delegate codeForTag:[NSString stringWithCString:ANNOUNCE_STIMULUS_TAGNAME
																		encoding:NSASCIIStringEncoding]] intValue];
		calAnnounceCodecCode = [[delegate codeForTag:[NSString stringWithCString:ANNOUNCE_CALIBRATOR_TAGNAME
																		encoding:NSASCIIStringEncoding]] intValue];
		eyeStateCodecCode = [[delegate codeForTag:EYE_STATE] intValue];
		
		[delegate unregisterCallbacksWithKey:EYE_WINDOW_CALLBACK_KEY];
		[delegate registerEventCallbackWithReceiver:self 
										andSelector:@selector(codecReceived:)
											 andKey:EYE_WINDOW_CALLBACK_KEY
									forVariableCode:[NSNumber numberWithInt:RESERVED_CODEC_CODE]];
		
		[delegate registerEventCallbackWithReceiver:self 
										andSelector:@selector(serviceHEvent:)
											 andKey:EYE_WINDOW_CALLBACK_KEY
									forVariableCode:[NSNumber numberWithInt:hCodecCode]];
		
		[delegate registerEventCallbackWithReceiver:self 
										andSelector:@selector(serviceVEvent:)
											 andKey:EYE_WINDOW_CALLBACK_KEY
									forVariableCode:[NSNumber numberWithInt:vCodecCode]];
		
		[delegate registerEventCallbackWithReceiver:self 
										andSelector:@selector(serviceStmEvent:)
											 andKey:EYE_WINDOW_CALLBACK_KEY
									forVariableCode:[NSNumber numberWithInt:stmAnnounceCodecCode]];
		
		[delegate registerEventCallbackWithReceiver:self 
										andSelector:@selector(serviceCalEvent:)
											 andKey:EYE_WINDOW_CALLBACK_KEY
									forVariableCode:[NSNumber numberWithInt:calAnnounceCodecCode]];
		
		[delegate registerEventCallbackWithReceiver:self 
										andSelector:@selector(serviceStateEvent:)
											 andKey:EYE_WINDOW_CALLBACK_KEY
									forVariableCode:[NSNumber numberWithInt:eyeStateCodecCode]];
	}
	if(hCodecCode == -1 || 
	   vCodecCode == -1 || 
	   calAnnounceCodecCode == -1 || 
	   eyeStateCodecCode == -1 || 
	   stmAnnounceCodecCode == -1) {
		NSString *warningMessage = @"Eye window can't find the following variables: ";
		if(hCodecCode == -1) {
			warningMessage = [warningMessage stringByAppendingString:EYE_H];
			warningMessage = [warningMessage stringByAppendingString:@", "];
		}
		if(vCodecCode == -1) {
			warningMessage = [warningMessage stringByAppendingString:EYE_V];
			warningMessage = [warningMessage stringByAppendingString:@", "];
		}
		if(stmAnnounceCodecCode == -1) {
			warningMessage = [warningMessage stringByAppendingString:[NSString stringWithCString:ANNOUNCE_STIMULUS_TAGNAME
																						encoding:NSASCIIStringEncoding]];
			warningMessage = [warningMessage stringByAppendingString:@", "];
		}
		if(calAnnounceCodecCode == -1) {
			warningMessage = [warningMessage stringByAppendingString:[NSString stringWithCString:ANNOUNCE_CALIBRATOR_TAGNAME
																						encoding:NSASCIIStringEncoding]];
			warningMessage = [warningMessage stringByAppendingString:@", "];
		}
		if(eyeStateCodecCode == -1) {
			warningMessage = [warningMessage stringByAppendingString:EYE_STATE];
			warningMessage = [warningMessage stringByAppendingString:@", "];
		}
		
		warningMessage = [warningMessage substringToIndex:([warningMessage length] - 2)];
		mwarning(M_NETWORK_MESSAGE_DOMAIN, [warningMessage cStringUsingEncoding:NSASCIIStringEncoding]);		
		
	}
}	


@end
