//----------------------------------------------------
// vMachine.ino
//----------------------------------------------------
// A v-type CNC machine based on the Grbl_Esp32 library.

#include <Grbl.h>           // for grbl_init() and run_once()
#include <Config.h>         // for ENABLE_TOUCH_UI
#include <Report.h>         // for v_debug()

#include "vMachine.h"       // for V_SDCARD_CS (must come after Grbl.h


#if WITH_VMACHINE

    #define INIT_SD_DURING_SETUP

    #ifdef INIT_SD_DURING_SETUP
        #include <SD.h>
    #endif

    // ENABLE_TOUCH_UI is not currently required for Grbl_MinUI and should
    // be removed from Grbl_Esp32
    //
    // For the moment, ENABLE_TOUCH_UI is defined in Grbl_Esp32/Config.h
    // which creates CLIENT_TOUCH_UI and which allows a client to poll
    // the output using their own "extern WebUI::InputBuffer touch_ui_out".
    // that is NOT

    // SET FOLLOWING #if to 0 for no UI, or 1 to include the UI

    #if 1           // 0=no UI, 1=with UI

        #include <Grbl_MinUI.h>

        void display_init()
            // override weak definition in Grbl_Esp32
            // called after the Serial port Client has been created
        {
            v_debug("vMachine.ino display_init() started");
            Grbl_MinUI_init();

            v_debug("vMachine.ino display_init() finished");
        }

    #endif

#endif  // WITH_VMACHINE



void setup()
{
    delay(2000);

    #ifdef INIT_SD_DURING_SETUP
        bool sd_ok = SD.begin(V_SDCARD_CS);
    #endif

    grbl_init();
    #if WITH_VMACHINE
        #ifdef INIT_SD_DURING_SETUP
            v_debug("vMachine.ino SD.begin() %s during setup()",sd_ok?"WORKED OK":"FAILED");
        #endif
        v_debug("vMachine.ino setup() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    #endif
}



void loop()
{
    #if WITH_VMACHINE
        v_debug("vMachine.ino loop() started %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    #endif

    run_once();

    #if WITH_VMACHINE
        v_debug("vMachine.ino loop() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    #endif
}
