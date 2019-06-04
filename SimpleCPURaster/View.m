//
//  View.m
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 03/06/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#import "View.h"

@implementation View

    
- (void)activeTrackingArea {
    NSTrackingAreaOptions options = (NSTrackingActiveAlways | NSTrackingInVisibleRect |  
                                     NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
    
    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                        options:options
                                                          owner:self
                                                       userInfo:nil];
    [self addTrackingArea:area];
}
- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Drawing code here.
}
    
-(void)mouseDown:(NSEvent *)event {
    CGPoint p = [self convertPoint:event.locationInWindow toView:self];
    [self.pointerDelegate mouseDown:p];
}
    
- (void)mouseDragged:(NSEvent *)event {
    CGPoint p = [self convertPoint:event.locationInWindow toView:self];
    [self.pointerDelegate mouseDragged:p];    
}
//-(void)mouseMoved:(NSEvent *)event {
//    CGPoint p = [self convertPoint:event.locationInWindow toView:self];
//    [self.pointerDelegate mouseMoved:p];
//}

-(void)mouseUp:(NSEvent *)event {
    CGPoint p = [self convertPoint:event.locationInWindow toView:self];
    [self.pointerDelegate mouseUp:p];
}

@end
