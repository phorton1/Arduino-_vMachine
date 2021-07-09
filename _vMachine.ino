
#include <Grbl.h>
#include <Report.h>

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
        touchUI_init();     // to initialize the touchUI
        
        #ifdef TOUCH_UI_DIRLIST
            // remove old direcory window object first
            if (!level)
            {
                if (sp_uiDirectory)
                    delete sp_uiDirectory;
                sp_uiDirectory = new uiDirectory(
                    the_app->getContentArea(),
                    fileSystem);
                sp_uiDirectory->navigate(dir);
            }
        #endif
        
        info_serial("vMachine.ino display_init() finished");
    }
#endif




void setup()
{
    // Put a volt meter on MISO ... it needs to be 3.3V here!
    //
    // The SD card should be inited before any other SPI devices
    // because it is not a well behaved SPI device until it is inited!
    // Besides the mandatory pullup (10K) on MISO, it seems to help if
    // we make sure the other CS pins are driven high (they could have
    // pullups too). This code forces them high before calling SD.begin(),
    // but is not appropriate in the fileTypeSystem object itself.
    
    delay(2000);        // was in original Grbl_Esp32 code

    #ifdef ENABLE_TOUCH_UI
        // prh - still fighting this problem
        // card initializes on hard powerup, but not warm boot
        //
        // I somehow relate this to the fact that I can hardly ever (never on new LENOVO3)
        // rely on the Arduino IDE to upload the program unless I press the button on the ESP32.
        // It *could* be related to the USB port power supply, the SD Card SPI initialization,
        // and the TFT all being cojoined at the hip.
        //
        // Maybe the ESP32 gets flakey with usage.
        
        pinMode(GPIO_NUM_5,OUTPUT);             // TOUCH_CS
        pinMode(GPIO_NUM_17,OUTPUT);            // TFT_CS
        // pinMode(GPIO_NUM_19,INPUT_PULLUP);   // MISO
        digitalWrite(GPIO_NUM_5,1);
        digitalWrite(GPIO_NUM_17,1);
        vTaskDelay(250);
    #endif
    
    grbl_init();    
    info_serial("vMachine.ino setup() completed");
}



void loop()
{
    info_serial("vMachine.ino loop() started");
    run_once();
    info_serial("vMachine.ino loop() completed");
}
