# Hairless MIDI<->Serial Bridge

Hairless MIDI<->Serial Bridge is the easiest way to connect serial devices (like Arduinos) to send and receive MIDI signals. This is a fork of the [original project](http://projectgus.github.com/hairless-midiserial/) which adds additional functionality.

## Things added by this [fork](eringerli/hairless-midiserial)

This is a relatively quick hack to have a scriptable MIDI bridge, so the code quality is not as good as it can be and the commit messages are quite terse. But it works reliably, so everything's well which ends well :wink:

My motivation was to connect a Traktor F1 controller as a controll pendant for the lightning mode in rekordbox. The F1 doesn't support native pages in MIDI mode (the smarts for all the fancy stuff are apparently in Traktor), and the MIDI endpoint/support in rekordbox is quite rudimentery (to say the least: some pads light up after activating a state, others don't, however that was programmed on their end and slipped through testing), so I needed an additional smart interface between the two.

### Lua

I added a Lua interpreter as a third pilar in the bridge (to keep the image), to allow for easy scripting. I searched a long time for a simple, free and scriptable MIDI bridge to do stuff like pages and feedback, even if the receiving software doesn't implement it and to do more complex stuff than simple 1:1 conversions between messages (which midiOx can do). With Lua scripts it is possible to have persistent state between messages, like activating pages and change the messages for the pads accordingly. So you can transform messages in more complex ways, pe generate steepped tone-on/tone-off from faders, vice versa or even implement push-to-increment pad-to-fader-conversions. Also you have the freedom to implement your own feedback, like light up pads with your own colors, if your controller supports it (most of them do, but you have to send a lot of different messages to access that feature).

The lua interpreter is built with full support for all bundled libraries, so you can do almost everything you want with it. No precautions are taken to secure in in any way (except that it doesn't crash if you load an syntactic invalid file), so you can shoot yourself quite comfortably in your foot with it.

To use it, you can implement the functions ```onMessageLeft(data)``` and ```onMessageRight(data)``` in your script (`data` is a simple array with the whole MIDI message as `std::vector<uint8_t>`), sending stuff out is possible with ```sendMessageRight(data)``` and ```sendMessageLeft(data)```. If you don't implement the two `onMessage...`-functions, the data is simply piped through. Lua is flexible, really fast and quite powerfull, so having presistent states (like pages or colorfull feedback on the pads) is now possible. Scripting in Lua for this easy stuff is simple, just use your search engine of choice to search for a tutorial.

And don't ever think about any performace hit, with todays multi core, multi GHz computers, you can't notice any delay even having a whole scripting engine running and piping the data through a couple of different drivers before the target software. Or at least I don't.

### Changes

- The bridge can now also bridge between two MIDI interfaces, pe a controller and a loopback to feed back into other software/hardware. Serial to serial is also possible.
- The tracing is done with a QTextView, which is not only a lot simpler and faster, but allows for colored entries, copy/paste and autoscrolling. Every 'pilar' can be fully traced.
- Everything is saved on modification and loads back to the last state if you close and reopen the program, but you have to activate the different parts (MIDI-interfaces, Lua-interpreter) yourself.
- A lot of code changes to get this working
- Works with the newest QT5 version, tested with 5.15.2.
- Building should be simple: clone it recursively and open it up in QtCreator.

### Things I didn't test/don't work

- I don't have any serial MIDI devices. Should work, I kept the old code as good as possible.
- I don't have a Mac, so using it there is your thing. Should work.
- The console stuff added by rhargreaves isn't tested (and is likely not working). As I want to use it on windows, where it doesn't make much sense. No comparison between an CMD.EXE window and a proper bash console...
- I removed the EverDrive Pro stuff, as I don't need that

### Traktor F1 + Rekordbox lightning mode

You can't connect the whole thing trivially like Traktor F1 <=> Lua <=> virtual MIDI port, Rekordboxs feedback creates a loop. You have to split the MIDI in and out to two virtual ports. To do it, follow these steps: 

1. Install [LoopMidi](https://www.tobias-erichsen.de/software/loopmidi.html) and create two seperate ports
1. Install Native Instruments [Controller Editor](https://www.native-instruments.com/en/support/downloads/drivers-other-files/) and load the template ```template.nckf1```
1. Compile and run this project
1. Set the left side to the Traktor F1 in/out, the middle to the script ```traktorF1.lua``` and the right side to each of the two created virtual MIDI ports
1. Open Rekordbox and then the MIDI editor. On the virtual port configured as input on the right side, import the file ```loopMIDI Port out.csv```, on the other the other file
1. Enjoy having a really cool lightning controller

## Things added by the last [fork](rhargreaves/hairless-midiserial)

### Command-line support

- Command-line options to set debug mode, serial device and MIDI ports.
- Output messages to console.
- Ability to hide the GUI.

#### Command-line options

```sh
$ ./hairless-midiserial -h
Usage: ./hairless-midiserial [options]
Sends and receives MIDI events over a serial device.

Options:
  -h, --help                 Displays help on commandline options.
  --help-all                 Displays help including Qt specific options.
  -v, --version              Displays version information.
  -d, --debug                Show debug MIDI messages
  -c, --console-output       Output messages to the console
  -w, --hide-window          Hides the GUI
  -s, --serial <serial>      Specify the name of the serial device to use
  -i, --midi-in <midi-in>    Specify the name of MIDI in device to use
  -o, --midi-out <midi-out>  Specify the name of MIDI out device to use
```

For example:

```
$ ./hairless-midiserial -w -c -d \
    -s /dev/cu.usbserial-A105J2JO \
    -o "IAC Driver IAC Bus 1" \
    -i "IAC Driver IAC Bus 2"
Opening serial port '/dev/cu.usbserial-A105J2JO'...
Opening MIDI Out port #0
Opening MIDI In port #1
```

### Compiled with newer Qt

- Dark theme support on macOS, higher resolution window

## Building Hairless Bridge from source

Hairless uses git submodules for library dependencies, so you should use `git clone --recursive URL` when cloning from Github. Alternatively, you can run `git submodule update --init` in order to fetch the submodules to an already-cloned directory.

Hairless Midiserial Bridge release 0.5 was built with Qt 5.14.

The Qt package should contain all dependencies, the graphical IDE "Qt Creator" or the program "qmake" can be used to compile the project hairless-midiserial.pro.

On Windows I recommend building with the [MingGW compiler](http://www.mingw.org/), Visual Studio has not been tested. Neither the MinGW site nor Qt's new owners Digia still distribute older MinGW builds, and MinGW 4.7 is too new for precompiled Qt 4.7.x, so it can be a bit hard to find a prebuilt combination that work. Recently I downloaded `mingw-static-4.4.5-all.7z` from [this Google Code project](https://code.google.com/p/qp-gcc/downloads/list), and can confirm that works.

(For the Windows release builds I actually [cross-build under Linux using wine, as described here](http://projectgus.com/2011/09/developing-qt-apps-for-windows-using-linux-wine/).)

# Libraries

- [qextserialport](https://code.google.com/p/qextserialport/) is hosted on Github and is linked into the source tree as a git submodule.
- [The RtMidi library](https://github.com/thestk/rtmidi) is hosted on Github and is linked into the source tree as a git submodule.

Both libraries are small so they compiled as source files directly into Hairless Bridge, not linked as libraries.

# Release builds

The official releases are static linked, so they've actually been built against Qt versions that were compiled from source, configured with `-static` for static linking.
