//----------------------------------------------------
// vMachine.ino
//----------------------------------------------------
// A v-type CNC machine based on the Grbl_Esp32 library.


#include <Grbl.h>           // for ENABLE_TOUCH_UI
#include <Report.h>         // for debug_serial()
#include <SD.h>             // to make it work!
#include "vMachine.h"       // for V_SDCARD_CS (must come after Grbl.h?? or Arduino.h??)


// for the moment, ENABLE_TOUCH_UI is defined in Grbl_Esp32/Config.h
// which creates CLIENT_TOUCH_UI and which allows a client
// to poll the output using their own "extern WebUI::InputBuffer touch_ui_out"


#ifdef ENABLE_TOUCH_UI
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




void setup()
{
    delay(2000);  // voodoo - was delay(2000) in original grbl_esp32 ino file

    // Put a volt meter on MISO ... it needs to be 3.3V here!
    // Note that there is a required mandatory pullup (10K) on MISO
    //
    // After months of fighting Grbl_Esp32's SDCard implementation,
    // I have finally arrived at the conclusion that the SDCard
    // must be started before Grbl_Esp32 gets it's hands on it
    //
    // "The SD card should be inited before any other SPI devices"
    // because it is not a well behaved SPI device until it is inited!
    // I had many problems fighting Grbl_Esp32's SDCard implementation.
    // Initializing the SDCard here works.
    //
    // Making it as early as possible helped. It still fails sometimes
    // from a warmboot after a re-compile. May need more playing with
    // pullup resistors.

    #if 0  // voodoo - def ENABLE_TOUCH_UI
        // For a while it seemed to help if we make sure the other CS pins are driven high
        // (they could have pullups too). This code forces them high before
        // calling SD.begin() below, but it turns out not be necessary.
        pinMode(GPIO_NUM_5,OUTPUT);             // TOUCH_CS
        pinMode(GPIO_NUM_17,OUTPUT);            // TFT_CS
        pinMode(V_SDCARD_CS,OUTPUT);            // SD CS == 21
        // pinMode(GPIO_NUM_19,INPUT_PULLUP);   // MISO
        digitalWrite(GPIO_NUM_5,1);
        digitalWrite(GPIO_NUM_17,1);
        digitalWrite(V_SDCARD_CS,1);
        delay(500);
    #endif


    #ifdef I_EVER_GET_THE_EFFING_SD_CARD_FIGURED_OUT

        // The TFT touch fails when this code works.
        // Pull the SDCard out and scrolling starts working.
        // ffs


        bool sd_ok = SD.begin(V_SDCARD_CS);
            // they say to initialize the SD card before any other SPI peripherals
            // was getting garbage from Serial.println() below till I moved this
            // before Serial.begin()

        Serial.begin(115200);       // for use before grbl_init() and debug_serial() become available
        Serial.print("\r\n");   // spacer and voodoo
        Serial.print("vMachine.ino setup() started\r\n");

        // Something is bullet holing memory or there is a rentrancy/latency issue
        // with Serial.printf, format() or something. I get garbage occasionally
        // in the Serial.print("SD.begin() failed\r\n") and/or
        // Serial.print("SD Card Type: %s\r\n",cardType_name) below.
        //
        // Hence the "voodo"
        //
        // Note that I put in \r\n after looking at the output in Putty
        // and that is mo-better in Uart0 debugging in wMonitor.cpp also

        if (!sd_ok)
        {
            Serial.print("SD.begin() failed\r\n");
        }
        else
        {
            uint8_t cardType = SD.cardType();
            const char *cardType_name =
                cardType == CARD_NONE ? "NONE" :
                cardType == CARD_MMC  ? "MMC" :
                cardType == CARD_SD   ? "SDSC" :
                cardType == CARD_SDHC ? "SDHC" :
                "UNKNOWN";
            uint64_t cardSize = SD.cardSize() / (1024 * 1024);
            Serial.print("SD Card Type: ");
            Serial.print(cardType_name);
            Serial.print("\r\n");
            Serial.printf("SD Card Size: %lluMB\r\n", cardSize);
            Serial.printf("Total space:  %lluMB\r\n", (SD.totalBytes()+1024*1024-1) / (1024 * 1024));
            Serial.printf("Used space:   %lluMB\r\n", (SD.usedBytes()+1024*1024-1) / (1024 * 1024));
        }

        // Note that we DO NOT call SD.end()
        // The Grbl_Esp32 usage of SD (and SPIFFS) needs to be reworked

        Serial.print("vMachine.ino setup() calling grbl_init()\r\n");
        Serial.flush(); // voodoo

        // grbl will switch to Uart0 output here, so we NEED to close
        // the serial port (or we get Uart0 errors)

        Serial.end();
        delay(500); // voodoo

    #endif

    // and away we go ...

    grbl_init();
    info_serial("vMachine.ino setup() completed");

}   // setup()



void loop()
{
    info_serial("vMachine.ino loop() started");
    run_once();
    info_serial("vMachine.ino loop() completed");
}
