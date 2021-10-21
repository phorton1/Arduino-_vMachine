//----------------------------------------------------
// vMachine.ino
//----------------------------------------------------
// A v-type CNC machine based on the FluidNC library.

#include <FluidNC.h>
#include <Config.h>         // FluidNC
#include <FluidDebug.h>     // FluidNC_extensions

#include "vMachine.h"       // for V_SDCARD_CS (must come after FluidNC.h

#define INIT_SD_DURING_SETUP

#ifdef INIT_SD_DURING_SETUP
    #include <SD.h>
#endif


#ifdef WITH_UI
    #include <FluidNC_UI.h>
    #include <gApp.h>

    void display_init()
        // override weak definition in FluidNC
        // called after the Serial port Client has been created
    {
        g_debug("vMachine.ino display_init() started");
        FluidNC_UI_init();
        g_debug("vMachine.ino display_init() finished");
    }


    void user_realtime_command(uint8_t command, Print &client)
    {
        // ctrl keys
        //
        // ctrl-Q			CMD_LIVE_Z_PLUS_COARSE
        // ctrl-W       	CMD_LIVE_Z_PLUS_FINE
        // ctrl-E       	CMD_LIVE_Z_RESET
        // ctrl-R       	CMD_LIVE_Z_MINUS_FINE
        // ctrl-T       	CMD_LIVE_Z_MINUS_COARSE
        //
        // 0x01 - ctrl-A
        // 0x02 - ctrl-B
        // 0x03 - ctrl-C	concole exit
        // 0x04 - ctrl-D	console clear
        // 0x05 - ctrl-E	CMD_LIVE_Z_RESET
        // 0x06 - ctrl-F	console experimental gcode
        // 0x07 - ctrl-G	console gcode
        // 0x08 - ctrl-H
        // 0x09 - ctrl-I
        // 0x0a - ctrl-J
        // 0x0b - ctrl-K
        // 0x0c - ctrl-L
        // 0x0d - ctrl-M
        // 0x0e - ctrl-N
        // 0x0f - ctrl-O    CMD_UI_SCREEN_GRAB_PENDING
        // 0x10 - ctrl-P    CMD_UI_SCREEN_GRAB
        // 0x11 - ctrl-Q	CMD_LIVE_Z_PLUS_COARSE
        // 0x12 - ctrl-R    CMD_LIVE_Z_MINUS_COARSE
        // 0x13 - ctrl-S
        // 0x14 - ctrl-T	CMD_LIVE_Z_MINUS_COARSE
        // 0x15 - ctrl-U
        // 0x16 - ctrl-V
        // 0x17 - ctrl-W	CMD_LIVE_Z_PLUS_FINE
        // 0x18 - ctrl-X	FluidNC reset (console upload if enabled)
        // 0x19 - ctrl-Y
        // 0x1a - ctrl-Z
        // 0x1b - ESC
        // 0x1c -
        // 0x1d -
        // 0x1e -
        // 0x1f -

        g_debug("user_realtime_command(0x%02x)",command);

        switch (command)
        {
			case CMD_UI_SCREEN_GRAB :
				gApplication::doScreenGrab(false);
				break;
			case CMD_UI_SCREEN_GRAB_PENDING :
				gApplication::doScreenGrab(true);
				break;
        }
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
        g_info("vMachine.ino SD.begin() %s during setup()",sd_ok?"WORKED OK":"FAILED");
    #endif

    g_info("vMachine.ino setup() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}



void loop()
{
    g_info("vMachine.ino loop() started %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
    run_once();
    delay(1000);    // for display of following message to work with FluidNC asynch debug output
    g_info("vMachine.ino loop() completed %d/%dK",xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
 }
