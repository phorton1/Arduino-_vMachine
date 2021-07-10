// vMachine - my minimalist port of maslow kinematics to the GRBL_ESP32 code base

// prh - notes on bringing up to YAML_SETTINGS branch
//
// When you first boot GRBL_ESP32 there is no config.yaml on the SPIFFS
// I had to modify config.yaml to set the STA/SSID though I can set the
// password via the serial port.

#include <Arduino.h>

#include "vMachine.h"
#include "vKinematics.h"
#include "vSensor.h"

#include <SD.h>
#include <SPI.h>

#include <Grbl.h>
#include <Config.h>
#include <System.h>
#include <SDCard.h>
#include <Report.h>
#include <Planner.h>
#include <Protocol.h>
#include <MotionControl.h>
#include <GLimits.h>


#define STEPS_PER_MM(axis)		(config->_axes->_axis[axis]->_stepsPerMm)

vMachine v_machine;


//-------------------------------------------
// COORDINATE SYSTEMS
//-------------------------------------------
// sys_position = an array of int32s representing the stepper motor position in steps
// motor position = floats in mm's == sys_position / axis_settings->steps_per_mm
// machine position = floats called "mpos" which is the cartesian coordinate within the work area,
// work position =  grbl floats (wpos) that lets you set 0,0 relative to the machine area.
//
// The "motor position" and sys_position are representative of the chain (cable) lengths.
//
// The system does not maintain the motor, machine, or work positions.
// It only maintains the int32 sys_positions.
//
// If GRBL needs to do something in the machine/work coordinate systems (i.e. move to the
// next point in some gcode) it calls cartesianToMotors() to get the "motor positions"
// for that coordinate, which it then turns back into sys_position steps for planning
// and actual movement.
//
// For reporting, sys_position ==> motor position is turned into the "machine position" by
// calling motors_to_cartesian() (forward kinematics), which in turn, does
// iterative calls to the reverse kinematics to find a "machine_position" that matches
// the motor position.  These forward kinematics are normally not needed to run GRBL,
// they are used only for reporting.

//-------------------------------------------
// HOMING AND ZERO STOPS
//-------------------------------------------
// #define ZERO_LENGTH  223.959     // mm. just about 224
//    sqrt(MOTOR_OFFSET_Y*MOTOR_OFFSET_Y + MOTOR_OFFSET_X*MOTOR_OFFSET_Y))
//         the length of the hypotenuse of the right trangle
//         from motor center points to the top left/right corners
//         of the machine area
//
// The forward edge of the "zero stop" white lines are painted on the
// belt 21.4 mm from the center of the sled.  This is 10mm "in" from the pully
// when the sled is at zero cable length. The stop will get not get
// triggered util the cable length is logically -10 (minus ten)
// millimeters (AFTER it passes zero) while reeling in the belt
// during homing.
//
// #define LEFT_ZERO_OFFSET    -10.000     // mm == -500 steps
// #define RIGHT_ZERO_OFFSET   -10.000
// 	// candidates for preferences
//
// These allow for an approximately 7mm horizontal and vertial
// "safe" zone around the machine area that the system can still
// navigate in and ensures that the end stops are not triggered
// within the work area.

// TBD: The "max" stops are also painted in such a way as to allow a bit
// more than the maximum length, but it is the zero stops that are
// important in homing and all subsquent calculations and the overall
// accuracy of the machine.

// We cannot o use the $XY/Home/MPos existing config EEPROM space
// to store these as preferences as that has a different semantic.

// Modifying the above constants is possible to account for the fact
// that the sensors may trigger sooner or later (not exactly on the edge)
// by doing a Homing cycle and, after the "pull off" (15 mm in our case)
// one could measure the distance from the white line to TDC of the pulley
// and adjusting the constants until it is actually 15mm from TDC after
// the pulloff.


// debugging the inverse kinematics may introduce
// timing problems that can crash the machine ...

#define DEBUG_VHOME   	2		// can be up to 2
#define DEBUG_VREVERSE  0
#define DEBUG_VFORWARD  0		// can be up to 2


Kinematics	kinematics;
bool in_homing = false;


static inline unsigned long mulRound(float *vals, int axis)
	// change "motor position" (chain lengths) mm's to steps
{
	float mm = vals[axis];
	float steps_per_mm = STEPS_PER_MM(axis);
	float rslt = mm * steps_per_mm + 0.5;
	unsigned long retval = rslt;
	return retval;
}


void debug_start_sdcard()
{
	#define INIT_SDCARD_AT_STARTUP   1
		// This is just here for serial initialization debugging.
		// Otherwise the "normal" grbl_esp32 code does not initialize it
		// until it is accessed via a [ESPxxx] command from the WebUI or
		// via [ESP210] or [ESP420].  I modified the webUI to start up
		// with the file list initialized and my UI code checks for a
		// card insertion every 2 seconds if one is not present.
		// REQUIRES 1K PULLUP RESISTOR ON MISO

	#if INIT_SDCARD_AT_STARTUP

		// vTaskDelay(200);
			// I still had to add a wonky delay here, apparently
			// to give time for the pullup resistor to work ?!?

		// In bringing vMachine up on the Yaml_Settings branch. I discovered
		// that they have overriden the core system pinMode, digitalWrite, and digitalRead
		// methods, and that their overrides don't generally work with my code or other libraries.
		// Particularly with the SDCard that DID NOT WORK until I diddled their code
		// The "fix" is a commented out section in (their) PinMapper.cpp to get rid of their
		// override methods so the core methods get called normally.

		#if 0
			// possibly useful when TFT is hooked up
			// Serial.println("setting pins high");
			usePinMode(GPIO_NUM_5,OUTPUT);         // TOUCH_CS
			usePinMode(GPIO_NUM_17,OUTPUT);        // TFT_CS
			// usePinMode(GPIO_NUM_19,INPUT_PULLUP);  // MISO
			useDigitalWrite(GPIO_NUM_5,1);
			useDigitalWrite(GPIO_NUM_17,1);
			vTaskDelay(250);
		#endif

		#if 0

			// Call SD.begin() directly.
			// GRBL_SDCARD_CS must be correctly set

			info_serial("starting SD Card on pin(%d)",GRBL_SDCARD_CS);
			if (!SD.begin(GRBL_SDCARD_CS))
			{
				info_serial("SD.begin() failed");
			}
			// else
			{

		#else	// relies on vMachine.yaml spi setup

			// The GRBL way of initializing the SD Card is to call
			// SDState state = get_sd_state(true) and then check for
			// SDState::NotPresent.  Otherwise it's there and if not
			// SDState::Idle it is considered busy. They then call
			// SD.end() after each transaction.

			info_serial("starting sdCard on yaml pin(%s)",config->_spi->_cs.name());
			SDCard::State state = config->_sdCard->get_state(true);
			// info_debug("machine_init() get_sd_state() returned %d",state);
			if (state == SDCard::State::NotPresent)
			{
				info_serial("SD Card Not Present");
			}
			else
			{
				if (state != SDCard::State::Idle)
					info_serial("SD Card BUSY");

		#endif

			uint8_t cardType = SD.cardType();
			info_serial("SD Card Type: %s",
				cardType == CARD_NONE ? "NONE" :
				cardType == CARD_MMC  ? "MMC" :
				cardType == CARD_SD   ? "SDSC" :
				cardType == CARD_SDHC ? "SDHC" :
				"UNKNOWN");
			uint64_t cardSize = SD.cardSize() / (1024 * 1024);
			info_serial("SD Card Size: %lluMB", cardSize);
			info_serial("Total space:  %lluMB", (SD.totalBytes()+1024*1024-1) / (1024 * 1024));
			info_serial("Used space:   %lluMB", (SD.usedBytes()+1024*1024-1) / (1024 * 1024));

			// prh - I am NOT going to close the SD Card here so as to test
			// the uiDirectory window, but something needs to be done about
			// SDCard::State() and it's usage in Grbl for me to proceed in
			// a sane manner.

			// SD.end();	// as per the GRBL way
		}
	#endif
}


void machine_init()
	// override weakly bound method called from Grbl.cpp
{
	info_serial("vMachine.cpp::machine_init() on core %d at priority %d",xPortGetCoreID(),uxTaskPriorityGet(NULL));

	// v_machine.initSettings();

	// my predilection

	debug_start_sdcard();

	// start my limit switch task
	// this really needs to be bound into the mainline grbl code

	xTaskCreate(vSensorTask,
		"vSensorTask",
		4096,
		NULL,
		1,  	// priority
		NULL);
	delay(200);
		// need to figure out why info_send() is not atoomic

	// Continuing my normal startup
	// initialize the kinematics engine

	kinematics.init();

	//----------------------------------------------------
	// set system at imaginary position of 200,150,0 to start
	//----------------------------------------------------
	// These are just imaginary values until we are homed.

	memset(sys_position,0,MAX_N_AXIS * sizeof(unsigned long));
		// trying to find bug

	float position[MAX_N_AXIS];
	kinematics.inverse(
		v_machine.getMachineWidth()/2,		// 200
		v_machine.getMachineHeight()/2,		// 150
		&position[X_AXIS],
		&position[Y_AXIS]);
	position[Z_AXIS] = v_machine.getZAxisSafePosition(); // is 20 was 0;
	sys_position[X_AXIS] = mulRound(position,X_AXIS);
	sys_position[Y_AXIS] = mulRound(position,Y_AXIS);
	sys_position[Z_AXIS] = mulRound(position,Z_AXIS);

	debug_serial("init sys_position(%d,%d,%d) mms(%f,%f,%f)",
		sys_position[X_AXIS],
		sys_position[Y_AXIS],
		sys_position[Z_AXIS],
		(float)sys_position[X_AXIS] / STEPS_PER_MM(X_AXIS),
		(float)sys_position[Y_AXIS] / STEPS_PER_MM(Y_AXIS),
		(float)sys_position[Z_AXIS] / STEPS_PER_MM(Z_AXIS));

	// magic_after_diddling_sys_position()

	// plan_reset();
	// gc_init();
	// gc_sync_position();
	// plan_sync_position();

	info_serial("vMachine.cpp::machine_init() finished");

}


//-------------------------------------------------------
// cartesian_to_motors - override weekly bound method
//-------------------------------------------------------
// the grbl method params are ineptly named and parameterized
// you have cartesian_to_motors(target,position)
// where input is first (target) and the "cartesian" and
// position is "motors", second.
//    and position does not need to be 'set' in retval
//    since we call mc_line()
// and then motors_to_cartesian(cartesian,motors)
//    where the return parameter is first, and input is second
// none of these is to be confused with sys_position, which is
// the unsigned long STEP_COUNT for the stepper motors which is really
// the underlying coordinate system ...

bool cartesian_to_motors(float* target, plan_line_data_t* pl_data, float* position)
	// PRH THE FEED RATES NEED TO BE FIGURED OUT
	// pl_data->feed_rate = 0.5 ...
	// something about the sqrt() of the distance (per segment ?!?)
{
	float new_position[MAX_N_AXIS];
	// memset(new_position,0,MAX_N_AXIS * sizeof(float));
		// Just in case ... we are passing a float array to lower level code
		// I'm not sure they don't call some method that touches the unused axes,
		// and/or whether they need to be zero.

	#if DEBUG_VREVERSE
		debug_serial("cartesian_to_motors(%f,%f,%f) old(%f,%f,%f) feed=%f",
			target[X_AXIS],
			target[Y_AXIS],
			target[Z_AXIS],
			position[X_AXIS],
			position[Y_AXIS],
			position[Z_AXIS],
			pl_data->feed_rate);
	#endif
	#if DEBUG_VREVERSE>1
		debug_serial("   ctm starting sys_lengths(%f,%f,%f) sys_pos(%d,%d,%d)",
			sys_position[X_AXIS] / STEPS_PER_MM(X_AXIS),
			sys_position[Y_AXIS] / STEPS_PER_MM(Y_AXIS),
			sys_position[Z_AXIS] / STEPS_PER_MM(Z_AXIS),
			sys_position[X_AXIS],
			sys_position[Y_AXIS],
			sys_position[Z_AXIS]);
	#endif

	#ifdef RENDER_TOOL_MOVEMENT
		extern void enqueueRenderMove(bool is_move, float *position, float *target);
		enqueueRenderMove(pl_data->motion.rapidMotion,position,target);
	#endif

	// constrain the X,Y target to the safe area

	float safe_offset = v_machine.getSafeAreaOffset();
	float machine_width = v_machine.getMachineWidth();
	float machine_height = v_machine.getMachineHeight();

	target[X_AXIS] =
		(target[X_AXIS] < -safe_offset) ? -safe_offset :
		(target[X_AXIS] > machine_width+safe_offset) ? machine_width+safe_offset :
		target[X_AXIS];
	target[Y_AXIS] =
		(target[Y_AXIS] < -safe_offset) ? -safe_offset :
		(target[Y_AXIS] > machine_height+safe_offset) ? machine_height+safe_offset :
		target[Y_AXIS];

	// prepare to do segments

	uint32_t num_segs = 1;
	float xdist = target[X_AXIS] - position[X_AXIS];
	float ydist = target[Y_AXIS] - position[Y_AXIS];
	float zdist = target[Z_AXIS] - position[Z_AXIS];

	// "rapid motion is not used to draw" (from polar_cooaster.cpp)
	// if neither X or Y changed, or if it's a system motion we do it in one segment
	// we round up to ensure at least one segment

	// pl_data->feed_rate = 2000;

	if (0 && !pl_data->motion.rapidMotion && (xdist!=0 || ydist!=0))
	{
		float dist = sqrt(xdist*xdist + ydist*ydist + zdist*zdist);
		float line_seg_len = v_machine.getLineSegmentLength();
		float f_num_segs = (dist + (line_seg_len/2))/line_seg_len;
		num_segs = f_num_segs;
		if (!num_segs) num_segs = 1;
	}

	//--------------------
	// do loop of segments
	//--------------------

	float xinc = xdist/num_segs;
	float yinc = ydist/num_segs;
	float zinc = zdist/num_segs;

	#if DEBUG_VREVERSE
		debug_serial("   ctm doing %d segments with incs(%f,%f,%f)",
			num_segs,
			xinc,
			yinc,
			zinc);
	#endif

	for (uint32_t seg_num=0; seg_num<num_segs; seg_num++)
	{
		position[X_AXIS] += xinc;
		position[Y_AXIS] += yinc;
		position[Z_AXIS] += zinc;

		kinematics.inverse(
			position[X_AXIS],
			position[Y_AXIS],
			&new_position[X_AXIS],
			&new_position[Y_AXIS]);
		new_position[Z_AXIS] = position[Z_AXIS];

		#if DEBUG_VREVERSE>1
			debug_serial("   ctm seg(%d) to(%f,%f,%f) lengths(%f,%f,%f)",
				seg_num,
				position[X_AXIS],
				position[Y_AXIS],
				position[Z_AXIS],
				new_position[X_AXIS],
				new_position[Y_AXIS],
				new_position[Z_AXIS]);
		#endif

		if (!mc_line(new_position, pl_data))
			return false;	// as per polar_coaster.cpp

	}	// for each segment


	#if DEBUG_VREVERSE>1
		debug_serial("cartesian_to_motors final lengths(%f,%f,%f) steps(%d,%d,%d)",
			new_position[X_AXIS],
			new_position[Y_AXIS],
			new_position[Z_AXIS],
			mulRound(new_position,X_AXIS),
			mulRound(new_position,Y_AXIS),
			mulRound(new_position,Z_AXIS));
	#endif

	return true;
}


//--------------------------------------------------------
// motors_to_cartesian - override weekly bound method
//--------------------------------------------------------

void motors_to_cartesian(float* cartesian, float* motors, int n_axis)
{
	#if DEBUG_VFORWARD
		if (prh_debug_forward)
			debug_serial("motors_to_cartesian(%f,%f,%f) old(%f,%f,%f)",
				motors[X_AXIS],
				motors[Y_AXIS],
				motors[Z_AXIS],
				cartesian[X_AXIS],
				cartesian[Y_AXIS],
				cartesian[Z_AXIS]);
	#endif

	kinematics.forward(
		motors[X_AXIS],
		motors[Y_AXIS],
		&cartesian[X_AXIS],
		&cartesian[Y_AXIS]);		// useless yGuess
	cartesian[Z_AXIS] = motors[Z_AXIS];

	#if DEBUG_VFORWARD
		if (prh_debug_forward)
			debug_serial("motors_to_cartesian(%f,%f,%f) final(%f,%f,%f)",
				motors[X_AXIS],
				motors[Y_AXIS],
				motors[Z_AXIS],
				cartesian[X_AXIS],
				cartesian[Y_AXIS],
				cartesian[Z_AXIS]);
	#endif
}


//------------------------------------------------------------------
// Homing
//------------------------------------------------------------------
// Unfortunately we cannot easily use the native GRBL homing code because
// of our coupled axis ... we have to let out one to home the other ....
// AND we have to do two separate homes ...
//
// Therefore we had to duplicate everything from limits_go_home()
// and learn a lot to write this method.
//
// The vMachine homes on 2 axes using optical sensors and analogRead()
//
// We describe the homing on the X axis. The Y axis is the opposite.
//
// As it homes on the X axis, it must let out the cable on the Y axis
// at the same time.  So we move the X axis in a negative direction and
// the Y axis in a positive one until a stop is hit. These initial moves
// are done at the HOMING/SEEK rate $25 (3000 mm per minute)
//
// If the Y axis stop is hit, we stop moving the Y axis but keep
// moving the X axis until it's stop is hit.
//
// When the X axis stop is hit, we stop moving both axes do a bounce
// off the X axis, and then use the finer HOMING/FEED rate $24 to re-identify
// the zero position (200 mm per minute)
//
// Note that "cycle_stop" is a global boolean that is set when the
// planner runs out of lines, but that you are responsible for clearing
// it as necessary.
//
// "cycle_stop" changedd to "rtCycleStop" in yaml branch


float *my_get_mpos()
	// the problem with system_get_mpos() is that it turns around and
	// calls the forward kinematics, which has nothing to do with
	// this level of interaction (just stepper motors and steps, really)
	// so this nearly identical method JUST divides positions by steps/mm
	// to get the mm as floats
	//
	// duplicate, basically of system_convert_axis_steps_to_mpos()
	// except that method only does one axis and does not provide
	// a satic working variable like system_get_mpos()
{
	static float motors[MAX_N_AXIS];
	for (int idx = 0; idx < MAX_N_AXIS; idx++)
	{
		motors[idx] = (float)sys_position[idx] / STEPS_PER_MM(idx);
	}
	return motors;
}


void debug_position(const char *what)
{
	debug_serial("      %-9s sys_pos(%d,%d,%d) sys_lengths(%f,%f,%f)",
		what,
		sys_position[X_AXIS],
		sys_position[Y_AXIS],
		sys_position[Z_AXIS],
		sys_position[X_AXIS] / STEPS_PER_MM(X_AXIS),
		sys_position[Y_AXIS] / STEPS_PER_MM(Y_AXIS),
		sys_position[Z_AXIS] / STEPS_PER_MM(Z_AXIS));
}


bool vHome(int axis)
	// this is unduly complicated due to the prodiguous
	//     use of global variables, undocumented assumptions about state,
	//     and the generally horrible encapsulation of functionality in
	//     the GRBL code.
	// all I wanted to do was move the steppers and check for the limit
	//     switches (or end of a travel length)
	// but you have to go through the "planner", and it makes assumptions
	//     about state, uses global variables, and has a byzantine level
	//     of complexity to accomlish this simple task.
{
	#if DEBUG_VHOME > 1
		debug_serial("vHome(%s)",axis?"Y":"X");
		debug_position("start");
	#endif

	int other_axis = axis == X_AXIS ? Y_AXIS : X_AXIS;
	int bit_axis = bit(axis);
	int bit_other_axis = bit(other_axis);

	#define EXTRA_TRAVEL_RATIO  1.1

	// setup empty plan data

	plan_line_data_t  plan_data;
	plan_line_data_t* pl_data = &plan_data;
	memset(pl_data, 0, sizeof(plan_line_data_t));
	pl_data->motion                = {};
	pl_data->motion.systemMotion   = 1;
	pl_data->motion.noFeedOverride = 1;
	#ifdef USE_LINE_NUMBERS
		pl_data->line_number = HOMING_CYCLE_LINE_NUMBER;
	#endif

	// get settings into variables

	// YAML_SETTINGS branch change

	// Well, that answers one question at least.
	//
	// There were, and are no "standards" as to what represents the grbl settings.
	// In the main branch there was a single homing_seek rate.
	// In the Yaml_Settings branch they are per axis.
	//
	// There is no specification document that lists the change, esp inasmuch
	// as there was no specificiation document that listed the main branch grbl_esp32 settings,
	// much like there is no specification document for the Protocol, or, in fact
	// what constitutes "GCode" as far as "GRBL" itself (or the esp32 branch),
	// or for that matter, the design or architecture of either repository, much
	// less a meaningful roadmap for the envisioned life cycle of the codebase.

	auto the_axis = config->_axes->_axis[X_AXIS]; 	// we use the X cannonically
	auto homing = the_axis->_homing;

	float fast_rate = homing->_seekRate;		// DEFAULT_HOMING_SEEK_RATE;
	float slow_rate = homing->_feedRate;		// DEFAULT_HOMING_FEED_RATE;
	float pull_off  = homing->_pulloff;			// DEFAULT_HOMING_PULLOFF;
	float other_axis_travel = EXTRA_TRAVEL_RATIO * the_axis->_maxTravel;	// out
	float axis_travel = -other_axis_travel;								// in

	// fawk - need to clear cycle stop and set the (next) axis sys_position to zero

	rtCycleStop = false;
	sys_position[axis] = 0;

	// setup for intial 'seek' move
	// where we wind in the axis and wind out the other_axis
	// note that the other_axis move has been made relative with +=

	float *target = my_get_mpos();				// system_get_mpos();
	target[axis] = axis_travel;					// this axis is at "0" (absoute move)
	target[other_axis] += other_axis_travel;	// relative move

	// mo fawk - sys.homing_axis_lock bits MUST be set for each axis
	// for the motors to work ...

	sys.homing_axis_lock = bit_axis | bit_other_axis;	// axislock;
		// a mask of the 'active' axes, but it sounds like the opposite

	// weird line that I did not include from limits_go_home() code:
	// homing_rate *= sqrt(n_active_axis);  // [sqrt(number of active axis)] Adjust so individual axes all move at homing rate
	// I still have to figure out feed rates

	// so there is only ONE feed rate per "move" ???
	// and it somehow relates to different stepper speeds becaue of the different distances traveled in the "move"
	// which happens, I presume over the "distance" of the move ... need to figure out feed rates

	// start initial 'seek' move

	pl_data->feed_rate = fast_rate;   		// Set current homing rate.
	plan_buffer_line(target, pl_data);  		// Bypass mc_line(). Directly plan homing motion.
	sys.step_control = {};
	sys.step_control.executeSysMotion = true;  	// Set to execute homing motion and clear existing flags.
	Stepper::prep_buffer();                         // Prep and fill segment buffer from newly planned block.
	Stepper::wake_up();    				// Initiate motion

	// check for limit hit(s)
	//
	// there are 4 parts to a successful home ...
	//    (0) we find the 'home' limit switch at the fast rate
	//    (1) we "pull off" (at the fast rate)
	//    (2) we find the home at the slow rate
	//    (3) we pull off (at the fast rate)
	//    (4) we are done
	// For fun, we repeat 2 and 3 an extra time if NUM_STEPS==6

	#define NUM_PHASES 6 		// or 4
		// candidate for preference

	int home_phase = 0;
	int home_error = 0;

	#if DEBUG_VHOME > 1
		debug_serial("   vHome phase(%d)  rate(%f)  axis_travel(%f) other_axis_travel(%f)",
			home_phase,
			pl_data->feed_rate,
			axis_travel,
			other_axis_travel);
	#endif

	while (!home_error && home_phase<NUM_PHASES)
	{
		// not sure why this is needed

		Stepper::prep_buffer();

		// move volatile state into a local

		int limit_state = getVSensorState();	// limits_get_state();

		// check rtReset and rtCycleStop for outer level errors

		if (rtReset)
		{
			#if DEBUG_VHOME
				debug_serial("   vHome rtReset==HomingFailReset 6");
			#endif
			sys_rt_exec_alarm = ExecAlarm::HomingFailReset;		// 6
			home_error = 1;
		}
		else if (rtCycleStop)
		{
			if (!(home_phase & 1))
			{
				#if DEBUG_VHOME
					debug_serial("   vHome rtCycleStop==HomingFailApproach 9");
				#endif
				sys_rt_exec_alarm = ExecAlarm::HomingFailApproach;   // 9
				home_error = 1;
			}
			else if (limit_state & bit_axis)
			{
				#if DEBUG_VHOME
					debug_serial("   vHome rtCycleStop==HomingFailPulloff 8");
				#endif
				sys_rt_exec_alarm = ExecAlarm::HomingFailPulloff;	// 8
				home_error = 1;
			}
		}

		if (!home_error)
		{
			// if either axis hits a stop,
			// stop the other axis and continue with the main one

			if (limit_state)
				sys.homing_axis_lock &= ~bit_other_axis;

			// if the main axis hits a stop on 0,2 we move to next state
			// if the main axis hits a stop on 1,3, it's an error
			// if we get a cycle stop on 1,3, we move to next state
			// next phase and home_error are mutually exclusive in this block

			bool next_phase = false;
			if (limit_state & bit_axis)
			{
				if (!(home_phase & 1))	// phase 1,3 etc
				{
					next_phase = true;
				}
			}
			else if (rtCycleStop)		// 0 and 2 were caught above
			{
				next_phase = true;
			}

			// move to the next phase

			if (next_phase)
			{
				home_phase++;
				Stepper::reset();  	    	// Immediately force kill steppers and reset step segment buffer.
				delay_ms(500);  		// Delay to allow transient dynamics to dissipate.
					// vTaskDelay(100)
				other_axis_travel = 0.0;

				if (home_phase & 1)
				{
					axis_travel = pull_off;
					pl_data->feed_rate = fast_rate;

					// if we hit the limit letting the other axis out,
					// for phase1 we pull IT off too.

					if (home_phase == 1 && limit_state & bit_other_axis)
						other_axis_travel = pull_off;
				}
				else if (home_phase < NUM_PHASES)
				{
					axis_travel = -pull_off * EXTRA_TRAVEL_RATIO;
					pl_data->feed_rate = slow_rate;
				}
				else
				{
					next_phase = false;	// we're done
				}

				if (next_phase)		// if still going
				{
					#if DEBUG_VHOME > 1
						debug_serial("   vHome next_phase(%d)  rate(%f)  axis_travel(%f)",
							home_phase,
							pl_data->feed_rate,
							axis_travel);
						debug_position("next");
					#endif

					// the target must be reset from the sys_position each new phase
					//     so that other axes are already where they will be targeted
					// and the system_position for the current axis must be zeroed
					// 	   or else the moves are considered to be relative to whatever tf
					//     the system position happens to be after the previous phase
					// you have to know, and manage these global variables and methods
					//     to implement a user_defined_homing() method.

					rtCycleStop = false;
					sys_position[axis] = 0;
					target = my_get_mpos();						// system_get_mpos();
					target[axis] = axis_travel;					// absolute move
					target[other_axis] += other_axis_travel; 	// relative move to come off of stop

					// then just 5 procedure calls, all magic, sheesh, and it works

					plan_buffer_line(target, pl_data);  		// Bypass mc_line(). Directly plan homing motion.
					sys.step_control = {};
					sys.step_control.executeSysMotion = true;  	// Set to execute homing motion and clear existing flags.
					Stepper::prep_buffer();                          	// Prep and fill segment buffer from newly planned block.
					Stepper::wake_up();

				}	// next_phase <= 3
			}	// next_phase
		}	// no exec our outer cycle errror
	}	// while !home_error && home_phase<4

	if (home_error)
	{
		config->_axes->set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), false);
		// motors_set_homing_mode(cycle_mask, false);
		// tell motors homing is done

		error_all("Homing fail");
		mc_reset();
		protocol_execute_realtime();
		return false;
	}
	else
	{
		#if DEBUG_VHOME > 1
			float *dbg_mms = my_get_mpos();
			debug_serial("   vHome(%s) finished",axis?"Y":"X");
			debug_position("end");
		#endif

		// so, at this point, this axis is now PULL_OFF mm from the limit mark

		return true;
	}

}	// vHome() one axis




bool user_defined_homing(uint8_t cycle_mask)
	// override weekly bound method.
	//
	// Homing Cycles are not used by my code
	// 		$Homing/Cycle0=Z
	// 		$Homing/Cycle1=XY
	//
	// $H comes in as cycle_mask=0
	// $HX,%HY, and $HZ with their bits set.
	// We only do $H or $HZ
	//
	// The primary purpose of this method is to establish the physical location of
	//     the sled by determining the lengths of the X and Y belts at an arbitray
	//     position.
	// It does not, per se, "go" to the "home position" which is the default
	//     $XYZ/Home/Mpos of 0,0,0
	// Once "homed", then position 0,0,0 is the bottom left corner of the work area,
	//     with the pen touching the paper and a command to go there will take
	//     the sled there.
	//
	// The Z Axis is assumed to be physically calibrated by the servo position.
	// The first thing this method does is make sure the Z axis is pulled up
	//    out of the way by positioning it at the Z_AXIS_SAFE_POSITION (20)
	//
	// It then reels in the Y belt until the stop is hit, figures out where
	// that is, and reels in the X belt until the stop is hit and backs off
	// the pull of amount.  At that point we know the chain lengths and
	// can establish our position.
	//
	// TODO? - after establishing position, move to known location,
	//    i.e. 0,299,20 top right corner.
{
	in_homing = 1;

	#if DEBUG_VHOME
		float *dbg_mms = my_get_mpos();
		debug_serial("vMachine-home(0x%02x)",cycle_mask);
		debug_serial(" STEPS_PER_MM(%f,%f,%f)",
			STEPS_PER_MM(X_AXIS),
			STEPS_PER_MM(Y_AXIS),
			STEPS_PER_MM(Z_AXIS));
		debug_position("begin");
	#endif

	// $H or $HZ only

	if (cycle_mask & (bit(X_AXIS) | bit(Y_AXIS) | bit(A_AXIS) | bit(B_AXIS) | bit(C_AXIS)))
	{
		error_all("error: %d only $H or $HZ are allowed in vMachine-home(0x%02x)",
			Error::SettingDisabled,
			cycle_mask);
		return true;
	}

	// Set the Z Axis Zervo at Z_AXIS_SAFE_POSITION
	// We just set sys_position and enable the servo for a while.
	//
	// EXCEPT I effing hate this ... THERE IS NO SIMPLE WAY TO JUST
	// MOVE A MOTOR TO A POSITION AND SET SYS_POSITION.  This scheme
	// is almost impossible to figure out. ALL I WANT TO DO IS MOVE
	// THE Z SERVO TO A SAFE PLACE !!!

	// if (cycle_mask & bit(Z_AXIS))		// for testing, only when Z home button is pressed
	{
		config->_axes->set_disable(Z_AXIS,false);	// motors_set_disable(false, bit(Z_AXIS));
		sys_position[Z_AXIS] = v_machine.getZAxisSafePosition() * STEPS_PER_MM(Z_AXIS);
		vTaskDelay(1000);
		config->_axes->set_disable(Z_AXIS,true);	// motors_set_disable(true, bit(Z_AXIS));

		// BUT HOW DO YOU DO THAT?
		// How bout throwing the kitchen sink out the window?
		// Will that help?

		plan_reset();
		gc_init();
		gc_sync_position();
		plan_sync_position();
	}

	// and if $HZ we are done

	if (cycle_mask == bit(Z_AXIS))
	{
		#if DEBUG_VHOME
			debug_serial("plotter-home Z_AXIS early exit");
		#endif
		return true;
	}

	// Begin XY homing
	config->_axes->set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), true);
		// motors_set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), true);

	sys_position[X_AXIS] = 0;
	sys_position[Y_AXIS] = 0;

	if (vHome(Y_AXIS) && vHome(X_AXIS))
	{
		#if DEBUG_VHOME > 1
			debug_position("middle");
		#endif

		// if both succeeded (should have been Y then X)
		// we are now sitting at a system_pos that is correct
		// relative to the zero stops on both axis IF they were at 0

		// in nominal case the first axis done will be slack at this
		// point and the latter will be close to the X motor at
		// something like (750,19000)

		sys_position[X_AXIS] +=
			(v_machine.getZeroLength() + v_machine.getLeftZeroOffset()) *
			STEPS_PER_MM(X_AXIS);
		sys_position[Y_AXIS] +=
			(v_machine.getZeroLength() + v_machine.getRightZeroOffset()) *
			STEPS_PER_MM(Y_AXIS);

		#if DEBUG_VHOME > 1
			debug_position("end");
		#endif

		// We should now be able to say $G1 X200 Y150 and end up somewhere near
		// the middle of the page .. or $G1 X0 Y0 to go near the bottom left corner
		// of the work area ...

		//--------------------------------------------
		// synchronize the system here
		//--------------------------------------------
		// rant: what is the use of providing a hook for user_defined_homing()
		// (in mc_home()) if the client then has to learn the whole goddamned
		// system to use it?  It would have been nice if this junk was documented
		// somewhere.
		//
		// I believe at least all of the following are necessary

		sys.step_control = {};										// definite
		config->_axes->set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), true);
		// motors_set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), false);	// appears to do nothing with standard steppers
		plan_reset();												// trying to fix segment mc_lines() in cartesian_to_motors()
		gc_init();													// <-- may be required in addition sync_position
		gc_sync_position();											// definite
		plan_sync_position();										// definite
		// kinematics_post_homing();								// not for me, duh
		limits_init();												// very little useful there ...

		#if DEBUG_VHOME
			debug_position("final");
		#endif

	}	// if homed both axis

	in_homing = false;
	return true; // we handled it

}	// user_defined_homing()



bool /*WEAK_LINK*/ not_limitsCheckTravel(float* target)
	// overriden version of limitsCheckTravel
{
	// so far I've just added debugging
	// Todo: enforce limits in terms of machine_width/height and safe area
	//     think about what "max_travel" really means, peg it closer, and
	//     figure out what do use in homing

    auto axes   = config->_axes;
    auto n_axis = axes->_numberAxis;
    for (int axis = 0; axis < n_axis; axis++)
	{
        float max_mpos, min_mpos;
        auto  axisSetting = axes->_axis[axis];
		// debug_serial("limit check %f > %f < %f",limitsMinPosition(axis),target[axis],limitsMaxPosition(axis));

        if ((target[axis] < limitsMinPosition(axis) || target[axis] > limitsMaxPosition(axis)) && axisSetting->_maxTravel > 0)
		{
			debug_serial("limit exceeded %f > %f < %f",limitsMinPosition(axis),target[axis],limitsMaxPosition(axis));
            return true;
        }
    }
    return false;
}
