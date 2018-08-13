# GPS & ACC DISTANCE MEASUREMENTS
A comparative study between GPS and Accelerometer-Gyroscope system for distance measures.

link to Thesis: 
https://drive.google.com/open?id=1GK0hkd7NWBRNkAMQzP-KpFczLosS8j8f

# Introduction:

This is a real time MCU system that measures the distance walked from point A to point B. This is done separately by  GPS (Venus GPS) and a Pedestrian Dead Reckoning algorithm that utilizes an accelerometer-gyroscope system (MPU-6050) for input. Two Atmega32A are being used for each sensor because of the high computing requirements, after a certain amount of samples the microcontroller connected to the MPU-6050 sends the current measured distance to the other Atmega32A. In doing so, both samples can be sent simultaneously from one MCU  to a PC via serial protocol for evaluation. 
 

# How to use:

PCB circuit and Schematic are provided in the links bellow, the MCUs can be flashed onboard with a JTAG programmer.
You can use AVR Studio in order to compile and upload the hex files. 
    Because of the PDR algorithm, it is necessary to place the device on the users foot, otherwise the measured samples will be inaccurate.
    When the circuit is connected to a power source wait for the GPS to acquire a satellite fix and press the record button when ready, if the measurement is complete hit the button again.


# SCHEMATIC:
https://drive.google.com/open?id=19yx1MFBCep7U_gX3Mb2y6pwL9v6VX0p8

# PCB:

https://drive.google.com/open?id=1tAWQ91JfIdx2fT2Nb_cEq7Htz_8Notzv
