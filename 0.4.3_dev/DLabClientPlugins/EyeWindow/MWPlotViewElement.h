//
//  MWPlotViewElement.h
//  MonkeyWorksEyeWindow
//
//	A Generic drawable object, meant to be inherited.
//
//  Created by David Cox on 1/31/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@protocol MWPlotViewElement

//- (NSRect) bounds;
- (void) stroke:(NSRect)visible;

@end
