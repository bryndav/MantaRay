/******************************************************************************
    Mantaray Slave - Follow

    This program takes messured changes in three dimensions (X, Y, Z) over I2C.
    These values are then mapped to positions used by three stepper motors.
    
    In the main loop the program checks if the current position of each motor
    corresponds to the current motor position. If not the motors will step
    towards the wanted position one step at a time. This is done to simulate 
    the motors running simultaneously and allow for software interrupts to
    alter the wanted positions mid movement. 

    The code is used in a underwater ROV project and therefore variable names
    and functionality may sometime cause confusion.

    Created 9 Jan 2018
    Modified 
    by David Bryngelsson

******************************************************************************/

/*** Needed librarys ***/
#include <Stepper.h>
#include <Wire.h>

/*** Defenitions ***/
#define LED 13

#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64

#define INJECTOR_OUT_POS 2048
#define INJECTOR_IN_POS 0

#define motorPin1 2
#define motorPin2 3
#define motorPin3 4
#define motorPin4 5

#define motorPin5 6
#define motorPin6 7
#define motorPin7 8
#define motorPin8 9

#define motorPin9 10
#define motorPin10 11
#define motorPin11 12
#define motorPin12 13

/*** Object initialization ***/
Stepper pitchStepper(STEPS_PER_MOTOR_REVOLUTION, motorPin1, motorPin3, motorPin2, motorPin4);
Stepper rollStepper(STEPS_PER_MOTOR_REVOLUTION, motorPin5, motorPin7, motorPin6, motorPin8);
Stepper injectorStepper(STEPS_PER_MOTOR_REVOLUTION, motorPin9, motorPin11, motorPin10, motorPin12);

/*** Global variables ***/
int ledVal = 0;

// Current motor positions
int pitchPos = 0;
int heading = 0;
int rollPos = 0;
int injectorPos = 0;

// Wanted motor positions
int wantedPitchPos = 0;
int wantedHeading = 0;
int wantedRoll = 0;
int wantedInjectorPos = 0;

void setup(){
    Wire.begin(15);                 // Initializes I2C as slave with adress 15
    Wire.onReceive(receiveEvent);   // Connect incoming messages with a function

    pinMode(LED, OUTPUT);

    // Motor configurations
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    pinMode(motorPin5, OUTPUT);
    pinMode(motorPin6, OUTPUT);
    pinMode(motorPin7, OUTPUT);
    pinMode(motorPin8, OUTPUT);
    pitchStepper.setSpeed(700);
    rollStepper.setSpeed(700);
    injectorStepper.setSpeed(700);

    Serial.begin(9600);
}

void loop(){

    // If a wanted position does not correspond with a wanted one. Step said motor one step
    if(pitchPos != wantedPitchPos){
        if (pitchPos > wantedPitchPos){
            pitchStepper.step(-1);
            pitchPos = pitchPos - 1;
        }else{
            pitchStepper.step(1);
            pitchPos = pitchPos + 1;
        }
    }

    if(rollPos != wantedRoll){
        if(rollPos > wantedRoll){
            rollStepper.step(-1);
            rollPos = rollPos - 1;
        }else{
            rollStepper.step(1);
            rollPos = rollPos + 1;
        }
    }

   if (injectorPos != wantedInjectorPos){
      injectorStepper.step(1);
      injectorPos = injectorPos + 1;
   }
}

void receiveEvent (int length){
    // Only integers will be sent on the I2C i.e 16 bits of data
    int result;

    if (length == 2){ // Length will always be 2 since the 16 bits is sent in two data cycles
        result = Wire.read();
        result <<= 8; // bit shift the first 8 bits eight positions forward
        result |= Wire.read();

        decodeMSG(result); // Decode the value recived to match either, heading, pitch, roll or injector
        digitalWrite (LED, ledVal ^= 1);

        while (Wire.available() > 0) // Gets rid of any spare junk on the I2C wire
            Wire.read();
        
    }

}

void decodeMSG(int val){
    // The encoding is done by adding either 2000, 4000, 6000 or 8000
    // By looking on the size of the value we can then decide which value to alter
    if (val < 3000){
        wantedHeading = val - 2000;
        wantedHeading = map(wantedHeading, 0, 360, 0,2048);
    }else if((val >= 3000) && (val < 5000)){
        wantedPitchPos = val - 4000;
        wantedPitchPos = map(wantedPitchPos, -179, 179, -5120, 5120);
    }else if((val >= 5000) && (val < 8000)){
        wantedRoll = val - 6000;
        wantedRoll = map(wantedRoll, 0, 90, 0, 2048);
    }else if(val >= 8000){   
        if((val - 8000) == 0){
           wantedInjectorPos = INJECTOR_IN_POS;
        }else if((val - 8000) == 688){
           wantedInjectorPos = INJECTOR_OUT_POS;  
        }
    }
}