// vMachineSettings

#include "vMachine.h"

#define IMPLEMENT_YAML_OVERRIDES        1
#define DEBUG_YAML_OVERRIDES    		1

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
	#if DEBUG_YAML
		v_debug("---> vMachine::afterParse() called");
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
		v_debug("---> vMachine::group(handler=%s) called",htype);
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



#if IMPLEMENT_YAML_OVERRIDES

	#include <SPIFFS.h>
	#include "Configuration/RuntimeSetting.h"

	#define YAML_FILENAME   	"/yaml_tmp.txt"
	#define YAML_TEMPNAME   	"/yaml_tmp.tmp"
	#define MAX_YAML_LENGTH		128

	static char yaml_buf[MAX_YAML_LENGTH+1] = {0};

	static const char *getYamlLine(File &f)
	{
		int len = 0;
		int c = f.read();
		yaml_buf[0] = 0;
		while (c >= 0 && c != '\n' && len < MAX_YAML_LENGTH)
		{
			yaml_buf[len++] = c;
			c = f.read();
		}
		if (len)
		{
			yaml_buf[len] = 0;
			char *p = yaml_buf;
			while (*p && *p != '=') p++;
			if (*p == '=')
			{
				*p++ = 0;
				#if DEBUG_YAML_OVERRIDES > 1
					v_debug("getYamlLine(%s,%s)",yaml_buf,p);
				#endif
				return p;
			}
		}
		#if DEBUG_YAML_OVERRIDES > 1
			v_debug("getYamlLine returning NULL");
		#endif
		return NULL;
	}


	Error saveYamlOverride(const char *path, const char *value)
		// Open the yaml.tmp file, see if the path is in it.
		// If the path was not in the file, open for write (append) and add it.
		// Otherwise, create a new temporary copy replaing the setting in the existin file and rename it.
	{
		static bool in_override = false;

		if (in_override)
		{
			v_error("saveYamlOverride(%s,%s) re-entered",path,value?value:"NULL");
			return Error::AnotherInterfaceBusy;
		}
		in_override = true;

		// First pass through file to see if path exists in it

		#if DEBUG_YAML_OVERRIDES
			v_debug("saveYamlOverride(%s,%s)",path,value?value:"NULL");
		#endif

		Error err = Error::Ok;
		bool file_exists = false;
		bool file_has_path = false;
		if (SPIFFS.exists(YAML_FILENAME))
		{
			file_exists = true;
			File f = SPIFFS.open(YAML_FILENAME);
			if (f)
			{
				while (getYamlLine(f))
				{
					if (!strcmp(path,yaml_buf))
					{
						file_has_path = true;
						#if DEBUG_YAML_OVERRIDES > 1
							v_debug("saveYamlOverride() file_has_path");
						#endif
						break;
					}
				}
				f.close();
			}
			else
			{
				v_error("saveYamlOverride() could not open SPIFFS %s for reading",YAML_FILENAME);
				err = Error::FsFailedOpenFile;
			}

		}
		else
		{
			#if DEBUG_YAML_OVERRIDES > 1
				v_debug("saveYamlOverride() %s does not exist",YAML_FILENAME);
			#endif
		}

		// 2nd pass append to existing file or copy/rename

		if (err==Error::Ok && !file_exists || !file_has_path)		// just append to existing file
		{
			File f = SPIFFS.open(YAML_FILENAME,FILE_APPEND);
			if (f)
			{
				f.print(path);
				f.print("=");
				f.print(value);
				f.print('\n');
				f.close();
				#if DEBUG_YAML_OVERRIDES > 1
					v_debug("saveYamlOverride() appended %s=%s to %s",path,value,YAML_FILENAME);
				#endif
			}
			else
			{
				v_error("saveYamlOverride() could not open SPIFFS %s for writing",YAML_FILENAME);
				err = Error::FsFailedOpenFile;
			}
		}
		else if (err==Error::Ok)		// copy from old file to new file then rename
		{
			#if DEBUG_YAML_OVERRIDES > 1
				v_debug("saveYamlOverride() creating %s from %s",YAML_TEMPNAME,YAML_FILENAME);
			#endif

			File fi = SPIFFS.open(YAML_FILENAME);
			if (!fi)
			{
				v_error("saveYamlOverride() could not open SPIFFS %s for reading",YAML_FILENAME);
				err = Error::FsFailedOpenFile;
			}
			else
			{
				File fo = SPIFFS.open(YAML_TEMPNAME,FILE_WRITE);
				if (!fo)
				{
					fi.close();
					v_error("saveYamlOverride() could not open SPIFFS %s for writing",YAML_TEMPNAME);
					err = Error::FsFailedOpenFile;
				}
				else
				{
					const char *yaml_value;
					while (yaml_value = getYamlLine(fi))
					{
						if (!strcmp(path,yaml_buf))
						{
							#if DEBUG_YAML_OVERRIDES > 1
								v_debug("saveYamlOverride() replaced %s=%s",path,value);
							#endif
							if (fo.print(path) != strlen(path) ||
								fo.print("=") != 1 ||
								fo.print(value) != strlen(value))
							{
								v_error("saveYamlOverride() could not write yaml_override %s=%s",path,value);
								err = Error:: NvsSetFailed;
								break;
							}
						}
						else
						{
							if (fo.print(yaml_buf) != strlen(yaml_buf) ||
								fo.print("=") != 1 ||
								fo.print(yaml_value) != strlen(yaml_value))
							{
								v_error("saveYamlOverride() could not write existing yaml_override %s=%s",yaml_buf,yaml_value);
								err = Error:: NvsSetFailed;
								break;
							}
						}
						fo.print('\n');
					}

					fo.close();
					fi.close();

					if (err==Error::Ok)
					{
						if (!SPIFFS.remove(YAML_FILENAME))
						{
							v_error("saveYamlOverride() could not remove %s",YAML_FILENAME);
							err = Error::NvsSetFailed;
						}
						else if (!SPIFFS.rename(YAML_TEMPNAME,YAML_FILENAME))
						{
							v_error("saveYamlOverride() could not renane %s to %s",YAML_TEMPNAME,YAML_FILENAME);
							err = Error::NvsSetFailed;
						}

					}	// no error copying from one to the other
				}	// no error opening fo
			}	// no error opening fi
		}	// no error on 1st read pass

		#if DEBUG_YAML_OVERRIDES > 1
			v_debug("saveYamlOverride() returning %d",err);
		#endif

		in_override = false;
		return err;
	}


	void loadYamlOverrides()
	{
		#if DEBUG_YAML_OVERRIDES
			v_debug("loadYamlOverrides()");
		#endif
		if (SPIFFS.exists(YAML_FILENAME))
		{
			File f = SPIFFS.open(YAML_FILENAME);
			if (f)
			{
				const char *yaml_value;
				while (yaml_value = getYamlLine(f))
				{
					Configuration::RuntimeSetting rts(yaml_buf, yaml_value, NULL);
					config->group(rts);

					// the runtime setting already set the value into the tree,
					// so validating it at this point is a bit anachrynous.
					// what about if it is a non-existent key (i.e. gone out of date)
					// or if it changed types?

					if (!rts.isHandled_)
					{
						v_error("YamlOverride(%s,%s) not handled!",yaml_buf,yaml_value);
					}

					// 	try
					// 	{
					// 		Configuration::Validator validator;
					// 		config->validate();
					// 		config->group(validator);
					// 	}
					// 	catch (std::exception& ex)
					// 	{
					// 		log_error("Validation error: " << ex.what() << " at line(" << line_num << "): " << yaml_buf << "=" << yaml_value << " in " << YAML_FILENAME);
					// 	}
				}

				f.close();
			}
			else
			{
				v_error("ERRORcould not open SPIFFS %s for reading",YAML_FILENAME);
			}
		}
		else
		{
			#if DEBUG_YAML_OVERRIDES > 1
				v_debug("loadYamlOverrides() %s does not exist",YAML_FILENAME);
			#endif
		}
	}


	void clearYamlOverrides()
	{
		#if DEBUG_YAML_OVERRIDES
			v_debug("clearYamlOverrides()");
		#endif
		SPIFFS.remove(YAML_FILENAME);
		SPIFFS.remove(YAML_TEMPNAME);
	}

#endif	// IMPLEMENT_YAML_OVERRIDES
