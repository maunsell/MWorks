//
//  MWStimulusPlotElement.m
//  MonkeyWorksEyeWindow
//
//  Created by Nuo Li on 8/9/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import "MWStimulusPlotElement.h"
#import "MonkeyWorksCore/StandardVariables.h"

@interface MWStimulusPlotElement(PrivateMethods)
- (void)openGLCommandsToDrawBoxInFrame:(NSRect)visible;
- (void)openGLCommandsToDrawCrossInFrame:(NSRect)visible
								withSize:(NSSize)draw_percentage;
@end

@implementation MWStimulusPlotElement

- (id)initStimElement: (NSString *)type Name:(NSString *)name {
	return [self initStimElement:type Name:name AtX:0 AtY:0 WidthX:0 WidthY:0];
}


- (id)initStimElement:(NSString *)type Name:(NSString *)name AtX:(float)pos_x AtY:(float)pos_y WidthX:(float)size_x WidthY:(float)size_y {
	self = [super init];
	if (self) {
		stm_name = [name copy];
		stm_type = [type copy];
		stm_isOn = YES;
		center = NSMakePoint(pos_x, pos_y);
		size = NSMakeSize(size_x, size_y);
	}
	return self;
}

- (void)dealloc {
	[stm_type release];
	[stm_name release];
	[super dealloc];
}

- (NSString *)getName {
	return stm_name;
}
	
	
- (NSString *)getType {
	return stm_type;
}
	
	
- (BOOL)onOff {
	return stm_isOn;
}
	
- (void)setOnOff: (BOOL)on {
	stm_isOn = on;
}

- (void)setPositionX: (float)pos_x {
	center = NSMakePoint(pos_x, center.y);
}


- (void)setPositionY: (float)pos_y {
	center = NSMakePoint(center.x, pos_y);
}


- (void)setSizeX: (float)size_x {
	size = NSMakeSize(size_x, size.height);
}


- (void)setSizeY: (float)size_y {
	size = NSMakeSize(size.width, size_y);
}

- (void)openGLCommandsToDrawBoxInFrame:(NSRect)visible {
	// draw the rectangle
	glBegin(GL_LINE_LOOP);
	{
		const float left_box = center.x-(size.width/2);
		const float right_box = center.x+(size.width/2);
		const float left_box_scaled = 2*((left_box-visible.origin.x)/(visible.size.width))-1;
		const float right_box_scaled = 2*((right_box-visible.origin.x)/(visible.size.width))-1;
		const float up_box = center.y-(size.height/2);
		const float down_box = center.y+(size.height/2);
		const float up_box_scaled = 2*((up_box-visible.origin.y)/(visible.size.height))-1;
		const float down_box_scaled = 2*((down_box-visible.origin.y)/(visible.size.height))-1;
		glVertex2f(left_box_scaled, up_box_scaled);
		glVertex2f(left_box_scaled, down_box_scaled);
		glVertex2f(right_box_scaled, down_box_scaled);
		glVertex2f(right_box_scaled, up_box_scaled);
	}
	glEnd ();	
}

- (void)openGLCommandsToDrawCrossInFrame:(NSRect)visible
								withSize:(NSSize)draw_size {
	// draw the cross
	glBegin(GL_LINES);
	{
		const float left_cross = center.x-draw_size.width/2;
		const float right_cross = center.x+draw_size.width/2;				
		const float left_cross_scaled = 2*((left_cross-visible.origin.x)/(visible.size.width))-1;
		const float right_cross_scaled = 2*((right_cross-visible.origin.x)/(visible.size.width))-1;
		const float horizontal_center_scaled = 2*((center.y-visible.origin.y)/(visible.size.height))-1;
		
		glVertex2f(left_cross_scaled, horizontal_center_scaled);
		glVertex2f(right_cross_scaled, horizontal_center_scaled);				
		
		const float up = center.y+draw_size.height/2;
		const float down_cross = center.y-draw_size.height/2;
		const float up_scaled = 2*((up-visible.origin.y)/(visible.size.height))-1;
		const float down_cross_scaled = 2*((down_cross-visible.origin.y)/(visible.size.height))-1;
		const float vertical_center_scaled = 2*((center.x-visible.origin.x)/(visible.size.width))-1;
		
		glVertex2f (vertical_center_scaled, up_scaled);
		glVertex2f (vertical_center_scaled, down_cross_scaled);
	}
	glEnd();	
}


//========================= Drawing the stimulus ====================================
- (void)stroke:(NSRect)visible {
	if (stm_isOn) {
		if ([stm_type isEqualToString:[NSString stringWithCString:STIM_TYPE_POINT encoding:NSASCIIStringEncoding]]) {
			glColor3f (0,1,0);
			
			[self openGLCommandsToDrawBoxInFrame:visible];
			glLineWidth (1);
			[self openGLCommandsToDrawCrossInFrame:visible 
										  withSize:NSMakeSize(0.3*size.width, 0.3*size.height)];
		} else if ([stm_type isEqualToString:[NSString stringWithCString:STIM_TYPE_POINT encoding:NSASCIIStringEncoding]] ||
				   [stm_type isEqualToString:[NSString stringWithCString:STIM_TYPE_IMAGE encoding:NSASCIIStringEncoding]]) {
			glLineWidth (1);
			glColor3f (1,0,0);
			
			[self openGLCommandsToDrawBoxInFrame:visible];
		} else if ([stm_type isEqualToString:@"calibratorSample"]) {
			glLineWidth (1);
			glColor3f (1,0,0);
			
			const float largest_visible_dimension = MAX(visible.size.width, visible.size.height);
			[self openGLCommandsToDrawCrossInFrame:visible 
										  withSize:NSMakeSize(0.03*largest_visible_dimension, 
															  0.03*largest_visible_dimension)];
			
			[NSTimer scheduledTimerWithTimeInterval: 0.1
				target: self
				selector: @selector(eraseCalSample:)
				userInfo: nil
				repeats: NO];	
		}
	}
}
//=====================================================================================

-(void) eraseCalSample:(NSTimer *)theTimer {
	[self setOnOff:NO];
}

@end
