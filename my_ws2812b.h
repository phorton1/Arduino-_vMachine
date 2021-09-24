//------------------------------------
// a ws2812b LED strip
//------------------------------------
// static object defined in vMachine.cpp

#pragma once

#define NUM_PIXELS         4
#define PIXEL_LEFT_SENSOR  3
#define PIXEL_RIGHT_SENSOR 2
#define PIXEL_SYS_LED      0

#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel pixels;
    // in vMachine.cpp

