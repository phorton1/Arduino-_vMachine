// A maslow-like vMachine for the FluidNC project
// This version is built on the Yaml_Settings branch
// of the FluidNC repository as of July 4, 2021

#pragma once

#include <Machine/MachineConfig.h>      // FluidNC - required

#define WITH_UI
// if you turn this on, you probably want to turn off FluidNC_UI/gDefs.h::UI_WITH_MESH


#define X_VLIMIT_PIN                GPIO_NUM_39
#define Y_VLIMIT_PIN                GPIO_NUM_36
#define V_SDCARD_CS                 GPIO_NUM_4
#define NEO_PIXEL_PIN               GPIO_NUM_15

//----------------------------------------
// Pin Usage
//----------------------------------------
// #define X_STEP_PIN               GPIO_NUM_13
// #define X_DIRECTION_PIN          GPIO_NUM_14
// #define Y_STEP_PIN               GPIO_NUM_26
// #define Y_DIRECTION_PIN          GPIO_NUM_25
// #define STEPPERS_DISABLE_PIN     GPIO_NUM_27
// #define Z_SERVO_PIN              GPIO_NUM_33
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
// #define TFT_CS                   GPIO_NUM_5
//
// Touch uses the one following additinal pin, which
// happens to be the default ESP32 SPI CS pin
//
// #define TOUCH_CS                 GPIO_NUM_22
//


#define V_NUM_AXES   3


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
