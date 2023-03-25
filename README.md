# Communicate-3-MSP430G2553-with-HC-05-bluetooth-module

This is a project for communicating between three MSP430G2553 microcontrollers using the HC-05 Bluetooth module. 

## Project Overview

The project consists of three MSP430G2553 microcontrollers connected to HC-05 Bluetooth modules. One microcontroller is designated as the master, and the other two are slaves. The master microcontroller sends commands to the slaves, and the slaves respond with their status. The status of each slave is indicated by an LED on the corresponding microcontroller.

## Usage

### Hardware Setup

The following hardware is required for this project:

- Three MSP430G2553 microcontrollers
- Three HC-05 Bluetooth modules
- Look to altium file for other components (LED, Res, Cap, etc)

The hardware should be set up as shown in the altium file

### Software Setup

To set up the software for this project, you will need to install IAR Studio (Or CCS) and the MSP430G2553 driver. 
Once IAR Studio is installed, open the project in IAR Studio and build the code for each microcontroller. 
You need to change the code for your hc-05 devices. Master device should know the slave device's MAC adresses.

### Usage

Once the hardware and software are set up, turn on the microcontrollers and they will pair them with the master HC-05 automaticaly. 
The slave microcontrollers will respond with their status. If the LED is on, the status will be `on`. If the LED is off, the status will be `off`. Other leds will
be on or off respect to the sensors status.


    FOR DETAILED INFORMATION READ THE RAPOR.
