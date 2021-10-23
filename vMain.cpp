//------------------------------------------------------------------------------
// vMain.cpp
//------------------------------------------------------------------------------
// contains most of the 'glue' (overriden weakly bound methds) to FluidNC

#include <Arduino.h>
#include "vMachine.h"
#include "vKinematics.h"
#include "vSensor.h"
#include "my_ws2812b.h"
#include <FluidNC.h>
#include <Config.h>             // FluidNC
#include <System.h>             // FluidNC
#include <Logging.h>            // FluidNC
#include <Planner.h>            // FluidNC
#include <Protocol.h>           // FluidNC
#include <MotionControl.h>      // FluidNC
#include <GLimits.h>            // FluidNC
#include <FluidDebug.h>     	// FluidNC_extensions
#include <gStatus.h>			// FluidNC_extensions



#define DEBUG_VHOME   	1		// can be up to 2
#define DEBUG_VREVERSE  0
#define DEBUG_VFORWARD  0		// can be up to 2
	// debugging the inverse kinematics may introduce
	// timing problems that can crash the machine ..

#define INIT_SDCARD_AT_STARTUP   1


#if INIT_SDCARD_AT_STARTUP
	#include <SD.h>
#endif

#define STEPS_PER_MM(axis)		(config->_axes->_axis[axis]->_stepsPerMm)



//-----------------------------
// global object declarations
//-----------------------------

vMachine v_machine;
Kinematics	kinematics;
bool in_homing = false;
#ifdef WITH_PIXELS
	Adafruit_NeoPixel pixels(NUM_PIXELS,NEO_PIXEL_PIN);
#endif



//------------------------------------------------------------
// display_init() and user_realtime_command()
//------------------------------------------------------------
// Overrides of WEAK_LINK methods from FluidNC

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


    #ifdef WITH_SCREEN_GRAB
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
    #endif  // WITH_SCREEN_GRAB
#endif  // WITH_UI



//---------------------------------------------------------------------
// static utilities used by machine_init()
//---------------------------------------------------------------------

#if INIT_SDCARD_AT_STARTUP
	static void debug_start_sdcard()
	{
		g_info("starting sdCard via FluidNC");
		SDState state = g_status.getSDState(true);

		if (state == SDState::NotPresent)
		{
			g_info("SD Card Not Present");
		}
		else
		{
			if (state != SDState::Idle)
				g_info("SD Card BUSY");

			uint8_t cardType = SD.cardType();
			g_info("SD Card Type: %s",
				cardType == CARD_NONE ? "NONE" :
				cardType == CARD_MMC  ? "MMC" :
				cardType == CARD_SD   ? "SDSC" :
				cardType == CARD_SDHC ? "SDHC" :
				"UNKNOWN");
			uint64_t cardSize = SD.cardSize() / (1024 * 1024);
			g_info("SD Card Size: %lluMB", cardSize);
			g_info("Total space:  %lluMB", (SD.totalBytes()+1024*1024-1) / (1024 * 1024));
			g_info("Used space:   %lluMB", (SD.usedBytes()+1024*1024-1) / (1024 * 1024));
			//	SD.end();
		}
	}
#endif	// INIT_SDCARD_AT_STARTUP


static inline unsigned long mulRound(float *vals, int axis)
	// change "motor position" (chain lengths) mm's to steps
{
	float mm = vals[axis];
	float steps_per_mm = STEPS_PER_MM(axis);
	float rslt = mm * steps_per_mm + 0.5;
	unsigned long retval = rslt;
	return retval;
}


static void setDefaultPosition()
	// set system at imaginary center position to start
	// These are just imaginary values until we are homed.
{
	memset(motor_steps,0,MAX_N_AXIS * sizeof(unsigned long));

	float position[V_NUM_AXES];
	kinematics.inverse(
		v_machine.getMachineWidth()/2,		// 200
		v_machine.getMachineHeight()/2,		// 150
		&position[X_AXIS],
		&position[Y_AXIS]);
	position[Z_AXIS] = v_machine.getZAxisSafePosition(); // is 20 was 0;
	motor_steps[X_AXIS] = mulRound(position,X_AXIS);
	motor_steps[Y_AXIS] = mulRound(position,Y_AXIS);
	motor_steps[Z_AXIS] = mulRound(position,Z_AXIS);

	g_debug("init motor_steps(%d,%d,%d) mms(%f,%f,%f)",
		motor_steps[X_AXIS],
		motor_steps[Y_AXIS],
		motor_steps[Z_AXIS],
		(float)motor_steps[X_AXIS] / STEPS_PER_MM(X_AXIS),
		(float)motor_steps[Y_AXIS] / STEPS_PER_MM(Y_AXIS),
		(float)motor_steps[Z_AXIS] / STEPS_PER_MM(Z_AXIS));
}



//===========================================================
// machine_init() - override of weakly bound FluidNC method
//============================================================
// Main Entry point to the vMachine

void machine_init()
	// override weakly bound method called from FluidNC
{
	g_info("vMain.cpp::machine_init() on core %d at priority %d %d/%dK",
		xPortGetCoreID(),
		uxTaskPriorityGet(NULL),
		xPortGetFreeHeapSize()/1024,
		xPortGetMinimumEverFreeHeapSize()/1024);

	#if INIT_SDCARD_AT_STARTUP
		debug_start_sdcard();
	#endif

	#ifdef WITH_PIXELS
		pixels.begin();
	#endif

	// initialize the kinematics engine
	// and set the default position

	kinematics.init();
	setDefaultPosition();

	// start the vSensorTask

	xTaskCreatePinnedToCore(
		vSensorTask,		// method
		"vSensorTask",		// name
		4096,				// stack_size
		NULL,				// parameters
		1,  				// priority
		NULL,				// returned handle
		0);					// core 1=main FluidNC thread/task, 0=my UI and other tasks

	delay(400);		// to allow task to display it's starup message

	g_info("vMain.cpp::machine_init() finished %d/%dK",V_SDCARD_CS,xPortGetFreeHeapSize()/1024,xPortGetMinimumEverFreeHeapSize()/1024);
}



//--------------------------------------------------------------------------------------------------------
// cartesian_to_motors - override weekly bound method
//--------------------------------------------------------------------------------------------------------
// the FluidNC method params are ineptly named and ordered

bool cartesian_to_motors(float* target, plan_line_data_t* pl_data, float* position)
	// PRH THE FEED RATES NEED TO BE FIGURED OUT
	// pl_data->feed_rate = 0.5 ...
	// something about the sqrt() of the distance (per segment ?!?)
{
	float new_position[MAX_N_AXIS];
	memset(new_position,0,MAX_N_AXIS * sizeof(float));

	#if DEBUG_VREVERSE
		g_debug("cartesian_to_motors(%f,%f,%f) old(%f,%f,%f) feed=%f",
			target[X_AXIS],
			target[Y_AXIS],
			target[Z_AXIS],
			position[X_AXIS],
			position[Y_AXIS],
			position[Z_AXIS],
			pl_data->feed_rate);
	#endif
	#if DEBUG_VREVERSE>1
		g_debug("   ctm starting sys_lengths(%f,%f,%f) sys_pos(%d,%d,%d)",
			motor_steps[X_AXIS] / STEPS_PER_MM(X_AXIS),
			motor_steps[Y_AXIS] / STEPS_PER_MM(Y_AXIS),
			motor_steps[Z_AXIS] / STEPS_PER_MM(Z_AXIS),
			motor_steps[X_AXIS],
			motor_steps[Y_AXIS],
			motor_steps[Z_AXIS]);
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

	// if neither X or Y changed, or if it's a system motion we do it in one segment
	// we round up to ensure at least one segment

	if (!pl_data->motion.rapidMotion && (xdist!=0 || ydist!=0))
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
		g_debug("   ctm doing %d segments with incs(%f,%f,%f)",
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
			g_debug("   ctm seg(%d) to(%f,%f,%f) lengths(%f,%f,%f)",
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
		g_debug("cartesian_to_motors final lengths(%f,%f,%f) steps(%d,%d,%d)",
			new_position[X_AXIS],
			new_position[Y_AXIS],
			new_position[Z_AXIS],
			mulRound(new_position,X_AXIS),
			mulRound(new_position,Y_AXIS),
			mulRound(new_position,Z_AXIS));
	#endif

	return true;
}


//--------------------------------------------------------------------------------------------------------
// motors_to_cartesian - override weekly bound method
//--------------------------------------------------------------------------------------------------------

void motors_to_cartesian(float* cartesian, float* motors, int n_axis)
{
	#if DEBUG_VFORWARD
		if (prh_debug_forward)
			g_debug("motors_to_cartesian(%f,%f,%f) old(%f,%f,%f)",
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
			g_debug("motors_to_cartesian(%f,%f,%f) final(%f,%f,%f)",
				motors[X_AXIS],
				motors[Y_AXIS],
				motors[Z_AXIS],
				cartesian[X_AXIS],
				cartesian[Y_AXIS],
				cartesian[Z_AXIS]);
	#endif
}


//--------------------------------------------------------------------------------------------------------
// Homing
//--------------------------------------------------------------------------------------------------------


static float *my_get_mpos()
{
	static float motors[MAX_N_AXIS];
	memset(motors,0,MAX_N_AXIS * sizeof(float));
	for (int idx = 0; idx < V_NUM_AXES; idx++)
	{
		motors[idx] = (float)motor_steps[idx] / STEPS_PER_MM(idx);
	}
	return motors;
}


static void debug_position(const char *what)
{
	g_debug("      %-9s sys_pos(%d,%d,%d) sys_lengths(%f,%f,%f)",
		what,
		motor_steps[X_AXIS],
		motor_steps[Y_AXIS],
		motor_steps[Z_AXIS],
		motor_steps[X_AXIS] / STEPS_PER_MM(X_AXIS),
		motor_steps[Y_AXIS] / STEPS_PER_MM(Y_AXIS),
		motor_steps[Z_AXIS] / STEPS_PER_MM(Z_AXIS));
}


static bool vHome(int axis)
	// this is unduly complicated due to the prodiguous
	//     use of global variables, undocumented assumptions about state,
	//     and the generally horrible encapsulation of functionality in
	//     the FluidNC code.
{
	#if DEBUG_VHOME > 1
		g_debug("vHome(%s)",axis?"Y":"X");
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

	auto the_axis = config->_axes->_axis[X_AXIS]; 	// we use the X cannonically
	auto homing = the_axis->_homing;

	float fast_rate = homing->_seekRate;		// DEFAULT_HOMING_SEEK_RATE;
	float slow_rate = homing->_feedRate;		// DEFAULT_HOMING_FEED_RATE;
	float pull_off  = the_axis->_motors[0]->_pulloff;			// DEFAULT_HOMING_PULLOFF;
	float other_axis_travel = EXTRA_TRAVEL_RATIO * the_axis->_maxTravel;	// out
	float axis_travel = -other_axis_travel;								// in

	// fawk - need to clear cycle stop and set the (next) axis motor_steps to zero

	rtCycleStop = false;
	motor_steps[axis] = 0;

	// setup for intial 'seek' move
	// where we wind in the axis and wind out the other_axis
	// note that the other_axis move has been made relative with +=

	float *target = my_get_mpos();				// system_get_mpos();
	target[axis] = axis_travel;					// this axis is at "0" (absoute move)
	target[other_axis] += other_axis_travel;	// relative move
	config->_axes->unlock_motors(bit_axis | bit_other_axis);

	// start initial 'seek' move

	pl_data->feed_rate = fast_rate;   			// Set current homing rate.
	plan_buffer_line(target, pl_data);  		// Bypass mc_line(). Directly plan homing motion.
	sys.step_control = {};
	sys.step_control.executeSysMotion = true;  	// Set to execute homing motion and clear existing flags.
	Stepper::prep_buffer();                     // Prep and fill segment buffer from newly planned block.
	Stepper::wake_up();    						// Initiate motion

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
		g_debug("   vHome phase(%d)  rate(%f)  axis_travel(%f) other_axis_travel(%f)",
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

		int limit_state = getVSensorState();

		// check rtReset and rtCycleStop for outer level errors

		if (rtReset)
		{
			#if DEBUG_VHOME
				g_debug("   vHome rtReset==HomingFailReset 6");
			#endif
			rtAlarm = ExecAlarm::HomingFailReset;		// 6
			home_error = 1;
		}
		else if (rtCycleStop)
		{
			if (!(home_phase & 1))
			{
				#if DEBUG_VHOME
					g_debug("   vHome rtCycleStop==HomingFailApproach 9");
				#endif
				rtAlarm = ExecAlarm::HomingFailApproach;   // 9
				home_error = 1;
			}
			else if (limit_state & bit_axis)
			{
				#if DEBUG_VHOME
					g_debug("   vHome rtCycleStop==HomingFailPulloff 8");
				#endif
				rtAlarm = ExecAlarm::HomingFailPulloff;	// 8
				home_error = 1;
			}
		}

		if (!home_error)
		{
			// if either axis hits a stop,
			// stop the other axis and continue with the main one

			if (limit_state)
				config->_axes->lock_motors(bit_other_axis);

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
				delay_ms(500);  		    // Delay to allow transient dynamics to dissipate.
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
						g_debug("   vHome next_phase(%d)  rate(%f)  axis_travel(%f)",
							home_phase,
							pl_data->feed_rate,
							axis_travel);
						debug_position("next");
					#endif

					// the target must be reset from the motor_steps each new phase
					//     so that other axes are already where they will be targeted
					// and the system_position for the current axis must be zeroed
					// 	   or else the moves are considered to be relative to whatever tf
					//     the system position happens to be after the previous phase
					// you have to know, and manage these global variables and methods
					//     to implement a user_defined_homing() method.

					rtCycleStop = false;
					motor_steps[axis] = 0;
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

		g_error("Homing fail");

		mc_reset();
		protocol_execute_realtime();
		return false;
	}
	else
	{
		#if DEBUG_VHOME > 1
			float *dbg_mms = my_get_mpos();
			g_debug("   vHome(%s) finished",axis?"Y":"X");
			debug_position("end");
		#endif

		// so, at this point, this axis is now PULL_OFF mm from the limit mark

		return true;
	}

}	// vHome() one axis




bool user_defined_homing(AxisMask cycle_mask)
	// override weekly bound method.
	//
	// The primary purpose of this method is to establish the physical location of
	//     the sled by determining the lengths of the X and Y belts at an arbitray
	//     position.
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
{
	in_homing = 1;

	#if DEBUG_VHOME
		g_debug("vMachine-home(0x%02x)",cycle_mask);
		g_debug(" STEPS_PER_MM(%f,%f,%f)",
			STEPS_PER_MM(X_AXIS),
			STEPS_PER_MM(Y_AXIS),
			STEPS_PER_MM(Z_AXIS));
		float *dbg_mms = my_get_mpos();
		debug_position("begin");
	#endif

	// $H or $HZ only

	if (cycle_mask & (bit(X_AXIS) | bit(Y_AXIS) | bit(A_AXIS) | bit(B_AXIS) | bit(C_AXIS)))
	{
		g_info("error: %d only $H or $HZ are allowed in vMachine-home(0x%02x)",
			Error::SettingDisabled,
			cycle_mask);
		return true;
	}

	// Set the Z Axis Zervo at Z_AXIS_SAFE_POSITION
	// We just set motor_steps and enable the servo for a while.

	// if (cycle_mask & bit(Z_AXIS))		// for testing, only when Z home button is pressed
	{
		config->_axes->set_disable(Z_AXIS,false);	// motors_set_disable(false, bit(Z_AXIS));
		motor_steps[Z_AXIS] = v_machine.getZAxisSafePosition() * STEPS_PER_MM(Z_AXIS);
		vTaskDelay(1000);
		config->_axes->set_disable(Z_AXIS,true);	// motors_set_disable(true, bit(Z_AXIS));

		// magic

		plan_reset();
		gc_init();
		gc_sync_position();
		plan_sync_position();
	}

	// and if $HZ we are done

	if (cycle_mask == bit(Z_AXIS))
	{
		#if DEBUG_VHOME
			g_debug("plotter-home Z_AXIS early exit");
		#endif
		return true;
	}

	// Begin XY homing
	config->_axes->set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), true);
		// motors_set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), true);

	motor_steps[X_AXIS] = 0;
	motor_steps[Y_AXIS] = 0;

	if (vHome(Y_AXIS) && vHome(X_AXIS))
	{
		#if DEBUG_VHOME > 1
			debug_position("middle");
		#endif

		// if both succeeded (should have been Y then X)
		// we are now sitting at an arbirary system_pos (motor_steps),
		// pulled off of the x homing position, that is CORRECT relative to the zero
		// stops on both axis AS IF they were at 0 when they were triggered.
		//
		// We now add in the magic diagonal distance to the work area
		// minus (plus the negative) left and right zero offsets.

		motor_steps[X_AXIS] +=
			(v_machine.getZeroLength() + v_machine.getLeftZeroOffset()) *
			STEPS_PER_MM(X_AXIS);
		motor_steps[Y_AXIS] +=
			(v_machine.getZeroLength() + v_machine.getRightZeroOffset()) *
			STEPS_PER_MM(Y_AXIS);

		#if DEBUG_VHOME > 1
			debug_position("end");
		#endif

		// We should now be able to say $G1 X200 Y150 and end up exactly at the
		// middle of the page .. or $G1 X0 Y0 to the bottom left corner
		// of the work area ...

		//--------------------------------------------
		// synchronize the system here
		//--------------------------------------------
		// I believe at least all of the following are necessary

		sys.step_control = {};										// definite
		config->_axes->set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), false);
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

	else
	{
		config->_axes->set_homing_mode(bit(X_AXIS) | bit(Y_AXIS), false);
		setDefaultPosition();
	}

	in_homing = false;
	return true; // we handled it

}	// user_defined_homing()
