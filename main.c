#include <stdio.h>
#include <IOKit/hid/IOHIDManager.h>
#include "./keymap.h"

static void AddUsage(CFMutableArrayRef array, int inUsagePage, int inUsage) {
    CFMutableDictionaryRef d = CFDictionaryCreateMutable(NULL, 2, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFDictionaryAddValue(d, CFSTR(kIOHIDDeviceUsagePageKey), CFNumberCreate(NULL, kCFNumberSInt32Type, &inUsagePage));
    CFDictionaryAddValue(d, CFSTR(kIOHIDDeviceUsageKey), CFNumberCreate(NULL, kCFNumberSInt32Type, &inUsage));
    CFArrayAppendValue(array, d);
}

static void ValueCallback(void* context, IOReturn result, void* sender, IOHIDValueRef value) {
    IOHIDElementRef elem = IOHIDValueGetElement(value);
    if (IOHIDElementGetUsagePage(elem) != 0x07)
    {
        return;
    }
    uint32_t scancode = IOHIDElementGetUsage(elem);
    if (scancode < 4 || scancode > 231)
    {
        return;
    }
    CFIndex pressed = IOHIDValueGetIntegerValue(value);
    if(pressed) {
        uint32_t keymap_index = scancode * 2;
        char* keymap_entry = (keymap_index < KEYMAP_MAX ? KEYMAP[keymap_index] : NULL);
        if(keymap_entry != NULL) {
            printf("%s ", keymap_entry);
        } else {
            printf("<%u> ", scancode);
        }
        fflush(stdout);
    }
}

int main() {
    IOHIDManagerRef ioManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (CFGetTypeID(ioManager) != IOHIDManagerGetTypeID())
    {
        perror("can't create manager");
        return 1;
    }
    CFMutableArrayRef matchArray = CFArrayCreateMutable(NULL, 2, &kCFTypeArrayCallBacks);
    AddUsage(matchArray, kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
    AddUsage(matchArray, kHIDPage_GenericDesktop, kHIDUsage_GD_Keypad);
    IOHIDManagerSetDeviceMatchingMultiple(ioManager, matchArray);
    IOHIDManagerRegisterInputValueCallback(ioManager, ValueCallback, NULL);
    IOHIDManagerOpen(ioManager, kIOHIDOptionsTypeNone);
    CFRunLoopRef runLoop = CFRunLoopGetMain();
    IOHIDManagerScheduleWithRunLoop(ioManager, runLoop, kCFRunLoopDefaultMode);
    CFRunLoopRun();
    return 0;
}
