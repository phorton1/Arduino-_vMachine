// vMachineSettings


#include "vMachine.h"

#if WITH_VMACHINE

#include <Report.h>


// Set G55 to 12x9" paper and go to upper right corner
//
// 		g10 L2 p2 x47.6 y35.7 z0
// 		g0 x304.8 y228.6
//
// Set G56 to 8x11.5" paper
//
// 		g10 L2 p3 x60.3 y42.05 z0
//		G0 x279.4 y215.9

//----------------------------------------
// Default vMachine Machine Definition
//----------------------------------------
// Defines the dimensions and some behavior of the vMachine
// machine that was created as an initial prototype
// "reference" machine for this code.
//
// These may be modified by changing the $vMachine settings
// at runtime or via the WEBUI  The following values will be
// initially used and after a $RST=* factory reset command.
//
// Note that after changing $nn or named $/ preferences, you
// should reboot the machine as many of these are read at system
// startup and cached in memory for usage.
//
// The "machine area" is 400x300mm == 15.748" x 11.811"

#define VMACHINE_DEFAULT_MACHINE_WIDTH          400  // mm, $ESP910, $vMachine/machineWidth
#define VMACHINE_DEFAULT_MACHINE_HEIGHT         300  // mm, $ESP911, $vMachine/machineHeight;

// the work area is 5" and a distance below the center line between motors

#define VMACHINE_DEFAULT_DIST_BETWEEN_MOTORS    (28.75 * 25.4)       // 730.25 mm, $ESP912, $vMachine/distBetweenMotors
#define VMACHINE_DEFAULT_MOTOR_OFFSET_Y         ((5 * 25.4) + 24.3)  // 151.30 mm, $ESP914, $vMachine/motorOffsetY;

// defaults for kinematic calculation details

#define VMACHINE_DEFAULT_SPROCKET_RADIUS       (9.6/2)  // mm, $ESP920, $vMachine/sprocketRadius

// if using sag calculations (see vKinematics.cpp)

#define VMACHINE_DEFAULT_CHAIN_LEFT_TOLERANCE    0.0   // $ESP922, $vMachine/leftChainTolerance
#define VMACHINE_DEFAULT_CHAIN_RIGHT_TOLERANCE   0.0   // $ESP924, $vMachine/rightChainTolerance
#define VMACHINE_DEFAULT_CHAIN_SAG_CORRECTION    0.0   // $ESP926, $vMachine/chainSagCorrection
#define VMACHINE_DEFAULT_SLED_RADIUS             0.0   // mm, $ESP928, $vMachine/rotationDiskRadius

// The number of guesses in forward kinematics could be a
// performance or accuracy issue. The following are only
// used in forward kinematics which is only used for reporting.

#define VMACHINE_DEFAULT_FORWARD_GUESS_TOLERANCE  0.05  // mm,$ESP935,$vMachine/forwardGuessTolerance
    // How close should we try to get to an X,Y position in forward kinematics.
#define VMACHINE_DEFAULT_MAX_FORWARD_GUESSES      200   // $ESP937,$vMachine/maxForwardGuesses
    // number of tries to converge to FORWARD_GUESS_TOLERANCE or fail
#define VPLOTTEER_DEFAULT_GUESS_MAX_CHAIN_LENGTH  1000  // mm, $ESP939, $vMachine/guessMaxchainLength
    // should be about the length of chain from bottom of opposite work corner to sprocket plus some slop
    // foward guessing stops (fails) if it gets to a chain length over this

// Behavioral Defaults
// See comments in vMachine.cpp how these ZERO lengths affect
// the internal representation of machine position and play into
// the "safe" area.

#define VMACHINE_DEFAULT_ZERO_LENGTH         223.959     // mm, $ESP943, $vMachine/zeroLength
#define VMACHINE_DEFAULT_LEFT_ZERO_OFFSET    -10.000     // mm, $ESP945, $vMachine/leftZeroOffsest  (-10 mm == -500 steps)
#define VMACHINE_DEFAULT_RIGHT_ZERO_OFFSET   -10.000     // mm, $ESP947, $vMachine/rightZeroOffsest

// we define a safe area around the work area to constrain movements
// and for soft limit checks.  This should all be within the augmented
// work area as defined by the Zero Offset settings (explained below)

#define VMACHINE_DEFAULT_SAFE_AREA_OFFSET       5.0     // mm, $ESP951, $vMachine/safeAreaOffset

#define VMACHINE_DEFAULT_Z_AXIS_SAFE_POSITION   20 	// mm, $ESP953, $vMachine/zAxisHomeSafe
	// safe position for Z axis during homing
#define VMACHINE_DEFAULT_LINE_SEGMENT_LENGTH    0.5	// mm, $ESP955, $vMachine/lineSegLength
	// length of subsegments done by cartesian_to_motors()



void vMachine::afterParse() // override
{
	Machine::MachineConfig::afterParse();
}



void vMachine::group(Configuration::HandlerBase& handler) // override
{
	// handler.section("vMachine", NULL);

	handler.item("machine_width", 			v_machine_width);
	handler.item("machine_height", 			v_machine_height);
	handler.item("dist_between_motors",		v_dist_between_motors);
	handler.item("motor_offset_y",			v_motor_offset_y);
	handler.item("sprocket_radius",         v_sprocket_radius);
	handler.item("chain_tolerance_left",    v_chain_left_tolerance);
	handler.item("right_tolerance_right=",  v_chain_right_tolerance);
	handler.item("sag_correction",          v_sag_correction);
	handler.item("sled_radius",             v_sled_radius);
	handler.item("guess_tolerance", 		v_forward_guess_tolerance);
	handler.item("guess_num_max",     		v_max_forward_guesses);
	handler.item("guess_max_chain_length",  v_guess_max_chain_length);
	handler.item("zero_length",             v_zero_length);
	handler.item("zero_offset_left",        v_left_zero_offset);
	handler.item("zero_offset_right",       v_right_zero_offset);
	handler.item("safe_area_offset",        v_safe_area_offset);
	handler.item("safe_position_z",     	v_zaxis_safe_position);
	handler.item("line_segment_length",     v_line_segment_length);

	Machine::MachineConfig::group(handler);
}


void vMachine::initSettings()
	// override weakly bound method called from Grbl.cpp
{
	// v_info("vMachine::initSettings()");
	// called from static ctor - bad idea to use Serial output

	v_machine_width				= VMACHINE_DEFAULT_MACHINE_WIDTH;
	v_machine_height			= VMACHINE_DEFAULT_MACHINE_HEIGHT;
	v_dist_between_motors		= VMACHINE_DEFAULT_DIST_BETWEEN_MOTORS;
	v_motor_offset_y			= VMACHINE_DEFAULT_MOTOR_OFFSET_Y;
	v_sprocket_radius			= VMACHINE_DEFAULT_SPROCKET_RADIUS;
	v_chain_left_tolerance		= VMACHINE_DEFAULT_CHAIN_LEFT_TOLERANCE;
	v_chain_right_tolerance		= VMACHINE_DEFAULT_CHAIN_RIGHT_TOLERANCE;
	v_sag_correction			= VMACHINE_DEFAULT_CHAIN_SAG_CORRECTION;
	v_sled_radius				= VMACHINE_DEFAULT_SLED_RADIUS;
	v_forward_guess_tolerance	= VMACHINE_DEFAULT_FORWARD_GUESS_TOLERANCE;
	v_max_forward_guesses		= VMACHINE_DEFAULT_MAX_FORWARD_GUESSES;
	v_guess_max_chain_length	= VPLOTTEER_DEFAULT_GUESS_MAX_CHAIN_LENGTH;
	v_zero_length				= VMACHINE_DEFAULT_ZERO_LENGTH;
	v_left_zero_offset			= VMACHINE_DEFAULT_LEFT_ZERO_OFFSET;
	v_right_zero_offset			= VMACHINE_DEFAULT_RIGHT_ZERO_OFFSET;
	v_safe_area_offset			= VMACHINE_DEFAULT_SAFE_AREA_OFFSET;
	v_zaxis_safe_position		= VMACHINE_DEFAULT_Z_AXIS_SAFE_POSITION;
	v_line_segment_length		= VMACHINE_DEFAULT_LINE_SEGMENT_LENGTH;

	// called from static ctor - bad idea to use Serial output
	// 	#if 0
	//  	v_debug("v_machine_width         = %-0.3f",getMachineWidth());
	//  	v_debug("v_machine_height        = %-0.3f",getMachineHeight());
	//  	v_debug("v_dist_between_motors   = %-0.3f",getDistBetweenMotors());
	//  	v_debug("v_motor_offset_x        = %-0.3f",getMotorOffsetX());
	//  	v_debug("v_motor_offset_y        = %-0.3f",getMotorOffsetY());
	//  	v_debug("v_sprocket_radius       = %-0.3f",getSprocketRadius());
	//  	v_debug("v_chain_left_tolerance  = %-0.3f",getChainLeftTolerance());
	//  	v_debug("v_chain_right_tolerance = %-0.3f",getChainRightTolerance());
	//  	v_debug("v_sag_correction        = %-0.3f",getSagCorrection());
	//  	v_debug("v_sled_radius           = %-0.3f",getSledRadius());
	//  	v_debug("v_forward_guess_tol     = %-0.3f",getForwardGuessTolerance());
	//  	v_debug("v_max_forward_guesses   = %d",	getMaxForwardGuesses());
	//  	v_debug("v_guess_max_chain_len   = %-0.3f",getGuessMaxChainLength());
	//  	v_debug("v_zero_length           = %-0.3f",getZeroLength());
	//  	v_debug("v_left_zero_offset      = %-0.3f",getLeftZeroOffset());
	//  	v_debug("v_right_zero_offset     = %-0.3f",getRightZeroOffset());
	//  	v_debug("v_safe_area_offset      = %-0.3f",getSafeAreaOffset());
	//  	v_debug("v_zaxis_safe_position   = %-0.3f",getZAxisSafePosition());
	//  	v_debug("v_line_segment_length   = %-0.3f",getLineSegmentLength());
	// #endif
}

#endif
