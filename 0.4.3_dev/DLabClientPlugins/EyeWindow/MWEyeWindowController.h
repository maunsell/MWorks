/* MWEyeWindowController 

This object reads MWDataEvents from the stream. It watches for certain 
MWDataEvents (e.g. eye sample event, stimulus announce event). 
Once an appropriate event is recieved, it calls on MWPlotView object to update
the eye window display

Created by Dave Cox

Modified by Nuo Li


Copy right 2006 MIT. All rights reserved.

*/

#import "MonkeyWorksCocoa/MWWindowController.h"
#import "MonkeyWorksCore/GenericData.h"
#import "MWEyeWindowOptionController.h"

extern NSString  * MWEyeWindowVariableUpdateNotification;

@class MWPlotView;

@interface MWEyeWindowController : NSWindowController {
    IBOutlet MWPlotView *plotView;
    IBOutlet NSScrollView *scrollView;
	IBOutlet NSSlider *scaleSlider;
    IBOutlet NSTextField *scaleTextField;
	IBOutlet id delegate;

	MWEyeWindowOptionController * OptionWindow;
	
	// Tag names for the eye data and stimulus announce, use this to find codec number
	NSString *EYE_H;
	NSString *EYE_V;
	NSString *EYE_STATE;

	
	bool eyeWindowStarted;
	
}

@property (assign, readwrite) id delegate;
/*!
 * @function acceptWidth:
 * @discussion
 *
 * @param sender
 */
- (IBAction)acceptWidth:(id)sender;
- (IBAction)redrawPlot:(id)sender;
- (IBAction)openOptionWin:(id)sender;

/*!
 * @function clear:
 * @discussion 
 *
 * @param sender
 */
- (IBAction)clear:(id)sender;


@end
