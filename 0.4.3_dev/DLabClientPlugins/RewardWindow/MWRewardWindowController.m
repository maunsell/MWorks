// histed 091226: added sound to reward

#import "MWRewardWindowController.h"
#include <unistd.h>

@implementation MWRewardWindowController

#define MW_REWARD_WINDOW_DURATION @"Reward Window - duration (ms)"
#define MW_REWARD_WINDOW_VAR_NAME @"Reward Window - var name"
#define MW_REWARD_WINDOW_SOUND_FILE_NAME @"Reward Window - sound played on reward"

@synthesize delegate;

- (void)setDelegate:(id)new_delegate {
	if(![new_delegate respondsToSelector:@selector(codeForTag:)] ||
	   ![new_delegate respondsToSelector:@selector(setValue: forKey:)]) {
		[NSException raise:NSInternalInconsistencyException
					format:@"Delegate doesn't respond to required methods for MWRewardWindowController"];		
	}
	
	delegate = new_delegate;
	
	NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];	
	duration_ms = [ud objectForKey:MW_REWARD_WINDOW_DURATION];
	reward_var_name = [[ud stringForKey:MW_REWARD_WINDOW_VAR_NAME] copy];
	if ([ud stringForKey:MW_REWARD_WINDOW_SOUND_FILE_NAME] == nil)
		sound_file_name = nil;
	else
		sound_file_name = [[NSURL alloc] initFileURLWithPath:[[ud stringForKey:MW_REWARD_WINDOW_SOUND_FILE_NAME] copy]]; //MH 091226- URLForKey available in 10.6+ only
	
	//NSLog(@"fname: %s", [sound_file_name path]);
	// load sound file into object in memory
	sound_obj = [[NSSound alloc] initWithContentsOfURL:sound_file_name byReference:NO];
	
}

@synthesize rewardVarName = reward_var_name;
- (void)setRewardVarName:(NSString *)new_reward_var_name {
	[reward_var_name release];
	reward_var_name = [new_reward_var_name copy];
	[[NSUserDefaults standardUserDefaults] setObject:reward_var_name forKey:MW_REWARD_WINDOW_VAR_NAME];
}

@synthesize duration = duration_ms;
- (void)setDuration:(NSString *)new_duration {
	[duration_ms release];
	duration_ms = [new_duration copy];
	[[NSUserDefaults standardUserDefaults] setObject:duration_ms forKey:MW_REWARD_WINDOW_DURATION];
}

//MH - I think this is needed to get new object values into the default system?
@synthesize soundFileName = sound_file_name;
- (void)setSoundFileName:(NSURL *)new_sound_file_name {
	[sound_file_name release];
	sound_file_name = [new_sound_file_name copy];
	// Convert to string for saving in defaults db; Leopard NSUserDefaults has no setURLforKey method
	[[NSUserDefaults standardUserDefaults] setObject:[sound_file_name path] forKey:MW_REWARD_WINDOW_SOUND_FILE_NAME];
	[soundNameField setStringValue:[sound_file_name path]];
}
	
- (IBAction)sendReward:(id)sender {
	int iDurationMs = 0;
	
	if(delegate != nil) {
		// get duration as a string and convert to ms
		iDurationMs = [duration_ms intValue];
		
		if(duration_ms < 0) {
			self.duration = @"0";
		} else {
		
			NSTimer * startT = [NSTimer alloc];

			[delegate setValue:[NSNumber numberWithInt:iDurationMs*1000] 
					forKeyPath:[@"variables." stringByAppendingString:self.rewardVarName]];
		
			if (sound_obj != nil) {
				[sound_obj play];	
			}	
		}
	}
}

- (IBAction)chooseSoundFile:(id)sender {
	NSArray *fileTypes = [NSArray arrayWithObjects:@"wav", @"aiff", nil];
	
	NSOpenPanel* oPanel = [NSOpenPanel openPanel];
	
	[oPanel setCanChooseDirectories:NO];
	[oPanel setCanChooseFiles:YES];
	[oPanel setCanCreateDirectories:NO];
	[oPanel setAllowsMultipleSelection:NO];
	[oPanel setTitle:@"Select a reward sound file"];
	
	// Display the dialog. If the OK button was pressed,
	// process the files.
	if ( [oPanel runModalForDirectory:nil file:nil types:fileTypes] == NSOKButton ){
		sound_file_name = [[oPanel URLs] objectAtIndex:0];
	}
	NSLog(@"read file name %@", sound_file_name);

	//put file name into the text box
	//[soundNameField setStringValue:[sound_file_name path]];
	[self setSoundFileName:sound_file_name];
	
	//load the sound object
	sound_obj = [[NSSound alloc] initWithContentsOfURL:sound_file_name byReference:NO];
	[sound_obj play];
	NSLog(@"Sound file duration: %5.3fs", [sound_obj duration]);
}


@end

