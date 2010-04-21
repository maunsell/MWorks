/* DLabTemplateClientWindowController */

// This is the minimum that is needed for a window.  It's likely that more will be
// if you want the window to do anything.

@interface DLabTemplateClientWindowController : NSWindowController {			
	IBOutlet id delegate;
}


// Accessors
@property (readwrite, assign) id delegate;

@end
