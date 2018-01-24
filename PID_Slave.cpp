/******************************************************************************
    Mantaray - PID Slave 

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

// Pitch motor
#define motorPin1 2
#define motorPin2 3
#define motorPin3 4
#define motorPin4 5

// Roll motor
#define motorPin5 6
#define motorPin6 7
#define motorPin7 8
#define motorPin8 9

// Injector motor
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

// PID-connected variables
const double timeConstant = 0.04;
const int maxOutput = 5120;
const int minOutput = -5120;

double propVal;
double deriveVal;
double integralVal;
double integral;
double prevError;

double currentAngle = 0.0;
double setpoint = 0.0;
bool calcCompleted = false;

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
    pinMode(motorPin9, OUTPUT);
    pinMode(motorPin10,OUTPUT);
    pinMode(motorPin11, OUTPUT);
    pinMode(motorPin12, OUTPUT);
    pitchStepper.setSpeed(700);
    rollStepper.setSpeed(700);
    injectorStepper.setSpeed(700);

    Serial.begin(9600);
}

void loop(){

}

int calcOutput(){
	double error = setpoint - currentAngle;
	
	double propOutput = propVal * error;

	integral += error * timeConstant;
	double integralOutput = integralVal * integral;

	double derivative = (error - prevError) / timeConstant;
	double derivativeOutput = deriveVal * derivative;

	int output = (int) round(propOutput + integralOutput + derivativeOutput);

	if(output > maxOutput)
		output = maxOutput;
	else if(output < minOutput)
		output = minOutput;

	prevError = error;
	calcCompleted = true;

	return output;
}

void resetPIDVal(){
	prevError = 0.0;
	integral = 0.0;
}

void receiveEvent (int length){


}