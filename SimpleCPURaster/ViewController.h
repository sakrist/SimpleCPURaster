//
//  ViewController.h
//  SimpleCPURaster
//
//  Created by Volodymyr Boichentsov on 13/04/2019.
//  Copyright © 2019 Volodymyr Boichentsov. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "View.h"

@interface ViewController : NSViewController <MousePointerDelegate>
@property (weak) IBOutlet NSImageView *imageView;


@end

