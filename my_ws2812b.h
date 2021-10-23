//------------------------------------
// a ws2812b LED strip
//------------------------------------
// static object defined in vMain.cpp

#pragma once
#include "vMachine.h"

#ifdef WITH_PIXELS

    #include <Adafruit_NeoPixel.h>

    #define PIXEL_LEFT_SENSOR  0
    #define PIXEL_RIGHT_SENSOR 2
    #define PIXEL_SYS_LED      1

    #define NUM_PIXELS         3

    #define MY_LED_BLACK    0x000000
    #define MY_LED_RED      0x440000
    #define MY_LED_GREEN    0x003300
    #define MY_LED_BLUE     0x000044
    #define MY_LED_CYAN     0x003333
    #define MY_LED_YELLOW   0x333300
    #define MY_LED_MAGENTA  0x330033
    #define MY_LED_WHITE    0x000044

    extern Adafruit_NeoPixel pixels;
        // in vMain.cpp

#endif
