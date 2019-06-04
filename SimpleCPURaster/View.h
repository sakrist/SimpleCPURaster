//
//  View.h
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 03/06/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol MousePointerDelegate <NSObject>

-(void)mouseDown:(CGPoint)point;
-(void)mouseDragged:(CGPoint)point;
-(void)mouseUp:(CGPoint)point;
    
@end

NS_ASSUME_NONNULL_BEGIN

@interface View : NSView

@property (nonatomic, weak) id <MousePointerDelegate> pointerDelegate;

- (void)activeTrackingArea;
    
@end

NS_ASSUME_NONNULL_END
