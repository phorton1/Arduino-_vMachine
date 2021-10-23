//----------------------------------------------------
// vMachine.ino
//----------------------------------------------------
// A v-type CNC machine based on the FluidNC library.

#include "vMachine.h"       // for V_SDCARD_CS
#include <FluidDebug.h>     // FluidNC_extensions


#define INIT_SD_DURING_SETUP

#ifdef INIT_SD_DURING_SETUP
    #include <SD.h>
#endif



void setup()
{
    delay(2000);

    #ifdef INIT_SD_DURING_SETUP
        bool sd_ok = SD.begin(V_SDCARD_CS);
    #endif

    main_init();        // call FluidNC setup() method

    #ifdef INIT_SD_DURING_SETUP
        g_info("vMachine.ino SD.begin() %s during setup()",sd_ok?"WORKED OK":"FAILED");
    #endif

    g_info("vMachine.ino setup() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}



void loop()
{
    g_info("vMachine.ino loop() started %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    run_once();     // call FluidNC loop() method
    delay(1000);    // for display of following message to work with FluidNC asynch debug output
    g_info("vMachine.ino loop() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
 }
