

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
    Serial.begin(115200);       // for use before grbl_init() and debug_serial() become available
    delay(2000);                // was in original Grbl_Esp32 code
    Serial.println("vMachine.ino setup() started");

    // Put a volt meter on MISO ... it needs to be 3.3V here!
    // Note that there is a required mandatory pullup (10K) on MISO
    //
    // The SD card should be inited before any other SPI devices
    // because it is not a well behaved SPI device until it is inited!
    // I had many problems fighting Grbl_Esp32's SDCard implementation.
    // Initializing the SDCard here works.

    #if 0  // def ENABLE_TOUCH_UI
        // For a while tt seemed to help if we make sure the other CS pins are driven high
        // (they could have pullups too). This code forces them high before
        // calling SD.begin() below, but it turns out not be necessary if you initialize
        // the SD Card correctly (as the first SPI device).

        pinMode(GPIO_NUM_5,OUTPUT);             // TOUCH_CS
        pinMode(GPIO_NUM_17,OUTPUT);            // TFT_CS
        // pinMode(GPIO_NUM_19,INPUT_PULLUP);   // MISO
        digitalWrite(GPIO_NUM_5,1);
        digitalWrite(GPIO_NUM_17,1);
        vTaskDelay(250);
    #endif

    // After months of fighting Grbl_Esp32's SDCard implementation,
    // I have finally arrived at the conclusion that the SDCard
    // must be started before Grbl_Esp32 gets it's hands on it

    if (!SD.begin(V_SDCARD_CS))
    {
        Serial.println("SD.begin() failed");
    }
	else
    {
		uint8_t cardType = SD.cardType();
        Serial.printf("SD Card Type: %s\n",
            cardType == CARD_NONE ? "NONE" :
            cardType == CARD_MMC  ? "MMC" :
            cardType == CARD_SD   ? "SDSC" :
            cardType == CARD_SDHC ? "SDHC" :
            "UNKNOWN");
        uint64_t cardSize = SD.cardSize() / (1024 * 1024);
        Serial.printf("SD Card Size: %lluMB\n", cardSize);
        Serial.printf("Total space:  %lluMB\n", (SD.totalBytes()+1024*1024-1) / (1024 * 1024));
        Serial.printf("Used space:   %lluMB\n", (SD.usedBytes()+1024*1024-1) / (1024 * 1024));
	}

    // Note that we DO NOT call SD.end()
    // The Grbl_Esp32 usage of SD (and SPIFFS) needs to be reworked

    Serial.println("vMachine.ino setup() calling grbl_init()");

    // grbl will switch to Uart0 output here, so we NEED to close
    // the serial port (or we get Uart0 errors)

    Serial.end();

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
