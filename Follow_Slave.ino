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
#include <Servo.h>
#include <Stepper.h>
#include <Wire.h>

/*** Defenitions ***/
#define LED 13

#define STEPS_PER_MOTOR_REVOLUTION 32
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64

#define INJECTOR_OUT_POS 2048
#define INJECTOR_IN_POS 0

// Injector motor
#define motorPin1 2
#define motorPin2 3
#define motorPin3 4
#define motorPin4 5

// Roll motor
#define PWMPin1   9

// Pitch motor
#define PWMPin2   10

/*** Object initialization ***/
Servo pitchServo;
Servo rollServo;
Stepper injectorStepper(STEPS_PER_MOTOR_REVOLUTION, motorPin1, motorPin3, motorPin2, motorPin4);

/*** Global variables ***/
int ledVal = 0;

// Current motor positions
int heading = 0;
int injectorPos = 0;

// Wanted motor positions
int wantedHeading = 0;
int wantedInjectorPos = 0;

// Servo output
int pitchOutput = 0;
int rollOutput = 0;

void setup(){
    Wire.begin(15);                 // Initializes I2C as slave with adress 15
    Wire.onReceive(receiveEvent);   // Connect incoming messages with a function

    pinMode(LED, OUTPUT);

    // Motor configurations
    rollServo.attach(PWMPin1);
    pitchServo.attach(PWMPin2);

    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
    injectorStepper.setSpeed(700);

    Serial.begin(9600);
}

void loop(){

    // If a wanted position does not correspond with a wanted one. Step said motor one step
   if (injectorPos != wantedInjectorPos){
      injectorStepper.step(1);
      injectorPos = injectorPos + 1;
   }

   rollServo.write(rollOutput);
   delay(10);
   pitchServo.write(pitchOutput);
   delay(10); 
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
        wantedHeading = (val - 2000);
        wantedHeading = map(wantedHeading, 0, 360, 0,2048);
        Serial.print("Recieved new heading: ");
        Serial.println(val - 2000);
    }else if((val >= 3000) && (val < 5000)){
        pitchOutput = (val - 4000);
        pitchOutput = map(pitchOutput, -179, 179, 0, 180);
        Serial.print("Recieved new pitch value: ");
        Serial.println(val - 4000);
        Serial.print("New pitch output: ");
        Serial.println(pitchOutput);
    }else if((val >= 5000) && (val < 8000)){
        rollOutput = (val - 6000);
        rollOutput = map(rollOutput, -90, 90, 0, 180);
        Serial.print("Recieved new roll value: ");
        Serial.println(val - 6000);
        Serial.print("New roll output: ");
        Serial.println(rollOutput);
    }else if(val >= 8000){ 
        Serial.print("Recieved new potentiometer value: ");
        Serial.println(val - 8000);  
        if((val - 8000) == 0){
           wantedInjectorPos = INJECTOR_IN_POS;
        }else if((val - 8000) == 690){
           wantedInjectorPos = INJECTOR_OUT_POS;  
        }
    }
}