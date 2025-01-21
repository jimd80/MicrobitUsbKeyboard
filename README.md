#Micro:bit to USB Keyboard interface

This project will allow you to connect a Micro:but to a computer and send keystrokes to it.

##Hardware setup

You'll need a board with an ATmega32U4 microcontroller, such as an Arduino Micro, Arduino Leonardo or a Pro Micro. Pin 1 of the Micro:bit should be connected to the serial port Rxd pin of the controller.

Wiring for the Arduino Micro

![Wiring diagram](https://github.com/jimd80/MicrobitUsbKeyboard/blob/main/doc/Microbit%20to%20arduino%20micro%20connection.png?raw=true)

##Example of usage

The project was initially intented to create local multiplayer games in Scratch (programming language)

[A horse racing game in Scratch](https://scratch.mit.edu/projects/1110965589/)

##Future developments

The creation of a circuit board is in progress. This PCB will contain a Micro:bit edge connector, the Arduino Micro socket and some extra buttons and LED's. These can be used to inhibit the keystrokes when not in the game. The PCB will also support the Pro Micro, a cheaper Chinese competitor for the Arduino Micro

In classrooms, there is often not much time to waste. When playing with 26 players, it can be challenging to handle the storage, distribution and collection of the Micro:bits in a short time. Therefore, a 3D printable case is available to store 27 Micro:bits, the USB cable and the battery box in a compact way.

To take it a step further, a Game controller PCB is also being developed. This will save the time to connect and disconnect the battery box, and the USC cable can be stored on the back of the PCB around the battery, thanks to a 3D printed holder.