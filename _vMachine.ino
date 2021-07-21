//----------------------------------------------------
// vMachine.ino
//----------------------------------------------------
// A v-type CNC machine based on the Grbl_Esp32 library.

#include <Grbl.h>           // for grbl_init() and run_once()
#include <Config.h>         // for ENABLE_TOUCH_UI
#include <Report.h>         // for debug_serial()
#include "vMachine.h"       // for V_SDCARD_CS (must come after Grbl.h

// for the moment, ENABLE_TOUCH_UI is defined in Grbl_Esp32/Config.h
// which creates CLIENT_TOUCH_UI and which allows a client
// to poll the output using their own "extern WebUI::InputBuffer touch_ui_out"

#ifdef ENABLE_TOUCH_UI

    // You can choose between, none, two different UI libraries
    //   0 == NO LIBRARY (to test memory without linking library)
    //   1 == grbl_TouchUI (based on LVGL, uses dynamic memory)
    //   2 == grbl_MinUI (direct to TFT_eSPI, little or no dynamic memory used)

    #define WITH_UI

    #ifdef WITH_UI

        #include <Grbl_MinUI.h>

        void display_init()
            // override weak definition in Grbl_Esp32
            // called after the Serial port Client has been created
        {
            debug_serial("vMachine.ino display_init() started");
            Grbl_MinUI_init();

            debug_serial("vMachine.ino display_init() finished");
        }

    #endif
#endif



void setup()
{
    delay(2000);
    grbl_init();
    debug_serial("vMachine.ino setup() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}



void loop()
{
    debug_serial("vMachine.ino loop() started %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    run_once();
    debug_serial("vMachine.ino loop() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}
