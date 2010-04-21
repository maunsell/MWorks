//
//  MWVarEntry.h
//  MonkeyWorksMATLABWindow
//
//  Created by Ben Kennedy on 9/22/07.
//  Copyright 2007 MIT. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface MWVarEntry : NSObject {
	BOOL selected;
	NSString *variableName;
}

- (NSNumber *)selected;
- (void)setSelected:(BOOL)selected;
- (NSString *)name;

@end
