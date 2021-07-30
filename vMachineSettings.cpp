// vMachineSettings


#include "vMachine.h"

#if WITH_VMACHINE

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


//------------------------------------------------------------------
// 2020-07-29 Configuration Experiments
//------------------------------------------------------------------
// There are many things I don't like.
//
// - the "machine" must be bound to a yaml file for anything to make sense
// - you cannot persistently change the vast majority of settings in the yaml file
//   though Grbl_Esp32 says "ok" if you try.
// - you can only change a small subset that the "team" has determined
//
// It barely works and is weird.  You have to edit and upload the yaml
// file to accomplish very common things, the most notable being:
//
// - change from STATION to AP mode
// - change the STATION SSID to which you connect
// - tune the settings (i.e. vMachine stuff) persistently
// - change to a bigger machine of the same type (i.e. a bigger vMachine)
//
// And perhaps most primarily, you HAVE to specify (or have uploaded)
// the correct working YAML file that is bound to your "machine" for
// anything to make sense.
//
//--------------------------------------------------------------------------
// The config.yaml included in our data directory is a denormalized
// copy of that from the Grbl_Esp32 library. By default it boots up
// with NO $comms/wifi_ap "section"
//---------------------------------------------------------------------------
//
// Today is experimentation to see if I can set things up to effect some
// kind of reasonable behavior.  My possible entry points are:
//
// grbl_init()          abstracted
//
// display_init()    - called just before settingsInit()
//
// settingsInit() sets global "setting" config_filename
//
//        config_filename = new StringSetting(EXTENDED, WG, NULL, "Config/Filename", "config.yaml");
//        from SettingDefinitions.cpp::make_settings() which is calle from ProcessSettings.cpp::settings_init()
//        which is called immediately AFTER display_init().
//
// bool configOkay = config->load(config_filename->get());
//
//      the primary STATIC entry point to configuration
//      yaml configuration takes place here.
//      I can diddle and add settings during overridden
//          vMachineSettings.cpp::afterParse() method
//
// I can't get *in-between* the setting of the default filename
// and it's passing to the static (non virtual) "load()" method.
//
// global non-static variable in MachineConfig.cpp *might* allow me to "force" a config file
// even if there is NO yaml file (but NOT if there is a config.yaml)
//
//   char defaultConfig[] = "name: Default\nboard: None\n";
//
// I still can't believe you cant change STA/SSID or "mode" at
// runtime.  This thing is so "hodge-podgy"
//
//------------------------------
// What do I want?
//------------------------------
//
// Well, i think you should be able to modify all those "settings" at runtime,
// like you used to.   It should create them (and read them) from EEPROM overriding
// the ones, if any, from the YAML settings. The YAML settings should define the
// "base" machine.  And *especially* the comm settings should, by default, include
// a station object and allow for the overriding of the station SSID (not done)
// like is done with the password at this time.
//
//
// So my choices:
//
//   (a) include my vMachine.yaml as the default "config.yaml" file
//   (b) include an empty STATION in the config.yaml
//   (c) allow for persistent values from EEPROM to override any existing
//       configured setting persistently.  Can note when they are different
//       than the yaml/default settings and remove/add them to EEPROM as
//       needed.
//
//  $comms/wifi_sta/ssid=THX36
//
//------------------------------------------
// How does this thing effing work?
//------------------------------------------
//
// Focusing on $machine_width=410
//
//      (1) all the blah-blah ends up calling ProcessSettings.cpp::do_command_or_setting(key,value), which then
//      (2) constructs a Configuration::RuntimeSetting rts(key, value, out)
//


#define CONFIGURE_WIFI_EXPERIMENT   	0
#define PERSISTENT_SETTING_EXPERIMENT  	0

#if CONFIGURE_WIFI_EXPERIMENT
	#include <Machine/Communications.h>
	#include <Machine/WifiAPConfig.h>
	#include <Machine/WifiSTAConfig.h>
#endif

#if PERSISTENT_SETTING_EXPERIMENT
	#include <nvs.h>
	#include <Settings.h>
	#include <Configuration/ParserHandler.h>
#endif


void vMachine::afterParse() // override
{
	v_debug("vMachine::afterParse() called");

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

	Machine::MachineConfig::afterParse();
}




void vMachine::group(Configuration::HandlerBase& handler) // override
{
	// handler.section("vMachine", NULL);

	// this method is called A LOT
	// it would be nice to know WHY

	#if PERSISTENT_SETTING_EXPERIMENT
		const char *key_name = "machine_width";

		const char *htype = "UNKNOWN";
		switch (handler.handlerType())
		{
			case Configuration::HandlerType::Parser		:  htype="Parser";  break;
			case Configuration::HandlerType::AfterParse	:  htype="AfterParse";  break;
			case Configuration::HandlerType::Runtime	:  htype="Runtime";  break;
			case Configuration::HandlerType::Generator	:  htype="Generator";  break;
			case Configuration::HandlerType::Validator	:  htype="Validator";  break;
		}

		v_debug("vMachine::group(handler=%s) called",htype);
		nvs_handle _nvs = Setting::_handle;
		// v_debug("NVS HANDLE=%08x",(uint32_t)_nvs);

		if (handler.handlerType() == Configuration::HandlerType::Parser)
		{
			Configuration::ParserHandler *parser = (Configuration::ParserHandler *) &handler;
			// v_debug("    ParserHandler.path=%s",parser->_path);
		}

		static bool started = false;
		if (_nvs && !started)
		{
			started = true;

			nvs_stats_t stats;
			esp_err_t err = nvs_get_stats(NULL, &stats);
			if (!err)
			{
				log_info("NVS Used:" << stats.used_entries << " Free:" << stats.free_entries << " Total:" << stats.total_entries << " NameSpaceCount:" << stats.namespace_count);
				#if 0  // The SDK we use does not have this yet
					nvs_iterator_t it = nvs_entry_find(NULL, NULL, NVS_TYPE_ANY);
					while (it != NULL)
					{
						nvs_entry_info_t info;
						nvs_entry_info(it, &info);
						it = nvs_entry_next(it);
						log_info("namespace:"<<info.namespace_name<<" key:"<<info.key<<" type:"<< info.type);
					}
				#endif
			}

			size_t  len = 0;
			err = nvs_get_str(_nvs, key_name, NULL, &len);
			if (!err)
			{
				char buf[len];
				err = nvs_get_str(_nvs, key_name, buf, &len);
				if (!err)
				{
					v_debug("PRH GOT config value for v_machine_width=%s",buf);
					char* floatEnd;
					v_machine_width = strtof(buf, &floatEnd);
				}
			}
		}
	#endif	// PERSISTENT_SETTING_EXPERIMENT

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

	#if	PERSISTENT_SETTING_EXPERIMENT
		if (handler.handlerType() == Configuration::HandlerType::Runtime)
		{
			if (v_machine_width != VMACHINE_DEFAULT_MACHINE_WIDTH)
			{
				v_debug("DETECTED CHANGED RUNTIME VALUE v_machine_width=%3.1f",v_machine_width);
				char buf[8];
				sprintf(buf,"%3.1f",v_machine_width);
				nvs_set_str(_nvs,key_name,buf);

			}
			else
			{
				v_debug("DETECTED DEFAULT VALUE v_machine_width=%3.1f",v_machine_width);
				nvs_erase_key(_nvs, key_name);
			}
		}
	#endif

	Machine::MachineConfig::group(handler);
}


void vMachine::initSettings()
	// called from my derived MachineConfig ctor
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
