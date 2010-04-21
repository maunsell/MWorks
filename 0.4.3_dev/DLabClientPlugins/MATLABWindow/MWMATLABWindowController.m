#import "MonkeyWorksCocoa/MWCocoaEvent.h"
#import "MonkeyWorksCocoa/MWClientServerBase.h"
#import "MonkeyWorksCore/GenericData.h"
#import "MonkeyWorksCore/VariableProperties.h"
#import "MWMATLABWindowController.h"
#import "MWVarEntry.h"


@interface MWMATLABWindowController(Private)
- (void)codecArrived:(MWCocoaEvent *)codec;
- (void)updateClientCallbacks:(NSArray *)names_of_variables_to_register;
- (void)MATLABExecutionLoop:(id)obj;
- (void)serviceEvent:(MWCocoaEvent *)event;
- (void)reset;
- (void)updateCurrentSelectedVariables:(id)arg;
@end

#define MATLAB_WINDOW_CALLBACK_KEY @"MonkeyWorksMATLABWindow callback key"

#define MW_PATH @"/Library/MonkeyWorks/Matlab/"

#define MATLAB_EXECUTABLE_PATH @"MATLAB client window - MATLAB executable path"
#define MATLAB_M_FILE @"MATLAB client window - MATLAB .m file"
#define SYNC_EVENT_NAME @"MATLAB client window - sync event name"
#define SELECTED_VAR_NAMES @"MATLAB client window - selected variables"
#define COLLECTING_DATA @"MATLAB client window - running"
#define PROCESSING_DATA @"MATLAB client window - processing"
#define MW_SCROLL_TO_BOTTOM @"MATLAB client window - scroll to bottom on output"

@implementation MWMATLABWindowController


-  (id) init {
	self = [super init];
	if (self != nil) {
		matlabLock = [[NSLock alloc] init];
		eventList = [[NSMutableArray alloc] init];
		executionList = [[NSMutableArray alloc] init];
		default_selected_variables = [[NSArray alloc] init];
		collectingEvents = NO;
		savedCodec = new Data();
	}
	return self;
}


- (void)finalize {
	delete savedCodec;
	[executionList release];
	[eventList release];
	[matlabLock release];
	[default_selected_variables release];
	
	[super finalize];
}

- (void)dealloc {
	delete savedCodec;
	[executionList release];
	[eventList release];
	[matlabLock release];
	[default_selected_variables release];
	
	[super dealloc];
}

- (NSString *)matlabFileName {return [matlab_file_name lastPathComponent]; }
- (void)setMatlabFileName:(NSString *)new_matlab_file {
	[matlab_file_name release];
	matlab_file_name = [new_matlab_file copy];	
	[mi setMatlabFile:matlab_file_name];
	
}

- (void) setLogTextContent:(NSString *)new_content {
	[logTextContent autorelease];
	logTextContent = [new_content copy];
	
	// set it in the UI
	[[[logTextView textStorage] mutableString] setString:logTextContent];
	
};	
@synthesize logTextContent;


@synthesize delegate;

- (void)setDelegate:(id)new_delegate {
	if(![new_delegate respondsToSelector:@selector(unregisterCallbacksWithKey:)] ||
	   ![new_delegate respondsToSelector:@selector(variableNames)] ||
	   ![new_delegate respondsToSelector:@selector(registerEventCallbackWithReceiver:
												   andSelector:
												   andKey:)] ||
	   ![new_delegate respondsToSelector:@selector(registerEventCallbackWithReceiver:
												   andSelector:
												   andKey:
												   forVariableCode:)] ||
	   ![new_delegate respondsToSelector:@selector(codeForTag:)] ||
	   ![new_delegate respondsToSelector:@selector(updateVariableWithTag:
												   withData:)]) {
		[NSException raise:NSInternalInconsistencyException
					format:@"Delegate doesn't respond to required methods for MWMATLABWindowController"];		
	}
	
	delegate = new_delegate;
	[delegate registerEventCallbackWithReceiver:self 
									andSelector:@selector(codecArrived:)
										 andKey:MATLAB_WINDOW_CALLBACK_KEY
								forVariableCode:[NSNumber numberWithInt:RESERVED_CODEC_CODE]];	
}

- (void)awakeFromNib {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	[self setProcessing:[defaults boolForKey:PROCESSING_DATA]];
	[self setRunning:[defaults boolForKey:COLLECTING_DATA]];
	
	NSString *matlabFile = [defaults objectForKey:MATLAB_M_FILE];
	if(matlabFile != nil) {
		[self setMatlabFileName:matlabFile];
	}
	
	
	if([defaults stringForKey:SYNC_EVENT_NAME]) {
		self.syncEventName = [defaults stringForKey:SYNC_EVENT_NAME];
	} else {
		self.syncEventName = @"";
	}
	
	[NSThread detachNewThreadSelector:@selector(MATLABExecutionLoop:) 
							 toTarget:self
						   withObject:nil];

	[self setLogTextContent:@"Matlab output:\n"];	
	if ([defaults boolForKey:MW_SCROLL_TO_BOTTOM]) {
		scrollToBottom = [defaults boolForKey:MW_SCROLL_TO_BOTTOM];
	} else {
		scrollToBottom = TRUE;
	}
	if (scrollToBottom == TRUE) {
		[scrollToBottomButton setState:NSOnState];
	} else {		
		[scrollToBottomButton setState:NSOffState];
	}
		
}

// *******************************************************************
// *                 Interface builder methods
// *******************************************************************
- (IBAction)chooseMATLABFile:(id)sender {
	[matlabLock lock];
	
	NSOpenPanel * op = [NSOpenPanel openPanel];
	[op setTitle:@"Select MATLAB .m file to open"];
	[op setCanChooseDirectories:NO];
	[op setCanChooseFiles:YES];
	[op setAllowsMultipleSelection:NO];
	
	int bp = [op runModalForTypes:[NSArray arrayWithObject:@"m"]];
	
	if(bp == NSOKButton) {
		
		NSArray * fn = [op filenames];
		NSEnumerator * fileEnum = [fn objectEnumerator];
		NSString * filename;
		NSString *matlabFile;
		
		while((filename = [fileEnum nextObject])) {
			matlabFile = filename;
		}
		
		NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
		[defaults setObject:matlabFile forKey:MATLAB_M_FILE];
		
		[self setMatlabFileName:matlabFile];
	}		
	
	[matlabLock unlock];
	[self reset];
}

- (IBAction)primeMATLABEngine:(id)sender {
	[matlabLock lock];
	[mi startMATLABEngine];
	[matlabLock unlock];
}

- (IBAction)resetAction:(id)sender {
	[self reset];
}


/**********************************
 * Accessors
 **********************************/
- (NSString *)syncEventName { return sync_event_name; }
- (void)setSyncEventName:(NSString *)new_sync_event_name {
	@synchronized(vl) {
		[sync_event_name release];
		sync_event_name = [new_sync_event_name copy];
		[vl setSyncEventName:sync_event_name];
		[[NSUserDefaults standardUserDefaults] setObject:sync_event_name forKey:SYNC_EVENT_NAME];
		
		[self updateVariableFilter];
		[vl reloadData];
	}
	
	[self reset];
}

- (int)processing { return processing; }
- (void)setProcessing:(int)new_processing {
	[[NSUserDefaults standardUserDefaults] setBool:new_processing forKey:PROCESSING_DATA];
	processing = new_processing;
}

- (int)running { return running; }
- (void)setRunning:(int)new_running {
	[[NSUserDefaults standardUserDefaults] setBool:new_running forKey:COLLECTING_DATA];
	running = new_running;
}

@synthesize numberToProcessString = number_to_process_string;




/*******************************************************************
 *                 MWDataEventListenerProtocol Methods
 *******************************************************************/
- (void)serviceEvent:(MWCocoaEvent *)event {
	[matlabLock lock];
	if(running == NSOnState) {
		if([event code] == [[delegate codeForTag:sync_event_name] intValue]) {
			Data *syncData = [event data];
			if(syncData->getInteger() > 0 && !collectingEvents) {
				collectingEvents = YES;
				[eventList removeAllObjects];
			} else if(syncData->getInteger() == 0) {				
				if(collectingEvents) {
					collectingEvents = NO;
					[eventList addObject:event];
					[executionList addObject:[NSArray arrayWithArray:eventList]];
					[eventList removeAllObjects];
				}
			}
		}
		
		if(collectingEvents) {
			[eventList addObject:event];
		}
	}
	[matlabLock unlock];
}

// *******************************************************************
// *                MWWindowController Methods
// *******************************************************************
- (NSString *)monkeyWorksFrameAutosaveName {
    return @"MonkeyWorksMATLABWindow";
}

#define MATLAB_DEBUG_OUTPUT_MAX_LENGTH 10000

- (IBAction)changeScrollToBottom:(id)sender {
	if ([sender state] == NSOnState) {
		[self doScrollToBottom];
		scrollToBottom = TRUE; 
	} else {
		scrollToBottom = FALSE;
	}
	[[NSUserDefaults standardUserDefaults] setBool:scrollToBottom forKey:MW_SCROLL_TO_BOTTOM];
	
	//if ([sender state] == NSOffState) {
	// do nothing
}


// *******************************************************************
// *                           Private Methods
// *******************************************************************



- (void) doScrollToBottomMain { 
	
	[logTextView scrollRangeToVisible:NSMakeRange([[logTextView textStorage] length], 0) ];
	
}


- (void) appendLogTextMain:(NSString *)logText {
	//NSString *log = [logTextContent copy];
	NSMutableString* tStr = [[self->logTextView textStorage] mutableString];
	[tStr appendString:logText];
	if ([tStr characterAtIndex:[tStr length]-1] != '\n') {
		// append newline
		[tStr appendString:@"\n"];
	}
			  
			  
	// Trim the beginning of the string if it is too long
	if ([tStr length] > MATLAB_DEBUG_OUTPUT_MAX_LENGTH) {
		NSRange deleteRange = NSMakeRange(0, ([tStr length] - MATLAB_DEBUG_OUTPUT_MAX_LENGTH));
			[tStr deleteCharactersInRange:deleteRange];
		}
			  
		[self->logTextView display];
		if (self->scrollToBottom) {
			[self doScrollToBottom];
		}
}			  
	
- (void)codecArrived:(MWCocoaEvent *)event {
	[matlabLock lock];
	
	// need to force it by capturing the pointer so it makes it a dictionary
	delete savedCodec;
	Data *new_codec = [event data];
	savedCodec = new Data(*new_codec);
	
	NSArray *current_selected_variables = [NSArray array];
	
	[self performSelectorOnMainThread:@selector(updateDefaultSelectedVariables:)
						   withObject:nil 
						waitUntilDone:YES];
	
	@synchronized(vl) {
		
		[vl clear];
		
		// determine max code number of codec
		int max_key = -1;
		for(int i=0; i< new_codec->getNElements(); ++i) {
			max_key = MAX(new_codec->getScarabDatum()->data.dict->keys[i]->data.integer, max_key);
		}
		
		
		for(int i=0;i<=max_key;++i) {
			Data codec_entry(new_codec->getElement(Data(M_INTEGER, i)));
			
			if(!codec_entry.isUndefined()) {
				NSString *variableName = [NSString stringWithCString:codec_entry.getElement("tagname").getString() 
															encoding:NSASCIIStringEncoding];
				MWVarEntry *var = [[MWVarEntry alloc] initWithName:variableName];
				
				// select previous variables
				if(default_selected_variables != nil) {
					NSEnumerator *enumerator = [default_selected_variables objectEnumerator];
					NSString *selectedVariableName;
					
					while(selectedVariableName = [enumerator nextObject]) {
						if([selectedVariableName isEqualToString:variableName]) {
							[var setSelected:YES];
						}
					}
				}
				[vl addVariable:[var autorelease]];		
			}
		}
		
		[vl setSyncEventName:[self syncEventName]];
		current_selected_variables = [vl currentSelectedVariables];
		[vl reloadData];
	}
	[matlabLock unlock];
	
	[self updateClientCallbacks:current_selected_variables];
	
	[self reset];
}

// must be run on main thread
- (void)updateDefaultSelectedVariables:(id)arg {
	[default_selected_variables release];
	default_selected_variables = [[NSArray alloc] initWithArray:[[NSUserDefaults standardUserDefaults] objectForKey:SELECTED_VAR_NAMES]];
}




- (void)updateClientCallbacks:(NSArray *)codes_to_register {
	[delegate unregisterCallbacksWithKey:MATLAB_WINDOW_CALLBACK_KEY];
	
	// register for codecs
	[delegate registerEventCallbackWithReceiver:self 
									andSelector:@selector(codecArrived:)
										 andKey:MATLAB_WINDOW_CALLBACK_KEY
								forVariableCode:[NSNumber numberWithInt:RESERVED_CODEC_CODE]];		
	
	NSString *name;	
	NSEnumerator *enumerator = [codes_to_register objectEnumerator];
	
	while(name = [enumerator nextObject]) {
		NSNumber *event_code = [delegate codeForTag:name];
		[delegate registerEventCallbackWithReceiver:self 
										andSelector:@selector(serviceEvent:)
											 andKey:MATLAB_WINDOW_CALLBACK_KEY
									forVariableCode:event_code];	
	}
}

- (void)MATLABExecutionLoop:(id)obj {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	do { 
		NSAutoreleasePool *pool2 = [[NSAutoreleasePool alloc] init];
		[matlabLock lock];
		self.numberToProcessString = [NSString stringWithFormat:@"Process %d events", [executionList count]];
		[matlabLock unlock];
		
		
		if(processing == NSOnState) {
			NSArray *eventsToExecute = nil;
			[matlabLock lock];
			Data codec(*savedCodec);
			if([executionList count] > 0 && codec.isDictionary()) {
				eventsToExecute = [[executionList objectAtIndex:0] retain];
				[executionList removeObjectAtIndex:0];
			}
			[matlabLock unlock];
			
			if(eventsToExecute != nil && codec.isDictionary()) {
				mxArray *dataStruct = [mi createDataStruct:eventsToExecute
												 withCodec:&codec];
				[mi runMatlabFile:dataStruct];
				mxDestroyArray(dataStruct);
				[eventsToExecute release];
			}
		}
		usleep(5000); // sleep to surrender the processor
		[pool2 release];
	} while (1);
	[pool release];
}

// this must be locked before being called
- (void)reset {
	[matlabLock lock];
	collectingEvents = NO;
	//	[delegate set:[[delegate codeForTag:[self syncEventName]] intValue] to:&sync_zero];
	//	[delegate setValue:[NSNumber numberWithInt:0] forKey:[@"variables." stringByAppendingString:self.syncEventName]];
	
	
	NSArray *variable_names = [delegate variableNames];
	NSEnumerator *var_name_enumerator = [variable_names objectEnumerator];
	NSString *var_name = nil;
	
	while(var_name = [var_name_enumerator nextObject]) {
		if([var_name isEqualToString:self.syncEventName]) {
			Data data(0L);
			[delegate updateVariableWithTag:self.syncEventName 
								   withData:&data];			
		}
	}
	
	[mi resetRetval];
	[executionList removeAllObjects];	
	[matlabLock unlock];
}


///////////////////////////////////////////////////////////////////////
// Delegate functions
///////////////////////////////////////////////////////////////////////
- (void)startX11 {
	system("open -a /Applications/Utilities/X11.app");
}


// this only gets called from the main thread
- (void)updateVariableFilter {
	[matlabLock lock];
	
	NSMutableArray *selected_variables = [NSMutableArray array];
	
	@synchronized(vl) {
		NSEnumerator *enumerator = [[vl current_vars] objectEnumerator];
		MWVarEntry *var;
		
		while(var = [enumerator nextObject]) {
			if([[var selected] boolValue]) {
				[selected_variables addObject:[var name]];
			}
		}
	}
	
	if([selected_variables count] > 1) {
		// don't update the defaults until there is some variables to update
		[[NSUserDefaults standardUserDefaults] setObject:selected_variables forKey:SELECTED_VAR_NAMES];		
	}
	
	
	[matlabLock unlock];
	[self updateClientCallbacks:selected_variables];	
}

- (void) appendLogText:(NSString *)logText {
	
	[self performSelectorOnMainThread: @selector(appendLogTextMain:)
						   withObject: logText
						waitUntilDone: YES];	
}

-(void) doScrollToBottom {
	
	[self performSelectorOnMainThread:@selector(doScrollToBottomMain)
						   withObject:nil 
						waitUntilDone:YES];
}

@end
