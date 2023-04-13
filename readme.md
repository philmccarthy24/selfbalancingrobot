![logo](https://raw.githubusercontent.com/philmccarthy24/selfbalancingrobot/master/media/logo.png)

## Introduction

A project to explore how self balancing vehicles like the Segway / hoverboards work. This will be a long running spare-time project.

![first test](https://raw.githubusercontent.com/philmccarthy24/selfbalancingrobot/master/media/first_pid_test.jpg)

## Hardware spec

The robot has been built using:

- 3 layers of MDF, the bottom 18mm thick and the top two 9mm thick
- 4 12mm diameter hollow aluminium poles 50mm in length slotted and bolted through the middle sheet to give strength and stability while keeping the assembly relatively lightweight - fully built, the robot has XX g mass.
- 28 25mm steel braces and 16 M4 bolts/washers/nuts to secure the poles to the boards.
- 2 6.5 inch 36v rated wheel hub motors taken from a broken hoverhoard bought off ebay for £20, each secured with 4 M8 bolts, washers and wing nuts.

See the mechanical drawings in the docs/mechanical directory for further details (created with [OpenSCAD](https://openscad.org/)). Here's a photo of the assembly during motor/ahrs testing, showing how the frame is bolted together:

![motor tests](https://raw.githubusercontent.com/philmccarthy24/selfbalancingrobot/master/media/motor_tests.jpeg)

## Electronics

The motor driver runs on 36v from two 18v LIon batteries I had knocking about from a dead electric screwdriver. 

The controller runs off a separate 18650 LIon battery (to prevent ground loops via ODrive UART connection) via a 3.7v to 5v up-converter.

A Teensy 4.1 MCU runs the control firmware, and is connected to:

- a FXAS2100 Gyroscope IC over I2C bus
- a FXOS8700 Accelerometer/Magnetometer IC over I2C bus
- a 128x64 OLED display over I2C bus
- 3 rotary encoders for user interaction
- a 18-56v [ODrive 3.6 BLDC motor controller board](https://odriverobotics.com) over a serial link at 115200 baud

The sensor hardware containing the gyro/accelerometer/mag sensors is an [Adafruit NXP 9-DOF Breakout Board](https://www.adafruit.com/product/3463).

The wheel hub motors are driven by the ODrive at 36v.

## Firmware

Coded using Arduino IDE, the firmware separates concerns using a basic (synchronous) EventBus PUB/SUB design, to decouple the different elements and allow classes to house library setup boilerplate. 

In this way, the main .ino sketch is kept minimal. `UserInterface` contains a state machine that describes which mode the controller is in. `RotaryEncoder` objects raise position update or button pushed events which are received by the `UserInterface` object, and depending on state either change between screens or allow PID contants to be tweaked (possibly more settings can be managed in future). The main loop periodically fires a timer message at 100Hz, which is received by the `AHRS` object. The handler then reads gyro/accelerometer/magnetometer raw counts from the NXP sensor module, applies NXP/Kalman sensor fusion, and then publishes an AHRS update message on the bus.
`UserInterface` reads AHRS messages and displays data to the screen every 200ms (to avoid timings going out of sync).

`MotionController` also receives AHRS update messages, and uses the values in a PID controller to command the ODrive via serial link to apply current to the wheel motors to keep the robot upright (by moving the robot to keep the weight over the wheels).


## Project restart April 2023

Initially the project used a Raspberry Pi 4 to control the ODrive. I could not get the robot to balance, experiencing violent wheel oscillation motion and instability despite experimenting with lots of PID values. Eventually the ODrive stopped driving one of the motors after a firmware update and failed recalibration.

Several things were wrong with this approach:

- Using a full Linux computer to perform real time motion control; a dedicated MCU is a better choice and allows timings to be more accurate, free of thread scheduling and OS jitter
- Applying velocity commands to ODrive. Current control is better, proportional to torque and more likely to result in successful balance control
- Not having an immediate way to dynamically tweak PID values, resulting in long experimentation cycles. Rotary Encoders and a little OLED screen will save tons of time.

In early April 2023 I dusted off (literally) the robot, flashed the ODrive with the latest firmware, and tried recalibrating the hub motors from scratch. This time I was successful, probably due to updated firmware and instructions. I managed to create a simple sketch for a Teensy 4.1 to drive the wheels via the ODrive serial pins.

I'm now creating a minimal firmware to periodically take AHRS readings, and apply current to the ODrive via a PID controller.

---


## Progress as of May 2021

- Mechanical frame built.
- 36v (2 x 18v) batteries lashed to top panel with bungee cords, and connected via spade terminals to power bus.
- On/off power rocker switch installed.
- 36v to 5v DC down converter connected to power bus and wired to USB-C cable for Pi4 power.
- Odrive board and wheels installed, wiring complete.
- Pi4 with AHRS breakout installed, wiring complete.
- Basic motor calibration done, tested moving hub motors using serial commands issued from Pi4 daemon. *Optimisation with odrivetool required*
- Attached impact lances to frame, to cushion robot during the rather violent oscillations of PID tuning
- Daemon software 95% implemented
  - CMake build system
  - Factories
  - Logging via spdlog library
  - Unit tests via google test library
  - Performance unit tests written to test AHRS at rest and drive motors via ODrive
  - AHRS system to determine orientation in 3D space
  - Gyroscope, accelerometer and magnetometer sensor calibration tool finished, and used to calibrate sensors in-situ after installation on chassis
  - Motor controller api
  - PID controller
- UI software barely started
  - Non-TS React interface started
  - Tested against a fake REST controller served by Node.js
  
#### TO DO:

- Write gRPC service in daemon to support remote drive control
- Complete React UI
- Complete tuning PID controller to make the robot stay up. 

## License/legal

Copyright (c) 2021 Phil McCarthy

Provided under the [MIT License](https://github.com/philmccarthy24/selfbalancingrobot/blob/8f1a7e7f0787cc03bbfced4b563601a9d68a3980/LICENSE).

The software uses the following open source libraries, with thanks and appreciation to their maintainers:

- [Bosch Power4All adapter design](https://github.com/usedbytes/power4all) base part Copyright 2020 Brian Starkey
- Adafruit SSD1306 and dependencies (OLED)
- Encoder (rotary encoders)
- Afafruit_AHRS (with FXOS/FXAS deps)
- [ODrive Arduino](https://github.com/odriverobotics/ODrive/tree/master/Arduino/ODriveArduino)
