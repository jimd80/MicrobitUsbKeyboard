# Micro:bit to USB Keyboard interface

This project will allow you to connect a Micro:bit to a computer and send keystrokes to it.

## Hardware setup

You'll need a board with an ATmega32U4 microcontroller, such as an Arduino Micro, Arduino Leonardo or a Pro Micro. Pin 1 of the Micro:bit should be connected to the serial port Rxd pin of the controller.

Wiring for the Arduino Micro

![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20to%20arduino%20micro%20connection.png?raw=true)

## Building

The project can be build using VSCode with the PlatformIO addon. Alternatively, src/main.cpp can be renamed to .ino if you are using the Arduino IDE. You will need to add the external library arduino-libraries/Keyboard manually.

You could tweak the constants PLAYER_DELAY_MS, KEYPRESS_TIME_MS and MAX_KEYS_SAME_TIME but it is strongly advised to leave then as-is. These values were carefully chosen to meet most types of games (either high volume clicker games, to time accurate "shooting" games).

Keep in mind that 26 controllers will generate a lot of keystrokes! The software is designed to handle a troughput of 26 controllers, each clicking 10 times/second, which is about the human limit of pressing a button as found on the Micro:but. The USB protocol also have a limit of 6 keyboard keys pressed at the same time. Therefore, the time between a KeyDown and KeyUp event is set to 20ms. This allowes a total troughput of 300 keystrokes per second, which is about 11.5 clicks per second per controller.

The time before the same key can be pressed again is set to 50ms. This prevents cheating and flooding in case of a buffer flood. To protect even more from cheating, when the same key is received within 50ms, the counter will start aging. This prevents that a cheater who sends a key each 30ms would still profit from a 60ms interval, which is still faster than the human limit.

The keyboard layout is set to Belgium or France (azerty). If you use another keyboard layout, please change KB_LAYOUT


## Example of usage

The project was initially intented to create local multiplayer games in Scratch (programming language)

[A horse racing game in Scratch](https://scratch.mit.edu/projects/1110965589/)

## Additional resources

In classrooms, there is often not much time to waste. When playing with 26 players, it can be challenging to handle the storage, distribution and collection of the Micro:bits in a short time. Therefore, a 3D printable case is available to store 27 Micro:bits, the USB cable and the battery box in a compact way.

The STL file is available [here](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20case.stl?raw=true)
![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20case%20stl%20file.png?raw=true)
![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20case%20printed%202.png?raw=true)

## Future developments

The creation of a circuit board is in progress. This PCB will contain a Micro:bit edge connector, the Arduino Micro socket and some extra buttons and LED's. These can be used to inhibit the keystrokes when not in the game. The PCB will also support the Pro Micro, a cheaper Chinese competitor for the Arduino Micro

![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20arduino%20bridge%20pcb.png?raw=true)

To take it a step further, a Game controller PCB is also being developed. This will save the time to connect and disconnect the battery box, and the USC cable can be stored on the back of the PCB around the battery, thanks to a 3D printed holder.

![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Gamecontroller%20pcb%203d.png?raw=true)
