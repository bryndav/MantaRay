/******************************************************************************
    Mantaray - PID Slave 

    Created 9 Jan 2018
    Modified 
    by David Bryngelsson

******************************************************************************/

/*** Needed librarys ***/
#include <Servo.h>
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

// PWN Pins
#define PWMPin1   3
#define PWMPin2   5

/*** Object initialization ***/
Servo pitchServo;
Servo rollServo;
Stepper injectorStepper(STEPS_PER_MOTOR_REVOLUTION, motorPin1, motorPin3, motorPin2, motorPin4);

/*** Global variables ***/
int ledVal = 0;
unsigned long timeStamp = 0;
int i;

// Current motor positions
int injPos = 0;
int rollAngle = 0;

// Wanted motor positions
int wantedInjPos = 0;

// PID-connected variables
const double timeConstant = 0.04;
const int maxOutput = 180;
const int minOutput = 0;

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
    injectorStepper.setSpeed(700);
    
    pitchServo.attach(PWMPin1);
    rollServo.attach(PWMPin2);

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
	if(length == 4){

		float message;
	
		Serial.print("Time since last message: ");
		Serial.println((millis() - timeStamp));
		timeStamp = millis();

		for(i = 4; i > 0; i--){
			if(i > 1){
				message |= Wire.read();
				message <<= i * 8;
			}else
				message |= Wire.read();
		}

		while (Wire.available() > 0)
			Wire.read();

		calcCompleted = false;
		currentAngle = message;

	}else if(length == 2){
		int message = 0;

		message = Wire.read();
		message <<= 8;
		message |= Wire.read();

		while (Wire.available() > 0)
			Wire.read();

		rollAngel = message;
	}
}