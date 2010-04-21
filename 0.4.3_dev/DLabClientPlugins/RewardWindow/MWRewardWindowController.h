/* MWRewardWindowController */

@interface MWRewardWindowController : NSWindowController {

	NSString *reward_var_name;
	NSString *duration_ms;
	IBOutlet id delegate;
	NSURL *sound_file_name;
	NSSound *sound_obj;
	IBOutlet NSTextField *soundNameField;
	
}

@property (readwrite, assign) id delegate;
@property (readwrite, copy) NSString *rewardVarName;
@property (readwrite, assign) NSString *duration;
@property (readwrite, copy) NSURL *soundFileName;  

- (IBAction)sendReward:(id)sender;
- (IBAction)chooseSoundFile:(id)sender;

@end