/******************************************************************************
    Mantaray Slave - Calibration

    

    Created 9 Jan 2018
    Modified 
    by David Bryngelsson

******************************************************************************/

/*** Needed librarys ***/
#include <Stepper.h>

/*** Defenitions ***/
#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64

#define INJECTOR_OUT_POS 2048
#define INJECTOR_IN_POS 0

// Pins for pitch motor
#define motorPin1 2
#define motorPin2 3
#define motorPin3 4
#define motorPin4 5

// Pins for roll motor
#define motorPin5 6
#define motorPin6 7
#define motorPin7 8
#define motorPin8 9

// Pins for injector motor
#define motorPin9 10
#define motorPin10 11
#define motorPin11 12
#define motorPin12 13

/*** Object initialization ***/
Stepper calibStepper(STEPS_PER_MOTOR_REVOLUTION, motorPin1, motorPin3, motorPin2, motorPin4);

/*** Global variables ***/
int motorPos = 0;
int wantedMotorPos = 0;

// Defenition and memory space for analog input
const int analogInPin = A0;
int prevPotentValue;

void setup(){
    // Motor configurations
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    calibStepper.setSpeed(700);

    Serial.begin(9600);
}

void loop(){
    currentPotentValue = readAnalogInput();

    if((currentPotentValue <  (wantedMotorPos + 2)) && (currentPotentValue > (wantedMotorPos - 2)))
        wantedMotorPos = currentPotentValue;
    // If a wanted position does not correspond with a wanted one. Step said motor one step
    if(motorPos != wantedMotorPos){
        if (motorPos > wantedMotorPos){
            calibStepper.step(-1);
            motorPos = motorPos - 1;
        }else{
            calibStepper.step(1);
            motorPos = motorPos + 1;
        }
    }
}

int readAnalogInput(){
    return (int) analogRead(analogInPin);
}