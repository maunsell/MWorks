/* MWPlotView 

This object is called upon to update the eye window display. It maintains 
two NSMutableArrays. One called eye_samples (originally created by Dave),
which keeps a finite number of past eye sample data to draw eye traces. 
The other NSMutableArray is stm_samples (added by Nuo), which stores any 
drawable objects detected by the stimulus announce event from the datastream and 
use it to draw stimulus on the eye window.

Created by Dave Cox

Modified by Nuo Li


Copy right 2006 MIT. All rights reserved.

*/

#import <Cocoa/Cocoa.h>
#import "MWEyeSamplePlotElement.h"
#import "MonkeyWorksCocoa/MWCocoaEvent.h"
#import "MonkeyWorksCore/GenericData.h"

@interface MWPlotView : NSOpenGLView
{
	float width;
	float gridStepX;
	float gridStepY;
	bool cartesianGrid;
	
	NSMutableArray *eye_samples;
	NSMutableArray *stm_samples;
	
	NSMutableArray *eyeHEvents;
	NSMutableArray *eyeVEvents;

	mw::MonkeyWorksTime last_state_change_time;
	int current_state;
	
	mw::MonkeyWorksTime timeOfTail;	
	mw::MonkeyWorksTime time_between_updates;
}

- (void)setWidth:(int)width;
- (void)addEyeHEvent:(MWCocoaEvent *)event;
- (void)addEyeVEvent:(MWCocoaEvent *)event;
- (void)addEyeStateEvent:(MWCocoaEvent *)event;
- (void)acceptStmAnnounce:(mw::Data *)stm_announce 
					 Time:(mw::MonkeyWorksTime)event_time;
- (void)setTimeOfTail:(NSTimeInterval)_newTimeOfTail;
- (void)setUpdateRate:(float)updates_per_second;
- (void)acceptCalAnnounce:(mw::Data *)cal_announce;
- (void)clear;

@end
