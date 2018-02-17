/******************************************************************************
    Mantaray - Calibration with servo motor

    Steps a stepper motor up or down depending on the value recived from a 
    analog potentiometer. 

    Created 9 Jan 2018
    Modified 
    by David Bryngelsson

******************************************************************************/

/*** Needed librarys ***/
#include <Servo.h>

/*** Defenitions ***/
#define PWMPin 9

/*** Object initialization ***/
Servo calibServo;

/*** Global variables ***/

// Defenition and memory space for analog input
const int analogInPin = A0;
int currentPotentValue = 0;
int servoOutput = 0;

void setup(){
    // Motor configurations
    calibServo.attach(PWMPin);
    calibServo.write(0);
    delay(50);

    Serial.begin(9600);
}

void loop(){
    currentPotentValue = readAnalogInput();
    servoOutput = map(currentPotentValue, 0, 690, 0, 180);
    Serial.print("Servo output: ");
    Serial.println(servoOutput);
    calibServo.write(servoOutput);
    delay(10);
}

int readAnalogInput(){
    return (int) analogRead(analogInPin);
}





/******************************************************************************
    Mantaray Slave - Calibration with Stepper motor

    

    Created 9 Jan 2018
    Modified 
    by David Bryngelsson

******************************************************************************/

/*** Needed librarys ***/
#include <Stepper.h>

/*** Defenitions ***/
#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64

// Pins for pitch motor
#define motorPin1 2
#define motorPin2 3
#define motorPin3 4
#define motorPin4 5

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