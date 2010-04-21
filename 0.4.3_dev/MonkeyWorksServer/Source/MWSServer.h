/*!
* @header MWSServer
 *
 * @discussion Provides access to a shared instance of the MWSSever object
 * which contains controller objects for important parts of the system.
 *
 * bkennedy on 07/05/07 - Created.
 */

#import <Cocoa/Cocoa.h>
#import "MWSToolbarControl.h"
#import "MWSMenuControl.h"
#import "MWSNetworkPreferencesController.h"
#import "MonkeyWorksCocoa/MWConsoleController.h"
#import "MonkeyWorksCocoa/MWClientServerBase.h"
#import "MonkeyWorksCore/Server.h"

@interface MWSServer : MWClientServerBase {
//@interface MWSServer : NSObject {
	boost::shared_ptr<Server> core;
	MWConsoleController *cc;
	
	IBOutlet MWSToolbarControl *tc;
	IBOutlet MWSMenuControl *mc;
	IBOutlet MWSNetworkPreferencesController *nc;
	IBOutlet NSMatrix *size_selection;
	
	float width_mm;
	float height_mm;
	float distance_mm;
	unsigned int display_number;
	BOOL mirror_window_enabled;
	float refresh_rate_hz;
	NSString *server_name;
	
	NSString *listeningAddress;
}

@property(assign) BOOL mirrorWindowEnabled;
@property(copy) NSString *serverName;
@property(assign) float width;
@property(assign) float height;
@property(assign) float distance;
@property(assign) float refreshRate;
@property(assign) unsigned int displayNumber;


- (IBAction)openExperiment:(id)sender;
- (IBAction)closeExperiment:(id)sender;
- (IBAction)saveVariables:(id)sender;
- (IBAction)loadVariables:(id)sender;

- (IBAction)openDataFile:(id)sender;
- (IBAction)closeDataFile:(id)sender;
- (IBAction)stopExperiment:(id)sender;
- (IBAction)startExperiment:(id)sender;

- (IBAction)changeSize:(id)sender;

@end


// delegate methods
@interface NSObject (MWSDelegateMethods) 
- (void)startServer;
- (void)stopServer;
- (void)startAccepting;
- (void)stopAccepting;
- (NSNumber *)serverAccepting;
- (NSNumber *)serverStarted;
- (NSNumber *)experimentRunning;
- (NSNumber *)experimentLoaded;
- (void)toggleConsole:(id)arg;
- (void)updateGUI:(id)arg;
- (void)registerEventCallbackWithRecevier:(id)receiver 
							  andSelector:(SEL)selector
								   andKey:(NSString *)key;
- (void)registerEventCallbackWithRecevier:(id)receiver 
							  andSelector:(SEL)selector
								   andKey:(NSString *)key
						  forVariableCode:(NSNumber *)code;
- (void)openNetworkPreferences:(id)sender;
- (NSString *)currentNetworkAddress:(id)sender;
- (NSString *)defaultNetworkAddress:(id)sender;
- (void)setListeningAddress:(NSString *)address;
- (NSArray *)variableNames;
@end

