*Last updated DDK 6/7/13*

#DEVICES


##OVERVIEW:
This repo contains code for controlling Arduino-managed devices for `ArduFSM` protcols like `MultiSens`. 


##REQUIREMENTS:
This directory should consist of the header file `devices.h` and the source file `devices.cpp`.

In addition, the Arduino sketchbook library folder should contain the following librari
es:

* `Arduino`
* `Stepper`
* `Tone`


##DESCRIPTION:
This library defines a number of devices classes, each of which corresponds to a type of device that might be controlled from an Arduino - e.g., steppers, speakers, solenoids, etc. 

Each class includes a `loop` function and an `s_finish` function. `loop` passes an integer index to a dispatch table that specifies some action the device should perform. For example, if a `Speaker` object is given an index of `0`, its `loop` function may do nothing; if it is given an index of `1`, it may generate a random tone. 

`s_finish` specifies how the device should behave at the end of a stimulus period. This includes things like turning off all speakers, retracting all steppers, etc. 

Because all of these device classes inherit from the same parent class (`Device`), all instances can be placed in an array, then some referring program can iterate through the array and call each device's `loop` function throughout the duration of some stimulus period. In practice, the `MultiSens` protocol's `States.cpp` defines a stimulus-period object that iterates through such an array throughout the duration of the stimulus period. The `ArduFSM` program can thus update what each device should be doing on every pass of its state-dependent operations, allowing for near-simultaneous control of multiple devices throughout the duration of some stimulus period.    

The library currently consists of the following devices:

* `mySpeaker` controls a speaker. Its `loop` function uses the following index-behavior mappings:
  * `0`: do nothing
  * `1`: play a white noise stimulus
 
* `myStepper` controls a stepper motor. Its `loop` function uses the following index-behavior mappings:
  * `0`: do nothing
  * `1`: extend whisker stimulus 





