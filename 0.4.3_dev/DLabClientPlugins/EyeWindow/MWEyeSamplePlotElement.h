//
//  MWEyeSamplePlotElement.h
//  MonkeyWorksEyeWindow
//
//  Created by David Cox on 2/3/06.
//  Copyright 2006 MIT. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MonkeyWorksCore/MonkeyWorksTypes.h"


@interface MWEyeSamplePlotElement : NSObject {
	
	NSPoint position;	
	int is_saccading;	// during a saccade or not?
	mw::MonkeyWorksTime	time;
	
}

- (id)initWithTime:(mw::MonkeyWorksTime)_time 
		  position:(NSPoint)position 
	   isSaccading:(int)_is_saccading;

@property(readonly) NSPoint position;
@property(readonly) mw::MonkeyWorksTime time;
@property(readonly) int saccading;

@end
