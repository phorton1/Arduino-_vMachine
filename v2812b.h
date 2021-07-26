//------------------------------------
// a ws2812b LED strip
//------------------------------------
// static object defined in vMachine.cpp

#pragma once

#include "vMachine.h"       // for V_SDCARD_CS (must come after Grbl.h
#if WITH_VMACHINE

#define WITH_V2812B

#ifdef WITH_V2812B
    // if this is defined, a neopixel strip is expected on
    // V_LIMIT_LED_PIN.  Otherwise a single RED led is
    // expected.

#define NUM_PIXELS         4
#define PIXEL_LEFT_SENSOR  3
#define PIXEL_RIGHT_SENSOR 2

#include <Adafruit_NeoPixel.h>

extern Adafruit_NeoPixel pixels;

#endif  // WITH_V2812B
#endif  // WITH_VMACHINE