//----------------------------------------------------
// vMachine.ino
//----------------------------------------------------
// A v-type CNC machine based on the FluidNC library.

#include <FluidNC.h>
#include <Config.h>         // FluidNC
#include <Report.h>         // FluidNC

#include "vMachine.h"       // for V_SDCARD_CS (must come after FluidNC.h

#define INIT_SD_DURING_SETUP

#ifdef INIT_SD_DURING_SETUP
    #include <SD.h>
#endif

// SET FOLLOWING #if to 0 for no UI, or 1 to include the UI

#if 1           // 0=no UI, 1=with UI

    #include <FluidNC_UI.h>

    void display_init()
        // override weak definition in FluidNC
        // called after the Serial port Client has been created
    {
        v_debug("vMachine.ino display_init() started");
        FluidNC_UI_init();
        v_debug("vMachine.ino display_init() finished");
    }

#endif


void setup()
{
    delay(2000);

    #ifdef INIT_SD_DURING_SETUP
        bool sd_ok = SD.begin(V_SDCARD_CS);
    #endif

    main_init();

    #ifdef INIT_SD_DURING_SETUP
        v_debug("vMachine.ino SD.begin() %s during setup()",sd_ok?"WORKED OK":"FAILED");
    #endif

    v_debug("vMachine.ino setup() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}



void loop()
{
    v_debug("vMachine.ino loop() started %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    run_once();
    delay(1000);    // for display of following message to work with FluidNC asynch debug output
    v_debug("vMachine.ino loop() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
 }
