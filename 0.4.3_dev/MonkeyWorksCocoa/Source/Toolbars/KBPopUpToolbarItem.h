//
//  KBPopUpToolbarItem.h
//  --------------------
//
//  Created by Keith Blount on 14/05/2006.
//  Copyright 2006 Keith Blount. All rights reserved.
//
//	Provides a toolbar item that performs its given action if clicked, or displays a pop-up menu
//	(if it has one) if held down for over half a second.
//

#import <Cocoa/Cocoa.h>
#import "MWToolbarItem.h"


@interface KBPopUpToolbarItem : MWToolbarItem
{
}
- (void)setMenu:(NSMenu *)menu;
- (NSMenu *)menu;
@end
