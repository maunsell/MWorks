#import "MATLABInterface.h"
#import "Scarab/scarab.h"
#import "MonkeyWorksCocoa/MWCocoaEvent.h"
#import "MonkeyWorksCore/GenericData.h"
#import "MonkeyWorksCore/Event.h"
#import "MonkeyWorksCore/Utilities.h"
#import "engine.h"
#import "matrix.h"
#import "mat.h"
#import "monkeyWorksStreamUtilities.h"
#import "MWMATLABWindowController.h"


#define STREAM @"MonkeyWorks Stream"

#define ml_ADD_MATLAB_PATH @"addpath('/Library/MonkeyWorks/Matlab/')"

#define ml_FILENAME "filename"
#define ml_EVENT_CODEC "event_codec"
#define ml_EVENTS "events"

#define MATLAB_APP_PATH "/usr/local/bin/matlab -maci -display :0.0"  // create your symbolic link
//#define MATLAB_APP_PATH "/Library/MonkeyWorks/Matlab/MATLAB/bin/matlab -maci -display :0.0"

#define ml_RETVAL @"retval"

@interface MATLABInterface (PrivateMethods)
- (Engine *)getMATLABEngine;
- (mxArray *)createTopLevelDataStructure:(NSString *)name;
- (mxArray *)createTopLevelEventStructure:(long)nevents;
- (mxArray *)createCodec:(Data *)payload;
@end

@implementation MATLABInterface

- (id) init {
	self = [super init];
	if (self != nil) {
		retval = 0;
		eventStructsQueue = [[NSMutableArray alloc] init];
		interfaceLock = [[NSLock alloc] init];
	}
	return self;
}

- (void) dealloc {
	if(retval) {
		mxDestroyArray(retval);
	}
	
	[super dealloc];
}

- (id)delegate {
    return delegate;
}

- (void)setDelegate:(id)newDelegate {
    delegate = newDelegate;
}


- (void)setMatlabFile:(NSString *)file {
	[interfaceLock lock];
	[matlabFile release];
	matlabFile = [file copy];
	[interfaceLock unlock];
}

- (NSString *)matlabFile {
	[interfaceLock lock];
	NSString *retstring = [[matlabFile copy] autorelease];
	[interfaceLock unlock];
	return retstring;
}	

- (void)resetRetval {
	[interfaceLock lock];
	if(retval) {
		mxDestroyArray(retval);
		retval = 0;
	}
	[interfaceLock unlock];
}

- (void)logMATLABOutput {
	NSString * tStr;
	
	if (!outputBuffer) {
		NSLog(@"bug: output buffer not initialized");
	} else {
		tStr = [[NSString alloc] initWithCString:outputBuffer];

		[delegate appendLogText:tStr];
		//NSLog(@"Matlab output: \n%@", tStr);
	}
	
	
	
}	

- (mxArray *)createDataStruct:(NSArray *)dataEventList
					withCodec:(Data *)codec {
	
	[interfaceLock lock];
	mxArray *codecStruct = [self createCodec:codec];
	int nevents = [dataEventList count];
	
	mxArray *data_struct = [self createTopLevelDataStructure:STREAM];
	if(codecStruct) {
		mxSetField(data_struct, 0, 
				   ml_EVENT_CODEC, 
				   codecStruct);
	} else {
		// no codec
		merror(M_CLIENT_MESSAGE_DOMAIN, "Illegal codec in MATLAB window");
		[interfaceLock unlock];
		return 0;
	}
	
	mxArray *events = [self createTopLevelEventStructure:nevents];
	
	mxArray *old_events = mxGetField(data_struct, 0, ml_EVENTS);
	if(old_events) {
		mxDestroyArray(old_events);
	}
	mxSetField(data_struct, 0, ml_EVENTS, events);
	
	int nread = 0;
	NSEnumerator *enumerator = [dataEventList objectEnumerator]; 
	MWCocoaEvent *event;
	while( (event = [enumerator nextObject]) ) { 
		Data data(*[event data]);
		
		Event de([event code], 
				  [event time], 
				  data);
		
		ScarabDatum *datum = de.toScarabDatum();
		
		
		
		// All events should be scarab lists
		if(datum->type != SCARAB_LIST){  
			scarab_free_datum(datum);
			break;
		}
		
		// Convert and add to event list
		scarabEventToDataStruct(data_struct, nread, datum);
		
		
		nread++;
		
		scarab_free_datum(datum);
	}
	
	[interfaceLock unlock];
	return data_struct;
}


- (void)runMatlabFile:(mxArray *)data_struct {	
	[interfaceLock lock];
	NSString *matlabFunction = [[matlabFile lastPathComponent] stringByDeletingPathExtension];

	Engine *e = [self getMATLABEngine];
	
	NSString *addpath_command = [NSString stringWithFormat:@"addpath('%@')", [matlabFile stringByDeletingLastPathComponent]];
	engEvalString(e, [addpath_command cStringUsingEncoding:NSASCIIStringEncoding]);
	[self logMATLABOutput];
	
	NSString *cmd;
	if(retval) {
		engPutVariable(e, 
					   [ml_RETVAL cStringUsingEncoding:NSASCIIStringEncoding], 
					   retval);
		//cmd = [ml_RETVAL stringByAppendingString:[@"=" stringByAppendingString:[matlabFunction stringByAppendingString:@"(events, retval);"]]];
		cmd = [NSString stringWithFormat:@"%@=%@(events,retval); ", ml_RETVAL, matlabFunction];
		mxDestroyArray(retval);
	} else {
		cmd = [[NSString alloc] initWithFormat:@"%@=%@(events); ", ml_RETVAL, matlabFunction];
		//cmd = [ml_RETVAL stringByAppendingString:[@"=" stringByAppendingString:[matlabFunction stringByAppendingString:@"(events);"]]];
	}
	engPutVariable(e, ml_EVENTS, data_struct);

	// make cmd return error output by wrapping in try; catch
	engEvalString(e, [@"if ~exist('printErrorStack'), disp('printErrorStack.m not found, cannot display error output');end"
				  cStringUsingEncoding:NSASCIIStringEncoding]);
	[self logMATLABOutput];
	
	NSString *catchCmd;
	catchCmd = [[NSString alloc] initWithFormat:@"try, %@, catch ex, printErrorStack(ex); end", cmd]; 
	//[delegate appendLogText:catchCmd]; // debug
	engEvalString(e, [catchCmd cStringUsingEncoding:NSASCIIStringEncoding]);
	[self logMATLABOutput];
	
	retval = engGetVariable(e, 
							[ml_RETVAL cStringUsingEncoding:NSASCIIStringEncoding]);
	[interfaceLock unlock];
}

- (void)startMATLABEngine {
	if ( [delegate respondsToSelector:@selector(startX11)] ) {
		[delegate startX11];
	}
	[delegate appendLogText:@"** X11 is now running"];
	
	[interfaceLock lock];
	[self getMATLABEngine];
	[interfaceLock unlock];
}

////////////////////////////////////////////////////////////////////////////////
// private methods
////////////////////////////////////////////////////////////////////////////////
- (Engine *)getMATLABEngine {
	
	if (!outputBuffer) {
		outputBuffer = (char *)calloc(32768, sizeof(char));
	}
			
	if(!matlabEngine) {
		matlabEngine = engOpen(MATLAB_APP_PATH);
		if (!matlabEngine) {
			[delegate appendLogText:@"** engOpen failed in starting Matlab\n"];
			[delegate appendLogText:[[NSString alloc] initWithFormat:@"** command used was %s\n", MATLAB_APP_PATH]];			
			// No need to crash here
			//[NSException raise:@"engOpen fail" format:@"Matlab not started successfully by engOpen"];
		} else {
				
			engSetVisible(matlabEngine, 1);
			engOutputBuffer(matlabEngine, outputBuffer, 32768);  /* now we can always read output buffer after eval */

			engEvalString(matlabEngine, 
						  [ml_ADD_MATLAB_PATH cStringUsingEncoding:NSASCIIStringEncoding]);
			NSLog(@"Matlab output (of addpath): \n%@", [[NSString alloc] initWithCString:outputBuffer]);	
		}

	} else {
		//NSLog(@"Matlab engine running already after starting X");
	}
	
	// check to see if engine is still running
	mxArray *dummyArray = mxCreateScalarDouble(0);
	if(engPutVariable(matlabEngine, "MW_DUMMY_VAR", dummyArray)) {
		[delegate appendLogText:@"** MATLAB engine found dead"];
		// if it's not, start it up again
		mxDestroyArray(dummyArray);
		engClose(matlabEngine);
		matlabEngine = 0;
		return [self getMATLABEngine];
	}
	mxDestroyArray(dummyArray);
	
	
	return matlabEngine;
}

- (mxArray *)createTopLevelDataStructure:(NSString *)name {
	// *****************************************************************
	// Create the file struct
	// events field will contain the actual events
	// event_types field will include a code that identifies the
	//			   event types (e.g. message, data, etc.)
	// event_codec field will contain the event name / code dictionary
	// *****************************************************************
	
	mwSize ndims = 1;
	mwSize data_dims = 1;
	const char *data_field_names[] = {ml_FILENAME, ml_EVENT_CODEC, ml_EVENTS};
	int data_nfields = 3; // filename, codec, event
	mxArray *dataStruct = mxCreateStructArray(ndims, &data_dims, data_nfields, 
											   data_field_names);
	
	mxArray *old_filename = mxGetField(dataStruct, 0, ml_FILENAME);
	if(old_filename){
		mxDestroyArray(old_filename);
	}
	mxSetField(dataStruct, 
			   0, 
			   ml_FILENAME, 
			   mxCreateString([name cStringUsingEncoding:NSASCIIStringEncoding]));	
	
	return dataStruct;
}

- (mxArray *)createTopLevelEventStructure:(long)nevents {
	// *****************************************************************
	// Allocate storage for the number of events we are about to read
	// *****************************************************************
	
	const char *event_field_names[] = {"event_code", "time_us", "data"};
	int event_nfields = 3;
	mwSize event_dims = nevents;
	mxArray *events = mxCreateStructArray(1, &event_dims, 
										  event_nfields, event_field_names);
		
	return events;
}

- (mxArray *)createCodec:(Data *)codec {
	ScarabDatum *payload = codec->getScarabDatum();
	
	int n_codec_entries = scarab_dict_number_of_elements(payload);
	
	ScarabDatum **keys = scarab_dict_keys(payload);
	ScarabDatum **values = scarab_dict_values(payload);	
	const char *codec_field_names[] = {"code", "tagname",
		"logging",  "defaultvalue", "shortname", 
		"longname","editable", "nvals", 
		"domain", "viewable", "persistant"}; // more to come later?
	int n_codec_fields = 11;
	mwSize ndims = 1;
	mwSize codec_size = n_codec_entries;
	mxArray *codec_struct = mxCreateStructArray(ndims, 
												&codec_size,
												n_codec_fields, 
												codec_field_names);

	
	for(int c = 0; c < n_codec_entries; c++){
		int code = keys[c]->data.integer;
		ScarabDatum *currentEntry = values[c];
		
		
		ScarabDatum **property_keys = scarab_dict_keys(currentEntry);
		ScarabDatum **property_values = 
			scarab_dict_values(currentEntry);
		
		mxSetField(codec_struct, c, "code", 
				   mxCreateDoubleScalar((double)code));
		
		for(int d = 0; d < currentEntry->data.dict->size; d++){
			const char *thiskey = scarab_extract_string(property_keys[d]);
			
			if(property_values[d] == NULL){
				
			} else if(property_values[d]->type == SCARAB_INTEGER){
				mxSetField(codec_struct, c, thiskey, 
						   mxCreateDoubleScalar(property_values[d]->data.integer));
			} else if(property_values[d]->type == SCARAB_OPAQUE){
				mxSetField(codec_struct, c, thiskey, 
						   mxCreateString(scarab_extract_string(property_values[d])));
			}			
		}
	}
	
	return codec_struct;
}

@end
