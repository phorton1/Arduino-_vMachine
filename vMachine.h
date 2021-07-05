#pragma once

// A maslow-like vMachine for the Esp32_Grbl project
// This version is built on the Yaml_Settings branch
// of the Esp32_Grbl repository as of July 4, 2021

// new version does not use global defines, except
// for my legacy vSensor defines here:

#define X_VLIMIT_PIN            GPIO_NUM_34     // my code only
#define Y_VLIMIT_PIN            GPIO_NUM_35     // my code only
#define V_LIMIT_LED_PIN         GPIO_NUM_22     // my code only


#include <Machine/MachineConfig.h>


class vMachine : public Machine::MachineConfig
{
    public:

        vMachine()
        {
            setInstance(this);
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





#if 0

//----------------------------------------
// GRBL Pin Defines
//----------------------------------------
// All of the following defines, except those labelled
// "my code only" do something in the core esp32_gbl code.

// Tell GRBL to turn on and use kinematics

#define USE_KINEMATICS
#define FWD_KINEMATICS_REPORTING    // report in cartesian

// define basic Axes and Pins

#define X_STEP_PIN              GPIO_NUM_27
#define X_DIRECTION_PIN         GPIO_NUM_26
#define X_VLIMIT_PIN            GPIO_NUM_34     // my code only
#define Y_STEP_PIN              GPIO_NUM_33
#define Y_DIRECTION_PIN         GPIO_NUM_32
#define Y_VLIMIT_PIN            GPIO_NUM_35     // my code only
#define STEPPERS_DISABLE_PIN    GPIO_NUM_14
#define V_LIMIT_LED_PIN         GPIO_NUM_22     // my code only

// use servo for z-axis

#define Z_SERVO_PIN             GPIO_NUM_13

//------------------------------------
// SD Card
//------------------------------------
// Note that HSPI (hardware 2nd SPI) is MOSI(13) MISO(12) CLK(14) CS(15)
// Uses defaults for ESP32 VSPI library (except CS):
//
//   #define SPI_CLK      GPIO_NUM_18
//   #define SPI_MISO     GPIO_NUM_19
//   #define SPI_MOSI     GPIO_NUM_23
//   #define DEFAULT_SPI_CS  GPIO_NUM_5

#define GRBL_SDCARD_CS    GPIO_NUM_21
    // I invented this define.  It is in the main code.
    // See my notes regarding overloading of GRBL_SPI_SS
    // #define in SDCard.cpp

//------------------------------------
// Touchscreen
//------------------------------------
// 240x320 SPI ILI9341 TFT with XPT/HR2046 touch screen
// Pins defined in TFT_eSPI/prh_Setup.h
// TFT and Touch share the SPI bus MOSI, MISO and CLK with the CD Card.
// Also see comments in Custom/touchScreen.cpp.
// TFT uses following additional pins:
//
//   #define TFT_CS       GPIO_NUM_17
//   #define TFT_DC/RS    GPIO_NUM_27
//
// Touch uses the one following additinal pin, which
// happens to be the default ESP32 SPI CS pin
//
//   #define TOUCH_CS    GPIO_NUM_5     // default ESP32 SPI CS pin (outputs PWM signal at boot)
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
//
//  D2  GPIO_NUM_9  - off limits (SPIFF)
//  D3  GPIO_NUM_10 - off limits (SPIFF)
//  CMD GPIO_NUM_11 - off limits (SPIFF)
//  CLK GPIO_NUM_6  - off limits (SPIFF)
//  D0  GPIO_NUM_7  - off limits (SPIFF)
//  D1  GPIO_NUM_8  - off limits (SPIFF)


//----------------------------------------
// GRBL $nn Defaults
//----------------------------------------
// The following defines are used by the
// core GRBL code to set $nn defaults that can
// be reset with the $RST=* factory reset command.

#define DEFAULT_SOFT_LIMIT_ENABLE 1     // $20
#define DEFAULT_HARD_LIMIT_ENABLE 0     // $21
#define DEFAULT_HOMING_ENABLE 1         // $22

#define DEFAULT_HOMING_DIR_MASK 7       // $23
    // This is inappropriately named.  It is represented in the
    // global variable 'dir_invert_mask' which applies to more
    // than just homing.
    //
    // It is, in fact, the basic notion of whether each axis
    // moves in a positive, or negative direction, and whether
    // MAX_TRAVEL should be interpreted as a positive or negative
    // number in limit checking
    //
    // If a bit is set for an axis in this mask, then that
    // axis moves from 0 .. max_travel in motor/step units,
    // as you would expect.
    //
    // If a bit for the axis is NOT set in this mask, then the
    // axis moves from 0 .. -max_travel in (negative) motor/step
    // units.
    //
    // It is called HOMING_DIR_MASK because if a bit is set
    // then the axis moves negative while HOMING, but that
    // is a limited concept of how it is used in the GRBL
    // code. The default should have been 0 to move in
    // normal positive directions.
    //
    // In our machine, increasing Z positive is away from the work
    // surface and decreasing it moves towards the work surface.

#define DEFAULT_HOMING_SEEK_RATE 4000   // %25 - mm/min - GRBL was 2000
#define DEFAULT_HOMING_FEED_RATE 400    // %26 - mm/min - GRBL was 200
#define DEFAULT_HOMING_PULLOFF  15.0    // $27 - mm - GRBL was 1

#define DEFAULT_X_STEPS_PER_MM 50.0     // $100
#define DEFAULT_Y_STEPS_PER_MM 50.0     // $101
#define DEFAULT_Z_STEPS_PER_MM 10.0     // $103 - granularity of Z axis moves

#define DEFAULT_X_MAX_RATE 4000.0       // $110 - mm/min
#define DEFAULT_Y_MAX_RATE 4000.0       // $111
#define DEFAULT_Z_MAX_RATE 10000.0      // $112 - this should be high, see prh_readme.cpp

#define DEFAULT_X_ACCELERATION 200.0    // $120 - mm/sec^2  from polar_coaster.h
#define DEFAULT_Y_ACCELERATION 200.0    // $121 - mm/sec^2  200 mm/sec^2 = 720000 mm/min^2
#define DEFAULT_Z_ACCELERATION 1000.0   // $122 - this should be high, see prh_readme.cpp

#define DEFAULT_X_MAX_TRAVEL 1000.0     // $130 - mm NOTE: Must be a positive value.
#define DEFAULT_Y_MAX_TRAVEL 1000.0     // $131
#define DEFAULT_Z_MAX_TRAVEL 90.0       // $132 - see comments in prh_readme.cpp

#define DEFAULT_DIRECTION_INVERT_MASK  0    //  $3 = Stepper/DirInvert
    // This is the same as the GRBL default, but is here for confirmation.
    // The way my blue SG90 servo is mounted, it moves counter clockwise
    // away from the work surface with increasing motor values. In alternate
    // designs you might need to change this.

#endif // no longer used #defines
