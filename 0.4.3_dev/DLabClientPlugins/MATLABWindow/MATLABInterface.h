/* MATLABInterface */

#import <Cocoa/Cocoa.h>
#import "MonkeyWorksCocoa/MWCocoaEvent.h"
#import "engine.h"
#import "matrix.h"

//static NSLock *interfaceLock;

@interface MATLABInterface : NSObject
{
	id delegate;
	Engine *matlabEngine;
	NSString *matlabFile;
	
	NSLock *interfaceLock;
	NSMutableArray *eventStructsQueue;
	
	char *outputBuffer;	

	mxArray *retval;
}

- (id)delegate;
- (void)setDelegate:(id)newDelegate;
- (void)setMatlabPath:(NSString *)path;
- (NSString *)matlabFile;
- (void)runMatlabFile:(mxArray *)event_struct;
- (void)resetRetval;
- (mxArray *)createDataStruct:(NSArray *)dataEventList
					  withCodec:(Data *)codec;
- (void)startMATLABEngine;
- (void)setMatlabFile:(NSString *)file;

- (void)logMATLABOutput;

@end

