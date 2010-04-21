/**
 * MWSServer.m
 *
 * History:
 * bkennedy on 07/05/07 - Created.
 *
 * Copyright MIT 2007.  All rights reserved.
 */

#import "MWSServer.h"
#import "MonkeyWorksCocoa/MWCocoaEventFunctor.h"
#import "MonkeyWorksCore/StandardVariables.h"
#import "MonkeyWorksCore/PlatformDependentServices.h"
#import "MonkeyWorksCore/LoadingUtilities.h"
#import <sys/types.h>
#import <sys/socket.h>
#import <ifaddrs.h>

#define DEFAULT_HOST_IP @"127.0.0.1"
#define LISTENING_ADDRESS_KEY @"listeningAddressKey"

@interface MWSServer(PrivateMethods)
- (void)processEvent:(MWCocoaEvent *)cocoaEvent;
- (void)updateGeometryParamFile;
- (NSXMLElement *)createDictionaryElementWithKey:(NSString *)key
											type:(NSString *)type
										andValue:(NSString *)value;
@end

@implementation MWSServer

- (id) init {
	self = [super init];
	if (self != nil) {
		core = boost::shared_ptr <Server>(new Server());
		
		NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
		listeningAddress = [defaults objectForKey:LISTENING_ADDRESS_KEY];
		
		//		// TODO: this is crazy slow on some machines/networks
		//		#define ESCHEW_NSHOST	1
		//		#if ESCHEW_NSHOST
		//			// TODO: double check this
		//			struct ifaddrs *addrs;
		//			int i = getifaddrs(&addrs);
		//			NSMutableArray *netAddresses = [[NSMutableArray alloc] init];
		//			while(addrs != NULL){
		//				[netAddresses insertObject:[NSString stringWithCString:addrs->ifa_name] atIndex:0];
		//				addrs = addrs->ifa_next;
		//			}
		//		#else	
		//			NSArray *netAddresses = [[NSHost currentHost] addresses];
		//		#endif
		//		
		//		NSString *regex  = @"[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}";
		//		NSString *predicateFormat = [[@"SELF MATCHES \"" stringByAppendingString:regex] stringByAppendingString:@"\""];
		//		
		//		NSPredicate *addressPredicate =
		//			[NSPredicate predicateWithFormat:predicateFormat];
		//		
		//		NSArray *filteredArray = [netAddresses filteredArrayUsingPredicate:addressPredicate];
		//				
		//		if(listeningAddress == nil || ![filteredArray containsObject:listeningAddress]) { 
		//			listeningAddress = [[NSString alloc] initWithString:DEFAULT_HOST_IP];
		//		}
		
		cc = [[MWConsoleController alloc] init];
	}
	return self;
}



- (void)dealloc {
	[listeningAddress release];
	[cc release];
	[super dealloc];
}

- (void)awakeFromNib {
	try {
		loadSetupVariables();
	} catch (std::exception& e){
		merror(M_PARSER_MESSAGE_DOMAIN, "Unable to load setup variables.  Error was: %s", e.what());
	}	
}


/****************************************************************
 *              NSApplication Delegate Methods
 ***************************************************************/
- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// initialize GUI items
	
	boost::shared_ptr <CocoaEventFunctor> cef = boost::shared_ptr <CocoaEventFunctor>(new CocoaEventFunctor(self,@selector(processEvent:), "MWSServer: #mainScreenInfo"));
	core->registerCallback(cef, [[self codeForTag:[NSString stringWithCString:MAIN_SCREEN_INFO_TAGNAME encoding:NSASCIIStringEncoding]] intValue]);

	cef = boost::shared_ptr <CocoaEventFunctor>(new CocoaEventFunctor(self,@selector(processEvent:), "MWSServer: #serverName"));
	core->registerCallback(cef, [[self codeForTag:[NSString stringWithCString:"#serverName" encoding:NSASCIIStringEncoding]] intValue]);
	
	
	core->setListenLowPort(19989);
    core->setListenHighPort(19999);
	
	string hostname;
	if(listeningAddress == Nil || [listeningAddress isEqualToString:@""]){
		hostname = "127.0.0.1";
		listeningAddress = @"127.0.0.1";
	} else {
		hostname = [listeningAddress cStringUsingEncoding:NSASCIIStringEncoding];
	}
	
	
	core->setHostname(hostname);
	
	[cc setTitle:@"Server Console"];
	[cc setDelegate:self];
	
	core->startServer();
	core->startAccepting();	
	[self updateGUI:nil];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // stop any data files still open.
	
    // close all open network connections.
    
    // close any applications it owns.
	//delete core;
}

@synthesize mirrorWindowEnabled=mirror_window_enabled;
@synthesize displayNumber=display_number;
@synthesize width=width_mm;
@synthesize height=height_mm;
@synthesize distance=distance_mm;
@synthesize refreshRate=refresh_rate_hz;
@synthesize serverName=server_name;

- (void)setMirrorWindowEnabled:(BOOL)new_mirror_window_enabled {
	mirror_window_enabled=new_mirror_window_enabled;                                                                                                                                                                 
	[self updateGeometryParamFile];                                                                                                                                                                                  
}

- (void)setDisplayNumber:(unsigned int)new_display_number {
	display_number = new_display_number;                                                                                                                                                                             
	[self updateGeometryParamFile];                                                                                                                                                                                  
}

- (void)setWidth:(float)new_width {
	width_mm = new_width;                                                                                                                                                                                            
	[self updateGeometryParamFile];                                                                                                                                                                                  
}

- (void)setHeight:(float)new_height {
	height_mm = new_height;                                                                                                                                                                                          
	[self updateGeometryParamFile];                                                                                                                                                                                  
}

- (void)setDistance:(float)new_distance {
	distance_mm = new_distance;                                                                                                                                                                                      
	[self updateGeometryParamFile];                                                                                                                                                                                  
}

- (void)setRefreshRate:(float)new_refresh_rate {
	refresh_rate_hz = new_refresh_rate;                                                                                                                                                                              
	[self updateGeometryParamFile];                                                                                                                                                                                  
}

- (void)setServerName:(NSString *)new_server_name {
	[server_name release];
	server_name = [new_server_name copy];
	[self updateGeometryParamFile];                                                                                                                                                                                  
}


/****************************************************************
 *              IBAction methods
 ***************************************************************/
- (IBAction)toggleConsole:(id)sender {
	if([[cc window] isVisible]) {
		[cc close];
	} else {
		[cc showWindow:nil];	
	}
}

- (IBAction)closeExperiment:(id)sender {
	core->closeExperiment();
}

- (IBAction)openExperiment:(id)sender {
    NSOpenPanel * op = [NSOpenPanel openPanel];
    [op setCanChooseDirectories:NO];
    // it is important that you never allow multiple files to be selected!
    [op setAllowsMultipleSelection:NO];
	
    int bp = [op runModalForTypes:[NSArray arrayWithObjects:@"xml", nil]];
    if(bp == NSOKButton) {
        NSArray * fn = [op filenames];
        NSEnumerator * fileEnum = [fn objectEnumerator];
        NSString * filename;
        while(filename = [fileEnum nextObject]) {
			if(!core->openExperiment([filename cStringUsingEncoding:NSASCIIStringEncoding])) {
                NSLog(@"Could not open experiment %@", filename);
            }
        }
    }
}

- (IBAction)saveVariables:(id)sender {
    NSSavePanel * save = [[NSSavePanel savePanel] retain];
    [save setAllowedFileTypes:[NSArray arrayWithObject:@"xml"]];
    [save setCanCreateDirectories:NO];
    if([save runModalForDirectory:nil file:nil] ==
	   NSFileHandlingPanelOKButton)  {
		core->saveVariables(boost::filesystem::path([[save filename] cStringUsingEncoding:NSASCIIStringEncoding], 
													boost::filesystem::native));
    }
	
	[save release];	
}

- (IBAction)loadVariables:(id)sender {
	NSOpenPanel * op = [[NSOpenPanel openPanel] retain];
    [op setCanChooseDirectories:NO];
    // it is important that you never allow multiple files to be selected!
    [op setAllowsMultipleSelection:NO];
	
    int bp = [op runModalForTypes:[NSArray arrayWithObjects:@"xml", nil]];
    if(bp == NSOKButton) {
        NSArray * fn = [op filenames];
        NSEnumerator * fileEnum = [fn objectEnumerator];
        NSString * filename;
        while(filename = [fileEnum nextObject]) {			
			core->loadVariables(boost::filesystem::path([filename cStringUsingEncoding:NSASCIIStringEncoding], 
														boost::filesystem::native));
        }
    }
	
	[op release];
}

- (IBAction)openDataFile:(id)sender {
    NSSavePanel * save = [[NSSavePanel savePanel] retain];
    [save setAllowedFileTypes:[NSArray arrayWithObject:@"mwk"]];
    [save setCanCreateDirectories:NO];
    if([save runModalForDirectory:nil file:nil] ==
	   NSFileHandlingPanelOKButton)  {
        core->openDataFile([[[save filename] lastPathComponent]
                            cStringUsingEncoding:NSASCIIStringEncoding]);
    }
	
	[save release];
}

- (IBAction)closeDataFile:(id)sender {
	core->closeFile();
}


- (IBAction)startExperiment:(id)delegate {
	if(!core->isExperimentRunning()) {
		core->startExperiment();
	}
	[self updateGUI:nil];
}

- (IBAction)stopExperiment:(id)delegate {
	if(core->isExperimentRunning()) {
		core->stopExperiment();
	}
	[self updateGUI:nil];
}

- (IBAction)changeSize:(id)sender {
	[self updateGeometryParamFile];
}



////////////////////////////////////////////////////////////////////////////////
// Delegate Methods
////////////////////////////////////////////////////////////////////////////////
- (NSNumber *)codeForTag:(NSString *)tag {
	return [NSNumber numberWithInt:core->getCode([tag cStringUsingEncoding:NSASCIIStringEncoding])];
}

- (void)startServer {
	core->startServer();
	[self setListeningAddress:listeningAddress];
	[self updateGUI:nil];
}

- (void)stopServer {
	core->stopServer();
	[self updateGUI:nil];
}

- (void)startAccepting {
	core->startAccepting();
	[self updateGUI:nil];
}

- (void)stopAccepting {
	core->stopAccepting();	
	[self updateGUI:nil];
}

- (NSNumber *)experimentLoaded {
	return [NSNumber numberWithBool:core->isExperimentLoaded()];
}

- (NSNumber *)experimentRunning {
	return [NSNumber numberWithBool:core->isExperimentRunning()];
}

- (NSNumber *)serverAccepting {
	return [NSNumber numberWithBool:core->isAccepting()];
}

- (NSNumber *)serverStarted {
	return [NSNumber numberWithBool:core->isStarted()];	
}

- (void)updateGUI:(id)arg {
	[mc updateDisplay];
	[tc updateDisplay];
}

- (void)unregisterCallbacksWithKey:(NSString *)key {
	core->unregisterCallbacks([key cStringUsingEncoding:NSASCIIStringEncoding]);
}

- (void)registerEventCallbackWithRecevier:(id)receiver 
							  andSelector:(SEL)selector
								   andKey:(NSString *)key { 
	boost::shared_ptr <CocoaEventFunctor> cef = boost::shared_ptr <CocoaEventFunctor>(new CocoaEventFunctor(receiver,
																											selector, 
																											[key cStringUsingEncoding:NSASCIIStringEncoding]));
	core->registerCallback(cef);
}

- (void)registerEventCallbackWithRecevier:(id)receiver 
							  andSelector:(SEL)selector
								   andKey:(NSString *)key
						  forVariableCode:(NSNumber *)_code {
	int code = [_code intValue];
	if(code >= 0) {
		boost::shared_ptr <CocoaEventFunctor> cef = boost::shared_ptr <CocoaEventFunctor>(new CocoaEventFunctor(receiver,
																												selector, 
																												[key cStringUsingEncoding:NSASCIIStringEncoding]));
		
		core->registerCallback(cef, code);
	}
	
}

- (void)openNetworkPreferences:(id)sender {
	[nc openAndInitWindow:sender];
}

- (NSString *)currentNetworkAddress:(id)sender {
	return listeningAddress;
}

- (NSString *)defaultNetworkAddress:(id)sender {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	NSString *defaultAddress = [defaults objectForKey:LISTENING_ADDRESS_KEY];
	
	if(defaultAddress == nil) { 
		defaultAddress = DEFAULT_HOST_IP;
	}
	
	return defaultAddress;
}

- (void)setListeningAddress:(NSString *)address {
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	[defaults setObject:address forKey:LISTENING_ADDRESS_KEY];
	[defaults synchronize];
	
	//	listeningAddress = address;
	
	//	core->stopAccepting();
	//	core->stopServer();
	//	core->setHostname([listeningAddress cStringUsingEncoding:NSASCIIStringEncoding]);
	//	core->startServer();
	//	core->startAccepting();	
	[self updateGUI:nil];	
}

- (NSArray *)variableNames {
	std::vector<std::string> varTagNames(core->getVariableNames());
	NSMutableArray *varNames = [[[NSMutableArray alloc] init] autorelease];
	
	for(std::vector<std::string>::iterator iter = varTagNames.begin();
		iter != varTagNames.end(); 
		++iter) {
		[varNames addObject:[NSString stringWithCString:iter->c_str() 
											   encoding:NSASCIIStringEncoding]];
	}
	
	return varNames;	
}


////////////////////////////////////////////////////////////////////////////////
// Private Methods
////////////////////////////////////////////////////////////////////////////////
- (void)processEvent:(MWCocoaEvent *)event {
	int code = [event code];
	
	if([[self codeForTag:@"#serverName"] intValue] == code) {
		mw::Data *server_string = [event data];
		self.serverName = [NSString stringWithCString:server_string->getString() encoding:NSASCIIStringEncoding];
	} else	if([[self codeForTag:[NSString stringWithCString:MAIN_SCREEN_INFO_TAGNAME encoding:NSASCIIStringEncoding]] intValue] == code) {
		mw::Data *main_screen_info_data = [event data];
		
		self.width = main_screen_info_data->getElement(M_DISPLAY_WIDTH_KEY).getFloat();                                                    
		self.height = main_screen_info_data->getElement(M_DISPLAY_HEIGHT_KEY).getFloat();                                                  
		self.distance = main_screen_info_data->getElement(M_DISPLAY_DISTANCE_KEY).getFloat();                                              
		self.refreshRate = main_screen_info_data->getElement(M_REFRESH_RATE_KEY).getFloat();                                               
		self.mirrorWindowEnabled = main_screen_info_data->getElement(M_ALWAYS_DISPLAY_MIRROR_WINDOW_KEY).getBool();                     
		self.displayNumber = main_screen_info_data->getElement(M_DISPLAY_TO_USE_KEY).getInteger();                                         
		
		// set the current size of the window
		unsigned int window_height = main_screen_info_data->getElement(M_MIRROR_WINDOW_BASE_HEIGHT_KEY).getInteger();
		if(window_height <= 100) {
			[size_selection selectCellAtRow:0 column:0];
		} else if(window_height <= 400) {
			[size_selection selectCellAtRow:1 column:0];		
		} else if(window_height <= 1200) {
			[size_selection selectCellAtRow:2 column:0];		
		} else {
			[size_selection selectCellAtRow:3 column:0];		
		}
	}
}

- (void)updateGeometryParamFile {
	// this shoud be better managed somehow
	
	NSXMLElement *dictionary = [NSXMLElement elementWithName:@"dictionary"];
	
	[dictionary addChild:[self createDictionaryElementWithKey:[NSString stringWithCString:M_DISPLAY_WIDTH_KEY 
																				 encoding:NSASCIIStringEncoding]
														 type:@"float" 
													 andValue:[[NSNumber numberWithFloat:self.width] stringValue]]];
	[dictionary addChild:[self createDictionaryElementWithKey:[NSString stringWithCString:M_DISPLAY_HEIGHT_KEY 
																				 encoding:NSASCIIStringEncoding]
														 type:@"float" 
													 andValue:[[NSNumber numberWithFloat:self.height] stringValue]]];
	[dictionary addChild:[self createDictionaryElementWithKey:[NSString stringWithCString:M_DISPLAY_DISTANCE_KEY 
																				 encoding:NSASCIIStringEncoding]
														 type:@"float" 
													 andValue:[[NSNumber numberWithFloat:self.distance] stringValue]]];
	[dictionary addChild:[self createDictionaryElementWithKey:[NSString stringWithCString:M_REFRESH_RATE_KEY 
																				 encoding:NSASCIIStringEncoding]
														 type:@"float" 
													 andValue:[[NSNumber numberWithFloat:self.refreshRate] stringValue]]];
	[dictionary addChild:[self createDictionaryElementWithKey:[NSString stringWithCString:M_ALWAYS_DISPLAY_MIRROR_WINDOW_KEY 
																				 encoding:NSASCIIStringEncoding]
														 type:@"integer" 
													 andValue:[[NSNumber numberWithBool:self.mirrorWindowEnabled] stringValue]]];
	[dictionary addChild:[self createDictionaryElementWithKey:[NSString stringWithCString:M_DISPLAY_TO_USE_KEY 
																				 encoding:NSASCIIStringEncoding]
														 type:@"integer" 
													 andValue:[[NSNumber numberWithInteger:self.displayNumber] stringValue]]];

	int mirror_window_size = 400;
	
	// identify the size of the window
	switch([size_selection selectedRow]) {
		case 0:
			mirror_window_size = 100;
			break;
		case 1:
			mirror_window_size = 400;
			break;
		case 2:
			mirror_window_size = 800;
			break;
		case 3:
			mirror_window_size = 1400;
			break;
		default: 
			mirror_window_size = 400;
			break;
	}
	
	[dictionary addChild:[self createDictionaryElementWithKey:[NSString stringWithCString:M_MIRROR_WINDOW_BASE_HEIGHT_KEY 
																				 encoding:NSASCIIStringEncoding]
														 type:@"integer" 
													 andValue:[[NSNumber numberWithInteger:mirror_window_size] stringValue]]];
	
	
	NSXMLElement *variable_assignment1 = [NSXMLElement elementWithName:@"variable_assignment"];
	[variable_assignment1 addAttribute:[NSXMLNode attributeWithName:@"variable" stringValue:@"#mainScreenInfo"]];
	[variable_assignment1 addChild:dictionary];

	NSXMLElement *variable_assignment2 = [NSXMLElement elementWithName:@"variable_assignment"];
	[variable_assignment2 addAttribute:[NSXMLNode attributeWithName:@"variable" stringValue:@"#serverName"]];
	[variable_assignment2 addAttribute:[NSXMLNode attributeWithName:@"type" stringValue:@"string"]];
	
	NSString *temp_server_name = @"unnamed server";
	
	if(self.serverName != nil) {
		temp_server_name = self.serverName;
	}

	[variable_assignment2 addAttribute:[NSXMLNode attributeWithName:@"value" stringValue:temp_server_name]];

	NSXMLElement *variable_assignments = [NSXMLElement elementWithName:@"variable_assignments"];
	[variable_assignments addChild:variable_assignment1];
	[variable_assignments addChild:variable_assignment2];
	NSXMLElement *monkeyml = [NSXMLElement elementWithName:@"monkeyml"];
	[monkeyml addAttribute:[NSXMLNode attributeWithName:@"version" stringValue:@"1.1"]];
	[monkeyml addChild:variable_assignments];
	
	NSXMLDocument *doco = [[[NSXMLDocument alloc] initWithRootElement:monkeyml] autorelease];
	NSString *path = [NSString stringWithCString:prependLocalPath("setup_variables.xml").string().c_str()];
	[[doco XMLDataWithOptions:NSXMLNodePrettyPrint] writeToFile:path atomically:YES];
}

- (NSXMLElement *)createDictionaryElementWithKey:(NSString *)key_string
											type:(NSString *)type_string
										andValue:(NSString *)value_string {
	NSXMLElement *key = [NSXMLElement elementWithName:@"key" stringValue:key_string];
	NSXMLElement *value = [NSXMLElement elementWithName:@"value" stringValue:value_string];
	NSXMLNode *type = [NSXMLNode attributeWithName:@"type" stringValue:type_string];
	[value addAttribute:type];
	
	NSXMLElement *dictionary_element = [NSXMLElement elementWithName:@"dictionary_element"];
	[dictionary_element addChild:key];
	[dictionary_element addChild:value];
	
	return dictionary_element;
	
}

@end
