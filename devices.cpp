/*  Last updated DDK 6/7/16
 * 
 * OVERVIEW:
 * This file contains source code for the library `devices`, used for controlling
 * Arduino-managed devices for `ArduFSM` protocols like `MultiSens`.
 *
 *
 * REQUIREMENTS: 
 * This file should be in the same directory as its header file `devices.h`. The 
 * directory must be saved in the Arduino sketchbook's library folder. In 
 * addition, the Arduino sketchbook's library folder should contain the following
 * libraries:
 * 
 * * `Arduino`
 * * `Stepper`
 * * `Tone`
 * 
 * 
 * DESCRIPTION:
 * This library defines a number of devices classes, each of which 
 * corresponds to a type of device that might be controlled from 
 * an Arduino - e.g., steppers, speakers, solenoids, etc. 
 * 
 * Each class includes a `loop` function and an `s_finish` function. 
 * `loop` passes an integer index to a dispatch table that specifies 
 * some action the device should perform. For example, if a `Speaker` 
 * object is given an index of `0`, its `loop` function may do nothing; 
 * if it is given an index of `1`, it may generate a random tone. 
 *
 * `s_finish` specifies how the device should behave at the end of a 
 * stimulus period. This includes things like turning off all speakers, 
 * retracting all steppers, etc. 
 * 
 * Because all of these device classes inherit from the same parent class 
 * (`Device`), all instances can be placed in an array, then some referring 
 * program can iterate through the array and call each device's `loop` function 
 * throughout the duration of some stimulus period. In practice, the `MultiSens` 
 * protocol's `States.cpp` defines a stimulus-period object that iterates through 
 * such an array throughout the duration of the stimulus period. The `ArduFSM` 
 * program can thus update what each device should be doing on every pass of its 
 * state-dependent operations, allowing for near-simultaneous control of multiple 
 * devices throughout the duration of some stimulus period.  
 * 
 * For specific index-behavior mappings, see this library's `README.md` file.
 */

#include "Arduino.h"
#include "devices.h"


// Parent Device class definitions*************************
Device::Device(){
  deviceCounter++;
}

//Dummy stepper class definitions**************************

//constructor
dummyStepper::dummyStepper(){
  this->stprState = "RETRACTED"; //initialize flag to "RETRACTED"
}

//Standard updateDev function required for all device classes:
void dummyStepper::loop( int fcnIdx ){

    //Dummy stepper action 1: do nothing
    if ( fcnIdx == 0){/*does nothing*/}

    //Dummy stepper action 2: extend at beginning of stimulus epoch
    else if ( fcnIdx == 1){
      if (this->stprState == "RETRACTED"){
        this->stprState = "EXTENDED"; 
      }
      Serial.println( "I extended a stepper!" );
    }
}

void dummyStepper::s_finish(){this->stprState = "RETRACTED";}

//Dummy speaker class defintions***************************

//Standard updateDev function required for all device classes:
void dummySpeaker::loop( int fcnIdx ){

    //Dummy speaker action 1:
    if ( fcnIdx == 0 ){/*do nothing*/}

    //Dummy speaker action 2:
    else if ( fcnIdx == 1){
      Serial.println( "I'm playing a tone!" );
    }
}

void dummySpeaker::s_finish(){}

//Speaker class defintions**********************************

//constructor
mySpeaker::mySpeaker( int pin ):_pin(pin){
	pinMode( _pin, OUTPUT ); //configure speaker pin for output
}

//Standard updateDev function required for all device classes:
void mySpeaker::loop( int fcnIdx ){

  //mySpeaker action 1: do nothing
  if( fcnIdx == 0 ){/*do nothing*/}

  //mySpeaker action 2: play a tone
  else if ( fcnIdx == 1 ){
    tone(_pin,random(10000,20000));
  }
}

void mySpeaker::s_finish(){
    noTone(_pin);
}

//myStepper class definitions*********************************
/* This is the standard Arduino stepper library plus my 
standard updateDev function, some higher-level "extend" 
and "retract" function and related variables, and hall 
effect sensor variables. */

//alternate constructor for stepper object using stepper motor driver
myStepper::myStepper(int stpPin, int dirPin, int hallPin, int hallThresh, int hallVal, int stepHalfdelay, int microstep, int reverseRotDeg)
{
	this->stpPin = stpPin;
	this->dirPin = dirPin;
	this->hallPin = hallPin;
	this->hallThresh = hallThresh;
	this->hallVal = hallVal;
	this->stepHalfDelay = stepHalfDelay; 
	this->microstep = microstep;
	this->reverseRotDeg = reverseRotDeg;
	this->stprState = "RETRACTED";
	this->numReverseSteps = floor( (reverseRotDeg/360.0) * FULLSTP_PER_ROTATION) * microstep;
	
	pinMode(this->stpPin, OUTPUT);
	pinMode(this->dirPin, OUTPUT);
}

void myStepper::rotate_one_step()
{
	digitalWrite(stpPin, HIGH);
	//PORTD |=_BV(PD6);
	delayMicroseconds( stepHalfDelay / microstep );
	digitalWrite(stpPin, LOW);
	//PORTD &= ~_BV(PD6);
	delayMicroseconds( stepHalfDelay / microstep );
}


//constructor
/*
myStepper::myStepper(int number_of_steps, int motor_pin_1, int motor_pin_2, int enbl, int hallPin, int hallThresh,  int stprSpeed, int CW, int CCW, int hallVal )
{
	this->step_number = 0;    // which step the motor is on
	this->direction = 0;      // motor direction
	this->last_step_time = 0; // time stamp in us of the last step taken
	this->number_of_steps = number_of_steps; // total number of steps for this motor

	// Arduino pins for the motor control connection:
	this->motor_pin_1 = motor_pin_1;
	this->motor_pin_2 = motor_pin_2;

	// setup the pins on the microcontroller:
	pinMode(this->motor_pin_1, OUTPUT);
	pinMode(this->motor_pin_2, OUTPUT);

	// When there are only 2 pins, set the others to 0:
	this->motor_pin_3 = 0;
	this->motor_pin_4 = 0;
	this->motor_pin_5 = 0;

	// pin_count is used by the stepMotor() method:
	this->pin_count = 2;

	//** DK 151208: initialize ancillary variables I've defined to manage interaction with the hall effect sensor, etc.
	this->enbl = enbl;
	this->hallPin = hallPin;
	this->hallThresh = hallThresh;
	this->hallVal = hallVal;
	this->stprSpeed;
	this->CW = CW;
	this->CCW = CCW;
	this->stprState = "RETRACTED";
	
	pinMode(this->enbl,OUTPUT);
	pinMode(this->hallPin,INPUT);
	
	this->setSpeed(stprSpeed);
}
*/

/*DK 151208: Standard updateDev function required for all device 
classes:*/
void myStepper::loop( int fcnIdx ){
  
  //myStepper action 1: do nothing
  if ( fcnIdx == 0 ){/*do nothing*/}
   
  //myStepper action 2: extend stepper if stepper is retracted
  else if ( fcnIdx == 1 ){
    if ( this->stprState == "RETRACTED" ){ 
      fwd(); 
      stprState = "EXTENDED";
    }
  }
}

void myStepper::s_finish(){ 
  if (this->stprState == "EXTENDED"){
	this -> back(); 
	//this -> stprState = "RETRACTED";
  }
}

/*
/*DK 151208: high-level function for extending stepper 100 steps 
with Hall effect sensor feedback in a single discrete function 
call (taken from Clay). */
/*
void myStepper::fwd(){
  digitalWrite( this->enbl, HIGH);
  delay(100);
  this->hallVal = analogRead( this->hallPin );
  while( this->hallVal > this->hallThresh ){
    this -> step( 1 );
    delay( 1 );
    this->hallVal = analogRead( this->hallPin );
  }
  Serial.println("stepper extended");
  this->stprState = "EXTENDED";
}
*/

// Alternative version of myStepper:fwd that uses rotate_one_step in order to accommodate use of the stepper motor driver

void myStepper::fwd(){
	digitalWrite(dirPin, HIGH);
	//this->hallVal = analogRead(hallPin);
	while(analogRead(hallPin)>hallThresh){
		rotate_one_step(); //how to deal with direction??
		delay(1);
		//hallVal = analogRead(hallPin);
	}
	Serial.println("stepper extended");
	stprState = "EXTENDED";
}


/*
void myStepper::fwd(){
	digitalWrite(dirPin, HIGH);
	//this->hallVal = analogRead(hallPin);
	for(int i = 0; i < numReverseSteps; i++){rotate_one_step();delay(1);}
	Serial.println("stepper extended");
	stprState = "EXTENDED";
}
*/

void myStepper::back(){
	digitalWrite(dirPin, LOW);
	//delay(10);
	for(int i = 0; i < numReverseSteps; i++){rotate_one_step(); delay(1);}
	Serial.println("stepper retracted");
	stprState = "RETRACTED";
}


/*DK 151208: high-level function for fully retracting stepper in a 
single discrete function call (taken from Clay). */
/*
void myStepper::back(){
  this -> step( this-> CCW );
  delay(200);
  digitalWrite( this->enbl, LOW );
  this->stprState = "RETRACTED";
}
*/

//Sets the speed in revs per minute
void myStepper::setSpeed(long whatSpeed)
{
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / whatSpeed;
}


//Moves the motor steps_to_move steps.  If the number is negative, the motor moves in the reverse direction.
void myStepper::step(int steps_to_move)
{
  int steps_left = abs(steps_to_move);  // how many steps to take

  // determine direction based on whether steps_to_mode is + or -:
  if (steps_to_move > 0) { this->direction = 1; }
  if (steps_to_move < 0) { this->direction = 0; }


  // decrement the number of steps, moving one step each time:
  while (steps_left > 0)
  {
    unsigned long now = micros();
    // move only if the appropriate delay has passed:
    if (now - this->last_step_time >= this->step_delay)
    {
      // get the timeStamp of when you stepped:
      this->last_step_time = now;
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction == 1)
      {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      }
      else
      {
        if (this->step_number == 0) {
          this->step_number = this->number_of_steps;
        }
        this->step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, ..., {3 or 10}
      if (this->pin_count == 5)
        stepMotor(this->step_number % 10);
      else
        stepMotor(this->step_number % 4);
    }
  }
}


//Moves the motor forward or backwards.
void myStepper::stepMotor(int thisStep)
{
  if (this->pin_count == 2) {
    switch (thisStep) {
      case 0:  // 01
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
      break;
      case 1:  // 11
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, HIGH);
      break;
      case 2:  // 10
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
      break;
      case 3:  // 00
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, LOW);
      break;
    }
  }
  if (this->pin_count == 4) {
    switch (thisStep) {
      case 0:  // 1010
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
      break;
      case 1:  // 0110
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
      break;
      case 2:  //0101
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
      break;
      case 3:  //1001
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
      break;
    }
  }

  if (this->pin_count == 5) {
    switch (thisStep) {
      case 0:  // 01101
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
        digitalWrite(motor_pin_5, HIGH);
        break;
      case 1:  // 01001
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, LOW);
        digitalWrite(motor_pin_5, HIGH);
        break;
      case 2:  // 01011
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
        digitalWrite(motor_pin_5, HIGH);
        break;
      case 3:  // 01010
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
        digitalWrite(motor_pin_5, LOW);
        break;
      case 4:  // 11010
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, HIGH);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
        digitalWrite(motor_pin_5, LOW);
        break;
      case 5:  // 10010
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, LOW);
        digitalWrite(motor_pin_4, HIGH);
        digitalWrite(motor_pin_5, LOW);
        break;
      case 6:  // 10110
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, HIGH);
        digitalWrite(motor_pin_5, LOW);
        break;
      case 7:  // 10100
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
        digitalWrite(motor_pin_5, LOW);
        break;
      case 8:  // 10101
        digitalWrite(motor_pin_1, HIGH);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
        digitalWrite(motor_pin_5, HIGH);
        break;
      case 9:  // 00101
        digitalWrite(motor_pin_1, LOW);
        digitalWrite(motor_pin_2, LOW);
        digitalWrite(motor_pin_3, HIGH);
        digitalWrite(motor_pin_4, LOW);
        digitalWrite(motor_pin_5, HIGH);
        break;
    }
  }
}


//version() returns the version of the library:
int myStepper::version(void)
{
  return 5;
}


//"ping" definitions (temporarily bundled here for debugging)****** 

///*
void Device::ping(){
  Serial.println( "device" );
}
//*/


///*
void mySpeaker::ping(){
  Serial.println( "mySpeaker" );
}

void myStepper::ping(){
  Serial.println( "myStepper" );
}

void dummySpeaker::ping(){
  Serial.println( "dummySpeaker" );
}

void dummyStepper::ping(){
  Serial.println( "dummyStepper" );
}
//*/


/*General utility function for reading data from serial
one line at a time (as opposed to the default one byte
at a time)*/
String getLine(){
  //Get the current line
  String currLine = "";
  boolean lineComplete = false;
  while ( lineComplete == false){
    if ( Serial.available() ){
      
      //add the current serial input character to the current input line string
      char inChar = Serial.read();
      
      currLine += inChar;
  
      //check if the current line is complete
      if ( inChar == '\n' ){
        lineComplete = true; 
      }
    }
  }
  return currLine;
}
