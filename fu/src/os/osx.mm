//
//  os.cpp
//  fu
//
//  Created by Klesh Wong on 18/01/2017.
//  Copyright © 2017 Klesh Wong. All rights reserved.
//

#include "os.h"
#import <Foundation/Foundation.h>
#import <AppKit/NSPasteboard.h>
#import <AppKit/NSRunningApplication.h>
#import <AppKit/NSApplication.h>

@interface NotificationCenterDelegate : NSObject <NSUserNotificationCenterDelegate>
@end

@implementation NotificationCenterDelegate
- (BOOL)userNotificationCenter:(NSUserNotificationCenter *)center
   shouldPresentNotification:(NSUserNotification *)notification
{
    return YES;
}
@end

void Init(wxTaskBarIcon *tray)
{
    static NotificationCenterDelegate *d = [[NotificationCenterDelegate alloc] init];
    [[NSUserNotificationCenter defaultUserNotificationCenter] setDelegate:d];
}


bool IsClipboardChanged()
{
    static NSInteger lastChangeCount = 0;
    NSPasteboard *pasteboard = [NSPasteboard generalPasteboard];
    NSInteger currChangeCount = [pasteboard changeCount];
    bool isChanged = currChangeCount != lastChangeCount;
    lastChangeCount = currChangeCount;
    return isChanged;
}

void BringAppToTop()
{
    [[NSRunningApplication currentApplication] activateWithOptions:(NSApplicationActivateAllWindows | NSApplicationActivateIgnoringOtherApps)];
}



void Toast(const wxString &title, const wxString &message, bool isError)
{
    NSUserNotification *notification = [[NSUserNotification alloc] init];
    notification.title = [NSString stringWithUTF8String:title.c_str()];
    notification.informativeText = [NSString stringWithUTF8String:message.c_str()];
    notification.soundName = NSUserNotificationDefaultSoundName;
    notification.hasActionButton = NO;
    //notification.contentImage = [NSApp applicationIconImage];
    [notification setValue:[NSApp applicationIconImage] forKey:@"_identityImage"];
    [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
    if (!isError)
    {
        NSTimeInterval delay = 3;
        [[NSUserNotificationCenter defaultUserNotificationCenter] performSelector:@selector(removeDeliveredNotification:)
                                                                       withObject:notification
                                                                       afterDelay:delay];
    }
}
