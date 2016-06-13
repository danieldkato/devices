#DEVICES

##OVERVIEW:
This repo contains code for controlling Arduino-managed devices for ArduFSM protcols like MultiSens. 


##DESCRIPTION
This library defines a number of devices classes, each of which corresponds to a type of device that might be controlled from an Arduino - e.g., steppers, speakers, solenoids, etc. 

Each class includes a `loop` function and an `s_finish` function. `loop` passes an integer index to a dispatch table that specifies how the device should behave throughout the duration of some stimulus period. For example, if a `Speaker` object is given an index of `0`, its `loop` function may do nothing throughout a stimulus period; if it is given an index of `1`, it may generate a white noise sitmulus throughout the duration of a stimulus period by randomly changing the tone it emits every time the main `ArduFSM` sketch performs its state-dependent operations. 

`s_finish` specifies how the device should behave at the end of a stimulus period. This includes things like turning off all speakers, retracting all steppers, etc. 

Because all of these device classes inherit from the same parent class (`Device`), all instances can be placed in an array, then some referring program (like a protocol's `States.cpp`) can iterate through the array and call each device's `loop` function throughout the duration of some stimulus period. This allows the ArduFSM program to rapidly update what each device should be doing for near-simultaneous control of multiple devices.    

The library currently consists of the following devices:

* `mySpeaker` controls a speaker. Its `loop` function uses the following index-behavior mappings:
  * `0`: do nothing
  * `1`: play a white noise stimulus
 
* `myStepper` controls a stepper motor. Its `loop` function uses the following index-behavior mappings:
  * `0`: do nothing
  * `1`: extend whisker stimulus 

*Note: These files were formerly located in the MultiSens protocol directory in the ArduFSM repo itself, but I decided that they might be useful for multiple protocols. Moreover, the MultiSens directory already contains a lot of files, so moving these files out declutters it a little.* 

*I could have moved these files to ArduFSM's libraries folder, but decided against it. The reason is that these files have to be in the libraries folder in the top-level Arduino sketchbook folder, which is NOT necessarily the same as the libraries folder in ArduFSM. The ArduFSM and Arduino sketchbook libraries happen to be identical for Chris, whose ArduFSM folder IS his Arduino sketchbook folder, but on most other systems, ArduFSM will be a subdirectory of the top-level Arduino sketchbook folder.* 

*Thus, even if these scripts were included in the ArduFSM libraries folder, one would have to remember to move them to the top-level Arduino libraries folder every time one pulled them from github anyway. It seems more straightforward to just have a devices repo saved in the main sketchbook library that can pull from github directly to the correct location.*

*I may ultimately decide, however, that the functionality included in this code is really specific to MultiSens after all, or that including functionality for all protocols that might use something like this would be too unwieldy and revert to having separate devices.h and devices.cpp files for each protocol that needs them.*


