//
//  MWCalibratorWindow.h
//  MonkeyWorksCalibratorWindow
//
//  Created by Ben Kennedy on 5/3/08.
//  Copyright 2008 MIT. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MWCalibratorWindow : NSWindow {
	IBOutlet id delegate;
}

@property (readwrite, assign) id delegate;

@end
