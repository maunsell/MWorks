//
//  CalibratorRecord.h
//  MonkeyWorksCalibratorWindow
//
//  Created by bkennedy on 9/12/06.
//  Copyright 2006 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MWCalibratorRecord : NSObject {
	NSString *name;

	NSMutableArray *hParameters;
	NSMutableArray *vParameters;
}
- (id)init:(NSString *)calibratorName:(int)maxHParams:(int)maxVParams;
- (NSString *)getCalibratorName;
- (int)getNHParameters;
- (int)getNVParameters;
- (double)getHParameter:(int)index;
- (double)getVParameter:(int)index;
- (void)setHParameter:(int)index:(double)value;
- (void)setVParameter:(int)index:(double)value;

@end
