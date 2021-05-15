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

The powertrain is 36v from two 18v LIon batteries I had knocking about from a dead electric screwdriver.

A Raspberry Pi 4 SBC is powered off this circuit via a 5v down-converter, and runs the control software. Connected to the Pi are:

- a FXAS2100 Gyroscope IC over I2C bus
- a FXOS8700 Accelerometer/Magnetometer IC over I2C bus
- a 18-56v [ODrive 3.6 BLDC motor controller board](https://odriverobotics.com) over a ground-loop isolated USB/serial link at 115200 baud

The sensor hardware containing the gyro/accelerometer/mag sensors is an [Adafruit NXP 9-DOF Breakout Board](https://www.adafruit.com/product/3463).

The wheel hub motors are driven by the ODrive at 36v.

## Software

The development system runs Raspian Buster. Eventually I'd like to use a mostly read-only FS to prevent SD card wear and protect against corruption when the power is shut off suddenly.

The main functional components are:

- **sbrcontrollerd** - a Linux daemon written in C++ that manages the robot's hardware. It reads raw sensor data from the AHRS hardware in a near-realtime (highest scheduling priority) loop at 100Hz, filters it through the Madgwick fusion algorithm to yield physical orientation (roll/pitch/yaw), and controls the wheel hub motors' velocities/torque via a PID controller to maintain unit balance.
- **sbrui** - a Typescript React web UI served from Node.js, allowing the robot to be controlled remotely over WiFi, and for users to update settings easily.

gRPC is used to send data between the web application and the backend control daemon.

Will provide some UML diagrams of detailed design if I get time.

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
  
#####TO DO:

- Write gRPC service in daemon to support remote drive control
- Complete React UI
- Fix M1 wheel hub erroring out in controller when M0 and M1 wheels are driven under load. Probably soldering 47nF capacitors onto M1 hall sensor pins of ODrive will fix as per ODrive forum recommendations
- Complete tuning PID controller to make the robot stay up. 
- 3D print Bosch Power4All mounts to more securely hold batteries against fall impact (https://github.com/usedbytes/power4all looks a good resource for a base model)

## License/legal

Copyright (c) 2021 Phil McCarthy

Provided under the [MIT License](https://github.com/philmccarthy24/selfbalancingrobot/blob/8f1a7e7f0787cc03bbfced4b563601a9d68a3980/LICENSE).

The software uses the following open source libraries, with thanks and appreciation to their maintainers:

- [nlohmann Json](https://github.com/nlohmann/json) Copyright (c) 2013-2021 Niels Lohmann
- [fmt](https://github.com/fmtlib/fmt) Copyright (c) 2012 - present, Victor Zverovich
- [spdlog](https://github.com/gabime/spdlog) Copyright (c) 2016 Gabi Melman.
- [Madgwick sensor fusion algorithm](https://x-io.co.uk/open-source-imu-and-ahrs-algorithms/) Copyright (c) 2009-2021 Sebastian Madgwick
- [Google Test](https://github.com/google/googletest) Copyright 2008, Google Inc. All rights reserved
