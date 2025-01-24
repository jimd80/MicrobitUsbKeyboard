# Micro:bit to USB Keyboard interface
This project will allow you to connect a Micro:bit to a computer and send keystrokes to it. It's intended use is to create local multiplayer games in [Scratch](https://scratch.mit.edu/) up to 26 players. Each player has another Micro:bit as a wireless game controller (with only 1 active button). The receiver will simulate a USB keyboard that sends a keypress (from a to z) for each player that presses the button on their controller.

## Hardware setup
You'll need an Arduino (compatible) board with an ATmega32U4 microcontroller, such as an Arduino Micro, Arduino Leonardo or a Pro Micro. Pin 1 of the Micro:bit should be connected to the serial port Rxd pin of the controller.

Wiring diagrams for the Arduino Micro and the Pro Micro:

![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20to%20arduino%20micro%20connection.png?raw=true)
![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20to%20pro%20micro%20connection.png?raw=true)

## Arduino build instructions
The project can be build using VSCode with the PlatformIO addon. Alternatively, src/main.cpp can be renamed to .ino if you are using the Arduino IDE. You will need to add the external library arduino-libraries/Keyboard manually.

You could tweak the constants PLAYER_DELAY_MS, KEYPRESS_MIN_TIME_MS, KEYPRESS_MAX_TIME_MS and MAX_KEYS_SAME_TIME but it is strongly advised to leave then as-is. These values were carefully chosen to meet most types of games (either high volume clicker games, or time accurate "shooting" games).

Keep in mind that 26 controllers will generate a lot of keystrokes! The software is designed to handle a throughput of 26 controllers, each clicking 10 times/second, which is about the human limit of pressing a Micro:bit pushbutton. The USB protocol also have a limit of 6 keyboard keys pressed at the same time. Therefore, the minimum time between a KeyDown and KeyUp event is set to 20 ms. This allowes a total throughput of 300 keystrokes per second, which is about 11.5 clicks per second per controller.

The time before the same key can be sent again is set to 50 ms. This prevents cheating and buffer backlog floods. To protect even more from cheating, when the same key is received within 50 ms, the wait time will restart. This prevents a cheater from sending on a 30 ms interval which would still result in a 60ms interval. That is still faster than the human limit.

The keyboard layout is set to Belgium or France (azerty). If you use another keyboard layout, please change KB_LAYOUT

## Micro:bit receiver build instructions
If you have a specific project, you can use below code as a starting point. Setup a serial port with TX on P1, RX on P0 and a baud rate of 115200. Next, every character sent over the serial port will be converted to a simulated keypress on the computer.

![Micro:bit sample code](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20sample%20code.png?raw=true)

Or, you can use the default software below for a receiver that handles up to 26 senders.

Microsoft MakeCode [DojoGameReceiver project](https://makecode.microbit.org/S01281-32840-25827-62408)

Alternatively, download [microbit-DojoGameControllerUniversal.hex](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/microbit-DojoGameControllerUniversal.hex?raw=true) and drag&drop it to the Micro:bit drive letter.

### Usage of the DojoGameReceiver:
* Press A for a test of 50 keys/sec for 5 sec, each key 10 times
* Press B for a test of 250 keys/sec for 5 sec, each key 50 times

## Micro:bit game controller (sender) build instructions
You can include this step in the learning process and game experience. You can educate the participants to build their game controller first before they can play. Writing a program for the game controller Micro:bit is as simple as this:

![Micro:bit sample code](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20gamecontroller%20code.png?raw=true)

Each player needs a "secret code" that will identify them as player a...z. They'll need to enter this, instead of 9999, in the "radio send number" block. Using this secret code will make sure they will be assigned to their own letter and not to another one (due to a typo or intended trolling).

Download the [list of "secret" game codes](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Gamecodes.csv?raw=true) here as a text/csv file.

## Micro:bit game controller (sender) fast alternative

If there is not enough time to include the Micro:bit programming as a part of the game, there is also a ready to use program: [DojoGameControllerUniversal project](https://makecode.microbit.org/S17170-50597-42799-53089) or download [DojoGameControllerUniversal.hex](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/microbit-DojoGameControllerUniversal.hex?raw=true) and drag&drop it to the Micro:bit drive letter.

You can hand over the pre-programmed Micro:bits to the participants, but they'll still need to assign their personal "secret" game code to the Micro:bit. This allowes to use the exact same program for each controller. To set the game code, tell the participants to follow the steps below.
### Usage of the DojoGameControllerUniversal:
The Micro:bit will show a ? symbol. If your game code is 4567 for example:
* Press 4 times A
* Press 5 times B
* Press 6 times A
* Press 7 times B
* Press A to confirm.

If succeeded, the Micro:bit will show the player assigned letter (a..z) which can also be useful to identify your player character in the game, if no personalised names are used.

If not succeeded, press B and try again. Note that A and B can be swapped and the procedure will still work.

![Microbit gamecontroller universal](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20gamecontroller%20universal.png?raw=true)

## Scratch developer tips
The difficult part is to build code that **not** needs to be copy-pasted 26 times for each player. The most simple starting point makes use of cloning a sprite and can be like this:
![DojoGameHelper](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Scratch%20DojoGameHelper.png?raw=true)
Try it yourself in the [DojoGameHelp1](https://scratch.mit.edu/projects/1124514165/) Scratch project. This is the simplest approch, but it sometimes misses keystrokes unfortunately because of the polling technique. If you need 100% accuracy, it is better to work event driven so that the key press time doesn't matter. This event driven approch can be found in the [DojoGameHelp2](https://scratch.mit.edu/projects/1124514475/) project.

To see a complete game using another technique, check out this [A horse racing game](https://scratch.mit.edu/projects/1124514978/) (music is removed due to copyright reasons).

Note that you do not need any extra hardware to run or test the games, just press a..z (and spacebar to go to the next screen). **This makes it an ideal challenge for participants to create their own games at home, and play them together with all classmates on a big screen!**

## Additional resources
In classrooms, there is often not much time to waste. When playing with 26 players, it can be challenging to handle the storage, distribution and collection of the Micro:bits in a short time. Therefore, a 3D printable case is available to store 27 Micro:bits, the USB cable and the battery box together in a compact way. **It is strongly adviced to print this with a special ESD-safe filament!**

The STL file is available [here](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20case.stl?raw=true)

![STL File for storing 27 Micro:bit](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20case%20stl%20file.png?raw=true)

![Picture of the box in use](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20case%20printed%202.jpg?raw=true)

## Future developments
The creation of a circuit board is in progress. This PCB will contain a Micro:bit edge connector, the Arduino Micro socket and some extra buttons and LED's. These can be used to inhibit the keystrokes when not in the game. The PCB will also support the Pro Micro, a cheaper Chinese competitor for the Arduino Micro

![Interface PCB](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20arduino%20bridge%20pcb.png?raw=true)

To take it a step further, a game controller PCB is also being developed. This will save the time to connect and disconnect the battery box, and the USB cable can be stored on the back of the PCB around the battery, thanks to a 3D printed holder.

![Game Controller PCB](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Gamecontroller%20pcb%203d.png?raw=true)
