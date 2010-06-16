/* MWRewardWindowController */
#import "MonkeyWorksCocoa/MWWindowController.h"

@interface MWRewardWindowController : MWWindowController {

	NSString *reward_var_name;
	NSString *duration_ms;
	NSURL *sound_file_name;
	NSSound *sound_obj;
	IBOutlet NSTextField *soundNameField;
	
}

@property (readwrite, copy) NSString *rewardVarName;
@property (readwrite, assign) NSString *duration;
@property (readwrite, copy) NSURL *soundFileName;  

- (IBAction)sendReward:(id)sender;
- (IBAction)chooseSoundFile:(id)sender;

@end