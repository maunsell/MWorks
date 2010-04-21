//
//  MWStimulusPlotElement.h
//  MonkeyWorksEyeWindow
//
//	This object is an extension of MWPlotViewElement. Basically, MWStimulusPlotElement 
//  is an object that contains all the essential information about a stimulus for 
//	display purposes. It also contains a stroke method, which draws the stimulus on the 
//	eye window when called. When operating, any stimulus announce from the data stream 
//	will trigger an MWStimulusPlotElement to be created or modified. All the MWStimulusPlotElement 
//	are stored in a NSMutableArray which is instantiated in MWPlotView object.
//
//  Created by Nuo Li on 8/9/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "MWPlotViewElement.h"


@interface MWStimulusPlotElement : NSObject <MWPlotViewElement>{
	
	BOOL stm_isOn;
	NSString *stm_name;
	NSString* stm_type;
	NSSize size;
	NSPoint center;

	
}


- (id)initStimElement: (NSString *)type Name:(NSString* )name;
- (id)initStimElement: (NSString *)type 
				 Name:(NSString *)name 
				  AtX:(float)pos_x 
				  AtY:(float)pos_y 
			   WidthX:(float)size_x 
			   WidthY:(float)size_y;

- (NSString *)getType;
- (NSString *)getName;
- (BOOL)onOff;

- (void)setOnOff: (BOOL)on;
- (void)setPositionX: (float)pos_x;
- (void)setPositionY: (float)pos_y;
- (void)setSizeX: (float)size_x;
- (void)setSizeY: (float)size_y;

@end
