# The vMachine - Home

**Home** --
**[History](docs/history.md)** --
**[Hardware](docs/hardware.md)** --
**[Electronics](docs/electronics.md)** --
**[User Interface](docs/ui.md)** --
**[Software](docs/software.md)** --
**[Installation](docs/installation.md)**

The vMachine is a full featured proof-of-concept implementation of a **V plotter**.
It is essentially a CNC machine that uses *triangular kinematics* to work in a
cartesian coordinate system.

It is based on the inexpensive, yet powerful **ESP32** module, rather than the usual
Arduino class proessors.  The ESP32 can easily be programmed from within the
Arduino development environment and has *dual cores*, runs at *240 Mhz* (as opposed to
the Arduino at 16Mhz) and comes with *520KB* (as opposed to 2KB) *of RAM*.  In addition
the ESP32 includes full onboard **WiFi and Bluetooth** capabilities, and can be had for
$5, or about half the price of a genuine Arduino.

Therefore my *primary design decision* was to base this, and my future CNC projects
on an ESP32 processor.

**VIDEO - introduction to the vMachine**

The vMachine project is intended to accomplish several goals. Firstly, I wanted
to start developing and publishing open source CNC machines with the ESP32.
And so I built the machine and now present the designs and information
needed to build one.

The vMachine includes a full **Web Browser User Interface** so that it can be
controlled wirelessly from your phone or computer via **WiFi**.  It can also be controlled
via the **serial port** on the ESP32, via **Telnet** on port 23, or via **Bluetooth**
for usage with commonly available gcode sender programs.
Finally, it can also store gcode files on the optional **SD Card** and run jobs entirely
locally, right on the ESP32, via the optional **Touch Screen User Interface**, without
involving any other devices.

This repository includes all of the files and information needed to
build a copy of this machine.  It includes the files needed to **3D print** the plastic
parts, as well as the underlying Fusion 360 designs if you need
or want to modify the 3D printed parts. It includes all of the **schematics**
and information needed to breadboard or otherwise create the electronic circuits,
as well as the KiCad files behind those, and the Flatcam and GCode files I used to
mill my own **circuit boards** for this version.


I have tried to provide a general roadmap for building a vMachine on these
pages.

A specific goal of this vMachine project is that it is generally scalable
to a larger machine, specifically one that can work with 4x8 feet sheets
of plywood, using a router to cut the plywood, as with the
[Maslow CNC](https://www.maslowcnc.com/).
Credits go to the Maslow team, as this vMachine source code actually
contains a *port* of the **Maslow triangular kinematics** to the ESP32.
I hope to build such a machine in the near future, and it should be
relatively straight-forward to scale this project up to a larger
machine of some sort.  The size of the machine is entirely parameterized
within the software and user interface.

But most importantly, this project establishes a **general approach** to
building ESP32 based CNC machines of various types, not only the vMachine
but also typical 3 and 4 axis CNC machines or any other kind of machine
I might choose to build in the future.  It is entirely based on the
next generation
[FluidNC](https://github.com/bdring/FluidNC)
project, a hugely important codebase that I have
forked and modified slightly so-as to better be able build projects
within the Arduino IDE development environment.


## Running the demo files

Included in this repository are a number of ready-to-go **gcode files**
that you can run on the vMachine.  It is beyond the scope of this project
to teach you how to use Inkscape, or Flatcam, or Fusion 360, or any other
available program to create gcode files to feed to the vMachine. You
may use any of those, or any other program you wish, to create gcode
files to feed to the vMachine.

But, if you build the machine, and populate the SD card from the **sdCard folder**
in this repository, there are some gcode files you can use right away to
test it, verify it is working correctly, and even produce some nice
drawings.  You can also view the contents of these gcode files to see what
kind of gcode the vMachine expects so that you can produce it with
any of the above programs, or by hand, as necessary.

**VIDEO - Running the Demo Files**

The video above demonstrates the touch screen UI, the browser
based Web/Wifi UI, and the serial terminal user interface by running the following
"programs" (gcode files) via one of the user interfaces:

- **homing** - first we "home" the machine
- **center.g** - moves the sled to the center of the page
- **ruler.g** - starts at the center and prints a simple ruler pattern
- **draw_boxes.g** - draws boxes on the board itself around the work area and 12x9" and 11x8.5" paper sizes
- **/test8.5x11/test8.5x11.ngc** - a sophisticated test that draws rulers, shapes, a compass and a sailboat
on 11x8.5" paper

When you get your vMachine working, the first thing you will print will probably be
the **ruler.g**, and you can do that by just manually moving the sled to the approximate
center of the work area, before you get **homing** working.   Once you can **home**
the machine, and you setup the G55 and G56 coordinate systems, you can run the
**draw_boxes.g** gcode to draw the work area, and 12x9 and 11x8.5 alignment boxes
directly on the board.  Then you can tape a piece of (US letter) paper in the
11x8.5 alignment box and print the **test8.5x11.ngc** reference test sheet on that.


## Documentation

The documentation starts with this page, and includes the following additional
pages:

- **[History](docs/history.md)** - my personal historical perspectives and
photos of early prototypes and previous versions of the vMachine.
- **[Hardware](docs/hardware.md)** - details the physical
construction of the vMachine, including the 3D printing and mechanical parts.
- **[Electronics](docs/electronics.md)** - the schematics and home made circuit
boards for the machine.
- **[User Interface](docs/ui.md)** - a synopsis of the behavior of the
vMachine and it's generalized **FluidNC_UI** user interface.
- **[Software](docs/software.md)** - an overview of the software architecture
of the vMachine and FluidNC.
- **[Installation](docs/Installation.md)** - instructions on building and installing
the vMachine firmware onto the ESP32 using the Arduino IDE environment.


It is recommended that you read all these pages BEFORE constructing a vMachine.
In my opinion if you are going to build a machine from these sources, or part of them,
you should **START** with the **[Installation](docs/Installation.md)**
page and the first thing you should do is *build the firmware and install it*
to a bare ESP32 dev module with just a USB cable.

As part of *that* process it is required that you verify and and utilize the browser
based WiFi UI to the module and upload some files to the file system on the ESP32,
so with just the bare ESP32 module you can verify that the firmware is installed,
test the Web UI, and make sure that it runs more or less correctly BEFORE investing
any time in 3D printing or creating breadboards or printed circuit boards.

In any case, all of these pages, and this source, is provided in the hopes that
you will find it useful and/or interesting.


## License

I'm gonna have to do something here.  Yech, I hate those headers in source files.


## Credits

- To the [Maslow CNC](https://www.maslowcnc.com/) team for the kinematics.

- To **bdring** and the [FluidNC Team](https://github.com/bdring/FluidNC) for the core of this project.



<br>
<hr>
<div style="text-align: right">
<a href='docs/history.md'>NEXT</a><i> - more about the vMachine ...</i>
</div>
