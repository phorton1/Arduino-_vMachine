// A maslow-like vMachine for the Esp32_Grbl project
// This version is built on the Yaml_Settings branch
// of the Esp32_Grbl repository as of July 4, 2021

#pragma once

#include <Machine/MachineConfig.h>

#define X_VLIMIT_PIN                GPIO_NUM_34
#define Y_VLIMIT_PIN                GPIO_NUM_35
#define V_LIMIT_LED_PIN             GPIO_NUM_22
#define V_SDCARD_CS                 GPIO_NUM_21


//----------------------------------------
// Pin Usage
//----------------------------------------
// #define X_STEP_PIN               GPIO_NUM_27
// #define X_DIRECTION_PIN          GPIO_NUM_26
// #define Y_STEP_PIN               GPIO_NUM_33
// #define Y_DIRECTION_PIN          GPIO_NUM_32
// #define STEPPERS_DISABLE_PIN     GPIO_NUM_14
// #define Z_SERVO_PIN              GPIO_NUM_13
//
// SD Card

// Note that HSPI (hardware 2nd SPI) is MOSI(13) MISO(12) CLK(14) CS(15)
// Uses defaults for ESP32 VSPI library (except CS) which we use for TOUCH_CS
//
// #define SPI_CLK                  GPIO_NUM_18
// #define SPI_MISO                 GPIO_NUM_19
// #define SPI_MOSI                 GPIO_NUM_23
// #define DEFAULT_SPI_CS           GPIO_NUM_5
//
// Touchscreen
//
// 240x320 SPI ILI9341 TFT with XPT/HR2046 touch screen
// Pins defined in TFT_eSPI/prh_Setup.h
// TFT and Touch share the SPI bus MOSI, MISO and CLK with the CD Card.
// TFT uses following additional pins:
//
// #define TFT_CS                   GPIO_NUM_17
//
// Touch uses the one following additinal pin, which
// happens to be the default ESP32 SPI CS pin
//
// #define TOUCH_CS                 GPIO_NUM_5
//
//---------------------------------------------
// Remaining Esp32 Devkit pins at this time
//---------------------------------------------
// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
// Notes say "ADC2" cannot be used while WiFi is active.
//
//  VP  GPIO_NUM_36 - ADC1  input only
//  VN  GPIO_NUM_39 - ADC1  input only
//      GPIO_NUM_25 - ADC2* input ok, output ok
//      GPIO_NUM_15 - ADC2* input ok, output ok, outputs PWM signal at boot
//      GPIO_NUM_12 - ADC2* input "ok", output ok, boot fail if pulled high
//      GPIO_NUM_4  - ADC2* input ok, output ok
//  RX  GPIO_NUM_3  -       input "ok", output nope "RX pin", HIGH at boot
//      GPIO_NUM_2  - ADC2*  input ok, output ok, connected to onboard LED
//  TX  GPIO_NUM_1  -       input nope "TX pin", output "ok", debug output at boot
//      GPIO_NUM_0  - ADC2* input "pulled up", output "ok", outputs PWM signal at boot
//  D2  GPIO_NUM_9  - off limits (SPIFF)
//  D3  GPIO_NUM_10 - off limits (SPIFF)
//  CMD GPIO_NUM_11 - off limits (SPIFF)
//  CLK GPIO_NUM_6  - off limits (SPIFF)
//  D0  GPIO_NUM_7  - off limits (SPIFF)
//  D1  GPIO_NUM_8  - off limits (SPIFF)



class vMachine : public Machine::MachineConfig
{
    public:

        vMachine()
        {
            config = this;
            initSettings();
        }

        float getMachineWidth()			    { return v_machine_width; }
        float getMachineHeight()			{ return v_machine_height; }
        float getDistBetweenMotors()		{ return v_dist_between_motors; }
        float getMotorOffsetX()         	{ return (getDistBetweenMotors() - getMachineWidth())/2; }
        float getMotorOffsetY()			    { return v_motor_offset_y; }
        float getSprocketRadius()			{ return v_sprocket_radius; }
        float getChainLeftTolerance()		{ return v_chain_left_tolerance; }
        float getChainRightTolerance()	    { return v_chain_right_tolerance; }
        float getSagCorrection()			{ return v_sag_correction; }
        float getSledRadius()				{ return v_sled_radius; }
        float getForwardGuessTolerance()	{ return v_forward_guess_tolerance; }
        int   getMaxForwardGuesses()		{ return v_max_forward_guesses; }
        float getGuessMaxChainLength()	    { return v_guess_max_chain_length; }
        float getZeroLength()				{ return v_zero_length; }
        float getLeftZeroOffset()			{ return v_left_zero_offset; }
        float getRightZeroOffset()		    { return v_right_zero_offset; }
        float getSafeAreaOffset()			{ return v_safe_area_offset; }
        float getZAxisSafePosition()		{ return v_zaxis_safe_position; }
        float getLineSegmentLength()		{ return v_line_segment_length; }

    protected:

        void afterParse() override;
        void group(Configuration::HandlerBase& handler) override;

        void  initSettings();

        float v_machine_width;
        float v_machine_height;
        float v_dist_between_motors;
        float v_motor_offset_y;
        float v_sprocket_radius;
        float v_chain_left_tolerance;
        float v_chain_right_tolerance;
        float v_sag_correction;
        float v_sled_radius;
        float v_forward_guess_tolerance;
        int   v_max_forward_guesses;
        float v_guess_max_chain_length;
        float v_zero_length;
        float v_left_zero_offset;
        float v_right_zero_offset;
        float v_safe_area_offset;
        float v_zaxis_safe_position;
        float v_line_segment_length;

};  // class vMachine


extern vMachine v_machine;


extern void v_info(const char *format, ...);
extern void v_debug(const char *format, ...);
extern void v_error(const char *format, ...);
    // decouple from calling Grbl_Esp32 output routines directly
    // at a small cost in code and stack space
