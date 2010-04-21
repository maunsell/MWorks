//
//  MWEyeSamplePlotElement.m
//  MonkeyWorksEyeWindow
//
//  Created by David Cox on 2/3/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "MWEyeSamplePlotElement.h"


@implementation MWEyeSamplePlotElement
- (id)initWithTime:(mw::MonkeyWorksTime)_time 
		  position:(NSPoint)_position 
	   isSaccading:(int)_is_saccading {
	
    self = [super init];
    if(self) {
		position = _position;
        is_saccading = _is_saccading;
		time = _time;
    }
    return self;
}

@synthesize position=position;
@synthesize time=time;
@synthesize saccading=is_saccading;

@end
