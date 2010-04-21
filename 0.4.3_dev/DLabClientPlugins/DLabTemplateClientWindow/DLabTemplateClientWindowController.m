#import "MonkeyWorksCore/StandardVariables.h"
#import "MonkeyWorksCocoa/MWCocoaEvent.h"
#import "DLabTemplateClientWindowController.h"

#define WINDOW_CALLBACK_KEY @"DLabTemplateClientWindowController callback key"

// these are the methods that are used to 
@interface DLabTemplateClientWindowController(PrivateMethods)
- (void)newCodecReceived:(MWCocoaEvent *)codec;
@end

@implementation DLabTemplateClientWindowController

- (void)awakeFromNib {
	// put initialization code here
}

- (void)dealloc {
	// put tear down code here
	//
	[super dealloc];
}

// this creates the acessor methods for "delegate"  It automatically creates
// [self delegate] and [self setDelegate:new_delegate]
@synthesize delegate=delegate;

// we're going to override setDelegate so we can have checks to make sure that all of 
// the required methods exist.  The delegate for client windows is MWClientInstance in
// the client application.
- (void)setDelegate:(id)new_delegate {
	
	// check to make sure the delegate has all of the required methods
	if(![new_delegate respondsToSelector:@selector(registerEventCallbackWithReceiver:
												   andSelector:
												   andKey:)] ||
	   ![new_delegate respondsToSelector:@selector(registerEventCallbackWithReceiver:
												   andSelector:
												   andKey:
												   forVariableCode:)] ||
	   ![new_delegate respondsToSelector:@selector(codeForTag:)] ||
	   ![new_delegate respondsToSelector:@selector(unregisterCallbacksWithKey:)] ||
	   ![new_delegate respondsToSelector:@selector(setValue: forKey:)]) {
		[NSException raise:NSInternalInconsistencyException
					format:@"Delegate doesn't respond to required methods for DLabTemplateClientWindowController"];		
	}
	
	
	delegate = new_delegate;
	// at first, just register a listener for the codec.  This call sets up a callback.  
	// newCodecReceived: will get called at every event received by the client.  We use the
	// key TEMPLATE_WINDOW_CALLBACK_KEY so we can unregister it later.
	[delegate registerEventCallbackWithReceiver:self 
									andSelector:@selector(newCodecReceived:)
										 andKey:WINDOW_CALLBACK_KEY
								forVariableCode:[NSNumber numberWithInt:RESERVED_CODEC_CODE]];
	
}


/*******************************************************************
 *                           Private Methods
 *******************************************************************/

// if a new codec is received, it's a good idea to reregister the callbacks in case a codec code changed.
// initially all events are registered, but by registering them individually, we can reduce the load on the
// by ignoring events that aren't used.
- (void)newCodecReceived:(MWCocoaEvent *)event {
	// first unregister all of the existing callbacks
	[delegate unregisterCallbacksWithKey:WINDOW_CALLBACK_KEY];
	
	// register the codec callback
	[delegate registerEventCallbackWithReceiver:self 
									andSelector:@selector(newCodecReceived:)
										 andKey:WINDOW_CALLBACK_KEY
								forVariableCode:[NSNumber numberWithInt:RESERVED_CODEC_CODE]];
}

@end
