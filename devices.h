/*  Last updated DDK 6/7/16
 * 
 * OVERVIEW:
 * This is the header file for the library `devices`, used for controlling
 * Arduino-managed devices for `ArduFSM` protocols like `MultiSens`.
 *
 *
 * REQUIREMENTS: 
 * This file should be in the same directory as its source file `devices.cpp`. The 
 * directory must be saved in the Arduino sketchbook's library folder. In 
 * addition, the Arduino sketchbook's library folder should contain the following
 * libraries:
 * 
 * * `Arduino`
 * * `Stepper`
 * * `Tone`
 */

#ifndef DEVS_MS_INCLUDED
#define DEVS_MS_INCLUDED

#include "Arduino.h"
#include <Stepper.h>

#define FULLSTP_PER_ROTATION 200

//Parent device declarations*******************************
class Device{
    public:
        Device();
        virtual void loop( int fcnIdx ) =0;
        virtual void s_finish() =0;
    
        virtual void ping() =0;
        //virtual void ping();
        //void ping();
  
    private:
        static int deviceCounter;
        static String devBaseName;
        String devID;
};


//Dummy device declarations********************************

//Dummy stepper declarations
class dummyStepper: public Device {
    public:
        dummyStepper();
        void loop( int fcnIdx );
        void s_finish();
        void ping();
        String stprState; //a flag to keep track of whether the stepper is extended or retracted
};


//Dummy speaker declarations
class dummySpeaker: public Device {
    public:
        void loop( int fcnIdx );
        void s_finish();
        void ping();
};


//Real device declarations*********************************

// Speaker declarations
class mySpeaker: public Device{
    public:
        mySpeaker( int pin );
        void loop( int fcnIdx );
        void s_finish();
        void ping();
  
    private:
	int _pin;
};


/* myStepper declarations; this is the standard Arduino 
stepper library plus my standard updateDev function, some
higher-level "extend" and "retract" function and related
variables, and hall effect sensor variables. */
class myStepper: public Device {
    public:
        //constructor
        myStepper(int spkrPin, int dirPin, int hallPin, int hallThresh, int hallVal, int stepHalfDelay, int microstep, int reverseRotDeg);

        /*DK 151208: my update function that I need to define 
            in every device*/
        void loop( int fcnIdx );
        void s_finish();

        /*DK 151208: ping function just to check that device
            has been instantiated and main sketch can call 
            functions*/
        void ping();

        /*DK 151208: Flag for keeping track of whether stepper 
            is extended or retracted */
        String stprState;
    
        // speed setter method:
        void setSpeed(long whatSpeed);
    
        // mover method:
        void step(int number_of_steps);
	void rotate_one_step();
    
        int version(void);

    
    private:
        void stepMotor(int this_step);
    
        /*DK 151208: my extend and retract functions (taken 
    from  Clay) that will be called from the updateDev 
    function */
        void fwd();
        void back();

	int stpPin;
	int dirPin;
	int stepHalfDelay;
	int microstep;
	int reverseRotDeg;
	int numReverseSteps;
    
    //DK 151208: hall effect sensor variables 
        int enbl;
        int hallPin;
        int hallThresh;
        int hallVal;
        int stprSpeed;
        int CW;
        int CCW;

        int direction;            // Direction of rotation
        unsigned long step_delay; // delay between steps, in ms, based on speed
        int number_of_steps;      // total number of steps this motor can take
        int pin_count;            // how many pins are in use.
        int step_number;          // which step the motor is on

        // motor pin numbers:
        int motor_pin_1;
        int motor_pin_2;
        int motor_pin_3;
        int motor_pin_4;
        int motor_pin_5;          // Only 5 phase motor

        unsigned long last_step_time; // time stamp in us of when the last step was taken
};


/*General utility function for reading data from serial
one line at a time (as opposed to the default one byte
at a time)*/
String getLine();

#endif
