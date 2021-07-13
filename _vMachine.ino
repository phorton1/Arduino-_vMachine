//----------------------------------------------------
// vMachine.ino
//----------------------------------------------------
// A v-type CNC machine based on the Grbl_Esp32 library.

#include <Config.h>         // for ENABLE_TOUCH_UI
#include <Report.h>         // for debug_serial()
#include "vMachine.h"       // for V_SDCARD_CS (must come after Grbl.h?? or Arduino.h??)

// for the moment, ENABLE_TOUCH_UI is defined in Grbl_Esp32/Config.h
// which creates CLIENT_TOUCH_UI and which allows a client
// to poll the output using their own "extern WebUI::InputBuffer touch_ui_out"

#ifdef ENABLE_TOUCH_UI
    #if 1
        #include <grblTouchUI.h>
        void display_init()
            // override weak definition in Grbl_Esp32
            // called after the Serial port Client has been created
        {
            info_serial("vMachine.ino display_init() started");
            touchUI_init();     // initialize the touchUI
            info_serial("vMachine.ino display_init() finished");
        }
    #endif
#endif



void setup()
{
    delay(2000);
    grbl_init();
    info_serial("vMachine.ino setup() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}



void loop()
{
    info_serial("vMachine.ino loop() started %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    run_once();
    info_serial("vMachine.ino loop() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}
