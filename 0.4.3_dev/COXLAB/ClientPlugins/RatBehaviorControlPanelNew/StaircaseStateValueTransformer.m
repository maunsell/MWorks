//
//  StaircaseStateValueTransformer.m
//  RatBehaviorControlPanel
//
//  Created by David Cox on 5/8/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "StaircaseStateValueTransformer.h"


@implementation StaircaseStateValueTransformer

+ (Class)transformedValueClass { return [NSString class]; }

+ (BOOL)allowsReverseTransformation{
	return YES;
}

- (NSNumber *)transformedValue:(NSNumber *)index{
	if([index intValue] < 0){
		return @"Frozen";
	}
	
	if([index intValue] > 0){
		return @"Active";
	}
	
	return @"Disabled";
}

- (NSNumber *)reverseTransformedValue:(NSString *)name{
	if([name isEqualToString:@"Frozen"]){
		return [NSNumber numberWithInt:-1];
	}
	
	if([name isEqualToString:@"Active"]){
		return [NSNumber numberWithInt:1];
	}
	
	return [NSNumber numberWithInt:0];
}

@end
