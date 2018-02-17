/******************************************************************************
    Mantaray - Calibration

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