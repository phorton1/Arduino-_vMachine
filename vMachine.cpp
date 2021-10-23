// vMachine.cpp

#include "vMachine.h"
#include <FluidDebug.h>     	// FluidNC_extensions
#include <YamlOverrides.h>		// FluidNC_extensions
	// YamlOverrides.h can be commented out to
	// avoid linking in the WEAK_LINK overrides

#define DEBUG_YAML 						0
#define CONFIGURE_WIFI_EXPERIMENT   	0


#if CONFIGURE_WIFI_EXPERIMENT
	#include <Machine/Communications.h>
	#include <Machine/WifiAPConfig.h>
	#include <Machine/WifiSTAConfig.h>
#endif



//----------------------------------------------
// Note
//----------------------------------------------
// Set G55 to 12x9" paper and go to upper right corner
//
// 		g10 L2 p2 x47.6 y35.7 z-90
// 		g0 x304.8 y228.6
//
// Set G56 to 8x11.5" paper
//
// 		g10 L2 p3 x60.3 y42.05 z-90
//		G0 x279.4 y215.9

//----------------------------------------
// Default vMachine Machine Definition
//---------------------------------------
// The "machine area" is 400x300mm == 15.748" x 11.811"

#define VMACHINE_DEFAULT_MACHINE_WIDTH          400  // mm, $vMachine/machineWidth
#define VMACHINE_DEFAULT_MACHINE_HEIGHT         300  // mm, $vMachine/machineHeight;

// the work area is 4" and a distance below the center line between motors

#define VMACHINE_DEFAULT_DIST_BETWEEN_MOTORS    (28.75 * 25.4)       // 730.25 mm, $vMachine/distBetweenMotors
#define VMACHINE_DEFAULT_MOTOR_OFFSET_Y         ((4 * 25.4) + 24.3)  // 125.9 mm,  $vMachine/motorOffsetY;

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

#define VMACHINE_DEFAULT_ZERO_LENGTH         209.849     // mm    $vMachine/zeroLength
	// 209.849 = sqrt(129.5^2 + 165.125^2)
	//
	// This number is the length of the hypotenuse from the center of the  motor
	// to the upper left/right corner of the work area, which is given by the formula:
	//
	//     sqrt(MOTOR_OFFSET_Y^2+ MOTOR_OFFSET_X^2)
	//
	// where MOTOR_X_OFFSET = (DIST_BETWEEN_MOTORS - VMACHINE_DEFAULT_MACHINE_WIDTH)/2,
	// or (730.25-400)/2 = 330.25/2 = 165.125 in our case, so sqrt(129.5^2 + 165.125)

#define VMACHINE_DEFAULT_LEFT_ZERO_OFFSET    -10.000     // mm, $vMachine/leftZeroOffsest  (-10 mm == -500 steps)
#define VMACHINE_DEFAULT_RIGHT_ZERO_OFFSET   -10.000     // mm, $vMachine/rightZeroOffsest
	// We place the white stripes 10mm closer to the sled, given by these constants,
	// so that when the sensors are triggered we are at VMACHINE_DEFAULT_ZERO_LENGTH - 10mm,
	// thus the we can reach the work area without triggering the sensors again, since it
	// will be 10,10 mm "in" from the trigger point.
	//
	// Thus the white stripes are painted just about 200mm (209.849-10 =~ 200) from the
	// center of the sled.   The sled attachment points are 40mm from the center, so we
	// place the leading edge of the white strips 160mm from the sled attachment points.
	//
	// This gives us a little extra room to work with.

#define VMACHINE_DEFAULT_SAFE_AREA_OFFSET       5.0     // mm, $ESP951, $vMachine/safeAreaOffset
	// we define a safe area around the work area to constrain movements
	// and for soft limit checks.  This should all be within the augmented
	// work area as defined by the Zero Offset settings above.

#define VMACHINE_DEFAULT_Z_AXIS_SAFE_POSITION   -70 	// mm, $ESP953, $vMachine/zAxisHomeSafe
	// The absolute positions for the Z-Axis are 0==up in the air to -90==touching,
	// hence we define "zero" as -90 for our paper settings above.  The safe travel
	// in ABSOLUTE settings is then something like -70
#define VMACHINE_DEFAULT_LINE_SEGMENT_LENGTH    0.5	// mm, $ESP955, $vMachine/lineSegLength
	// length of subsegments done by cartesian_to_motors()





void vMachine::afterParse() // override
{
	#if DEBUG_YAML
		g_debug("---> vMachine::afterParse() called");
	#endif

	#if CONFIGURE_WIFI_EXPERIMENT
		if (!_comms)
		{
			log_info("Comms: vMachine creating _comms");
			_comms = new Machine::Communications();
		}
		#ifdef ENABLE_WIFI
			if (!_comms->_apConfig)
			{
				log_info("Comms: vMachine creating _comms->_apConfig");
				_comms->_apConfig = new Machine::WifiAPConfig();
			}
			if (!_comms->_staConfig)
			{
				log_info("Comms: vMachine creating _comms->_apConfig");
				_comms->_staConfig = new Machine::WifiSTAConfig();
				// _comms->_staConfig->_ssid = "THX36";
			}
		#endif
	#endif

	// At this point the tree is fleshed out with items from the yaml file, but not
	// with the entire tree as *determined* by MachineConfig::afterParse().

	Machine::MachineConfig::afterParse();

	// After this config->group(AfterParse) will be called at which time
	// NVS ovrrides will be applied
}




void vMachine::group(Configuration::HandlerBase& handler) // override
{
	// handler.section("vMachine", NULL);

	#if DEBUG_YAML
		const char *htype = "UNKNOWN";
		switch (handler.handlerType())
		{
			case Configuration::HandlerType::Parser		:  htype="Parser";  break;
			case Configuration::HandlerType::AfterParse	:  htype="AfterParse";  break;
			case Configuration::HandlerType::Runtime	:  htype="Runtime";  break;
			case Configuration::HandlerType::Generator	:  htype="Generator";  break;
			case Configuration::HandlerType::Validator	:  htype="Validator";  break;
		}
		g_debug("---> vMachine::group(handler=%s) called",htype);
	#endif


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
	// called from my derived MachineConfig static ctor
{
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

}
