This notes.md contains various notes to myself.


## Ideas for SDCard rewrite

1. Implement the SD State as a bitwise value
2. Initialize the SD Card, if present, at startup with notification.
3. Implement a low priority task to check for SD Card insertion and removal
4. Issue CLIENT_ALL notifications upon idle removal and insertion of SD Card
5. Issue CLIENT_ALL Alarms upon ANY SDCard error, including detecting removal
   while a file/dir is open.
5. DONT start() and end() the SD card willy-nilly all over the place
6. Allow concurrent access where it makes sense.
7. Really should STOP using the state of the SDCard as a behavior semaphore.
   The concept is really a "job is running" which *should* be doable from
   a client (I want exclusive access to the gcode machine) (i.e. by % sign)
   as well as from the SD Card.

   HOWEVER that is beyond my scope for today, so the notion that an
   SDCard job is running will continue to preclude other GCode from
   running, but otherwise, it's first-come first served.

THE BIGGEST SCARY ISSUE is that, at a low level, accessing the SDCard
or SPIFFs will upset the motor control or other intrrupts. I think it
is true for SPIFFS (the "EEPROM") from my readings. HOWEVER I don't see any
code that indicates that anyone tried to prevent that particular issue.
It seems to be more about protecting the GCODE machine from getting mixed
up WHILE a job was running than protecting the GCode machine from timing
interrupts due to the job running.

All I can tell you is that it was a mess, using the SDState as a semaphore
for all kinds of behaviors.



## G codes, M codes, SD Card, Jobs, and multiple concurrent interfaces

Yikes. Yech.

There is no standard for G codes and M codes.  There are only implementations.

There are purported standards (i.e. GRBL 1.1) but they are not technical specifications.

The field is vast, wide, and wild.


### Lack of "job" demarcations

In 'gcode' there is no standardized encapsulation of a "job".

Note that there is not even a documented "standard set" of gcodes
related to GRBL 1.1 .. try to find it!  If you dig, you will get
a disclaimer that GRBL is a "beta" thing, and a "discussion" of
the gcodes is available at https://wiki.shapeoko.com/index.php/G-Code

From that page: "Program Start/End --- this is traditionally indicated by a "%" symbol
--- since most controllers ignore anything else on the line it may
be abused as a way to insert a comment."

Handy.

There is no consistent concept for starting and ending jobs
with streaming interfaces, except, perhaps in the G28? "send
file to the SD card" command, where the percent signs (should
be) (are) mandatory to signal the start end end of the file
for transmission over the serial port.


### Gcode Analsyis

For completeness, please note the following RepRap M codes which are
NOT part of GRBL ... these codes have completely different semantics
in GRBL and there are no equivilants in FluidNC:

    M21 Initialise (mount) SD Card
    M22 Release (unmount) SD Card
    M23 Select File for Printing
    M24 Start / Resume SD Card Print (see M23)
    M25 Pause SD Card Print (see M24)
    M26 Set SD Position in bytes
    M27 SD Print status
    M28 Write programm to SD Card
    M29 Stop writing programm to SD Card





## Relative Coordinate Systems

When you press the "zero xyz" button in the WebUI it emits this command:

G10 L20 P0 X0 Y0 Z0

This tells the machine to assign the coordinate system 0 (P0)
zero relative to the current positions (L20) of the machine XYZ ... Here
we did that when the machine was at 200,150,4.

(G10 L2 lets you set the numbers directly)

If you type $# you can see the machine's coordinate systems:

    $#
    [G54:200.000,150.000,4.000]
    [G55:0.000,0.000,0.000]
    [G56:0.000,0.000,0.000]
    [G57:0.000,0.000,0.000]
    [G58:0.000,0.000,0.000]
    [G59:0.000,0.000,0.000]
    [G28:0.000,0.000,0.000]
    [G30:0.000,0.000,0.000]
    [G92:0.000,0.000,0.000]
    [TLO:0.000]
    [PRB:0.000,0.000,0.000:0]

G54 through G59 define six coordinate systems that you may
change between  G28 and G30 are predefined positions, and
G92 is a coordinate system offest.

They can be cleared with $RST=#

Note that one of these is always active.
And that G54 really serves as the "default" machine setup.
It should really NOT be used by the WebUI IMHO ...

The following command sets 0,0 to the bottom left
corner of a 12x9 paper centered in the work area
at z=4.

    ; 12 * 25.4 = 304.8
    ; 9 * 25.4 = 228.6
    ; (400-304.8)/2 = 47.6
    ; (300-228.6)/2 = 35.7

    G10 L2 P2 X47.6 Y35.7 Z4    ; define the P2=G55 coordinate system
    G55                         ; switch to it

By not using G54, which we save as the default coordinate system,
we can return to 'machine' coordintes by merely executing G54

    G54

I can now adjust the Z height with
    G10 L2 P2 Z3.4


And just have inkscape "GOTO ZERO" for the Z-Axis, whatever
that is.

And can either switch to the G55 coordinate system manually,
or via the header/footer in inkscape



## COORDINATE SYSTEMS DETAILS

motor_steps = an array of int32s representing the stepper motor position in steps
motor position = floats in mm's == sys_position / axis_settings->steps_per_mm
machine position = floats called "mpos" which is the cartesian coordinate within the work area,
work position =  floats (wpos) that lets you set 0,0 relative to the machine area.

The "motor position" and motor_steps are representative of the chain (cable) lengths.

The system does not maintain the motor, machine, or work positions.
It only maintains the int32 sys_positions.

If FluidNC needs to do something in the machine/work coordinate systems (i.e. move to the
next point in some gcode) it calls cartesianToMotors() to get the "motor positions"
for that coordinate, which it then turns back into sys_position steps for planning
and actual movement.

For reporting, motor_steps is turned into the "machine position" by
calling motors_to_cartesian() (forward kinematics), which in turn, does
iterative calls to the reverse kinematics to find a "machine_position" that matches
the motor position.  These forward kinematics are normally not needed to run FluidNC,
they are used only for reporting.


## HOMING AND ZERO STOPS

OLD NUMBERS!


```
#define ZERO_LENGTH  223.959     // mm. just about 224
   sqrt(MOTOR_OFFSET_Y*MOTOR_OFFSET_Y + MOTOR_OFFSET_X*MOTOR_OFFSET_Y))
        the length of the hypotenuse of the right trangle
        from motor center points to the top left/right corners
        of the machine area
```

The forward edge of the "zero stop" white lines are painted on the
belt 21.4 mm from the center of the sled.  This is 10mm "in" from the pully
when the sled is at zero cable length. The stop will get not get
triggered util the cable length is logically -10 (minus ten)
millimeters (AFTER it passes zero) while reeling in the belt
during homing.

```
#define LEFT_ZERO_OFFSET    -10.000     // mm == -500 steps
#define RIGHT_ZERO_OFFSET   -10.000
```

These allow for an approximately 7mm horizontal and vertial
"safe" zone around the machine area that the system can still
navigate in and ensures that the end stops are not triggered
within the work area.

TBD: The "max" stops are also painted in such a way as to allow a bit
more than the maximum length, but it is the zero stops that are
important in homing and all subsquent calculations and the overall
accuracy of the machine.

We cannot o use the $XY/Home/MPos existing config EEPROM space
to store these as preferences as that has a different semantic.

Modifying the above constants is possible to account for the fact
that the sensors may trigger sooner or later (not exactly on the edge)
by doing a Homing cycle and, after the "pull off" (15 mm in our case)
one could measure the distance from the white line to TDC of the pulley
and adjusting the constants until it is actually 15mm from TDC after
the pulloff.


## RC_SERVO

### Top Level Notes

The RC Servos are enabled/disabled for G0 moves and enabled for G1 moves.
So, you cannot use a "jog" command to probe zero as it will end up disabling
the servo and it will bounce fall back.  But with G1 commands, they will move
and hold their position.

The const double values in RcServoSettings.h for min/max mm's are RED_HERRINGS.
The are not used in any code.

    const double SERVO_POSITION_MIN_DEFAULT = 0.0;   // mm
    const double SERVO_POSITION_MAX_DEFAULT = 20.0;  // mm


### How they work

The basic algorithm is that an RC_Servo will send out a PWM pulse at the
correct intervals.  The PWM pulse will be between SERVO_MIN_PULSE_SEC and
SERVO_MAX_PULSE_SEC as defined in RCServoSettings.h.  Those pulses define
the 0 and maximum positions of the servo (motor).  The default settings result
in about 90 degrees on the blue SG90 servos.

These values can be "scaled" by user parameters $Z/RcServo/Cal/Max and
$Z/RcServo/Cal/Min which default to 1 to change the overall throw range.
I do not scale them at this time.

These servo positions are mapped to the'machine positions' from "Home/MPos"
to "maxTravel" which are 0 to 90 (DEFAULT_Z_MAX_TRAVEL) in our implementation,
and happen, when all is said and done, to represent degrees on the servo
(because it happens to move 90 degrees over it's range of travel).

Note that we have to set the z-bit into HOMING_DIR_MASK (global
dirInvertMask) so that the normal expected behavior takes place.

My device has the servo mounted such that it moves counterlockwise
for increasing motor positions, away from the work surface.

In my kinematics, I just pass the target position to mc_line() as the
desired position(), which is 'degrees' from some known position.

We *could* convert that accurately to mm's usng trig, but we don't.


### Feed Rate and Acceleration

The default feed rate for the ZAXIS in the WEBUI of 100mm/min is bogus,
and really led to a lot of confusion.  That comes out to 1.6 degrees
per second which is barely noticable on the servo.  I thought it was
not working.  So, first thing is to change the WebUI preference to
something that shows it working, like 1000mm (or more) per minute.

1000 is a little slow for actual work.  4000 is better.

If the motor moves 90 units (which are degrees but FluidNC thinks are mms),
and the feed rate is 1000mm/minute, which is about 60mm/second, it would
take 1.5 seconds to move 90 degrees.  If my pen needs to move, say 20 degrees
between a safe place and the work surface that would be 1/4 t 1/2 second
per feed move, and if the max_rate is 4000mm/minute, pretty quick to retract.


### Using Inkscape and gCodeTools

1. Open or Create the Desired SVG.
2. Create a new layer for gCodeTools and lock all the actual layers in the SVG.
   With the new layer selected:
3. Go to Extensions-gCodeTools-OrientationPoints and ceate a set of
   points with zSurface=1 and zDepth=0. Pressing "Apply" will cause it to
   create Text" items into the layer for the points.  These values will
   cause it to do one pass with moves to Z0 to cut.
4. Go to gCodeTools-ToolsLibrary and add a default Tool by pressing Apply.
   You can then edit the text in the "tool" with the Inkscape Text tool.
5. Edit the tool definition to change "Penetration Feed" to 1000 (zAxis feed rate)
   The default XY feed of 400 is ok.
6. Go to gCodeTools-PathToGCode.  Under Preferences, set the Z safe height
   to 6 or 20 or whatever.
7. Make sure the Preferences-Directory is set to /junk
8. If present the files "header" and "footer" will be included in the resultant
   gcode.  RECOMMEND that these exist and just have comments in them.
9. Finally, go to the "Path to Gcode" tab and press Apply.
   (the Apply button does not work in Preference or Options window)
10. This will create a sequenced gCode ile in /junk


### other notes

1. To get 180 degrees of motion you can modify Motors/RCServoSettings.h
   and change SERVO_MIN_PULSE_SEC and SERVO_MAX_PULSE_SEC from
   0.001,0.002 to  0.0005,0.0021  .. it sort of works but especially
   the cheap blue ones "jump" backwards sometimes (not liking it for pens
   so I'm reverting to 90 degrees of 'travel'

2. You can get 180 degrees at runtime (and probably in DEFAULTS) by setting
   $Z/RcServo/Cal/Max=1 +higher and $Z/RcServo/Cal/Min=1 -lower to match the
   ratio of commenteed out ms in RCServoSettings.h.  Something like

        $Z/RcServo/Cal/Min=0.5  = 0.0005/0.001  (50% of 0.001 secnods) and
   		$Z/RcServo/Cal/Max=1.05 = 0.0021/0.002  (105% of 0.002 seconds)

   But the servos are wonky at those settings.

   Note that it would be simpler to just send the desired PWM consistently
   but FluidNC feels the need to do it incrementally (with accelleration) which
   really mucks thigs up, I think, causing, probably, the jumping around.
   To save, i guess, leaving the PWM signal from going out and parking the servos.
   I'm wonked out!


## SD Card Resistor

**I always had problems with the SDCard on power on resets**

I finally added a **1K pullup resistor from MISO(19) to 3.3V**
which seemed to make it work.

**DON'T** hookup 5V to the SDCard VCC!!  It will then drive
5V out over MISO and could damage the ESP32.

**The 1K pull on MISO made it work.**

Adding the SD Card Module to my prototype circuit required almost
no code changes (except to fix a stack overflow) but was complicated
to learn and do.   So I record these notes:

The SD Card is not commented/documented very well.

The SD Card makes use of the pin defaults in the Espresif-ESP32 SD/FS libarary,
where the CS (chip select) pin is "GPIO_NUM_5" and the standard MOSI,MISO, and CLK
pins are used.  I list them here as a reminder

    #define SDCARD_CS    GPIO_NUM_5
    #define SPI_CLK      GPIO_NUM_18
    #define SPI_MISO     GPIO_NUM_19
    #define SPI_MOSI     GPIO_NUM_23

There is some kind of "SDCard detection scheme" in FluidNC
that I do not understand.   In any case, you should leave
SDCARD_DET_PIN undefined.  At one point I tried the following
and it did not help: $define SDCARD_DET_PIN GPIO_NUM_5

Any time I would try to access the file window from the WebUI,
the ESP32 would crash.  It turned out to be a stack overflow
that I "corrected" below, but I didn't understand it at first,
and so I started by trying to put debugging in SDCard.cpp's
get_sd_state() method, which is where it eventually calls SD.begin().
I could make no progress.

I compiled the ESP32 example SDCard program and it worked fine.
So I added code to prh_v_plotter.cpp machine_init() to call SD.begin()
and report the type, size, total, and used space on the SD Card during
initialization.  That worked fine and is now ifdef'd out.

Then I was able to learn that I could hit the SD Card via the
Serial (gcode) command line, but it was obscure information.
There is an obsolete webpage up that says you can use the $FM
command to mount the SDCard and $F to list files, but those give
"Error:20" (unsupported GCode).

### MISO Pullup Resistor

- Initializing the SDCard is inconsistent.
- A 50K pullup from MISO to 3V was recommended on the net.
- A 10K that I had handy seemed to help.
