/******************************************************************************
    Mantaray Master - Follow

    This program sends messured changes in three dimensions (X, Y, Z) over I2C.
    Messurements are done with a nine axes motion shield connected to a UNO.
    In order for I2C to work analog pin 4 and 5 on the master arduino needs to 
    be connected to with analog pins 4 and 5 on the slave card.

    A potentiometer is used for simulation and is connected to analog pin 0.
    These changes are also sent over the I2C bus.

    Most of the hardwork is done by the Nine Axes Motion library written by
    Sergio Masello and a lot of credit goes to him and Bosch.
    Link: https://github.com/arduino-org/arduino-library-nine-axes-motion

    For questions regarding the function of the NineAxesMotion library please
    consult the github page.

    Created 9 Jan 2018
    Modified 
    by David Bryngelsson

******************************************************************************/

/*** Needed librarys ***/
#include "NineAxesMotion.h"
#include <Wire.h>

/*** Defenitions ***/
#define LED 13

/*** Object initialization ***/
NineAxesMotion gyroSensor;          // Sensor object that update and reads values

/*** Global variables ***/
unsigned long lastStreamTime = 0;   // Time stamp for the last update
const int streamPeriod = 20;        // Sets the stream time to 50 Hz (1000 / 20)

const int analogInPin = A0;
int potentValue;

int ledVal = 0;

int roll;
int pitch;
int heading;

void setup(){
    Serial.begin(9600);
    I2C.begin();                    // Initializes I2C communication 
    Wire.begin();                   // Sets the I2C master devic

    gyroSensor.initSensor();
    gyroSensor.setOperationMode(OPERATION_MODE_NDOF);
    gyroSensor.setUpdateMode(MANUAL);

    pinMode(LED, OUTPUT);
}

void loop()
{
    if((millis() - lastStreamTime) >= streamPeriod)
    {
        lastStreamTime = millis();
        updateSensors();
        readSensors();
    }
}

void updateSensors(){
    gyroSensor.updateEuler();       //Updates the Euler data in the sensor object
    gyroSensor.updateCalibStatus();
}


void readSensors(){
    //Checks for differences in the 3 directions and the potentiometer value
    //If a new value is retrived broadcast it over I2C with encoding

    bool errMsg = false; //Can be used for error handling in future modifications
    int message;
    
    if(diffCheck(heading, 'h')){
        heading = (int) gyroSensor.readEulerHeading();
        Serial.print("Current heading: ");
        Serial.println(heading);

        message = heading + 2000;
        errMsg = sendValue(message);
        
        if(!errMsg)
            digitalWrite (LED, ledVal ^= 1); //Blink the built in led to confirm that data has been sent correctly
    }

    if(diffCheck(roll, 'r')){
        roll    = (int) gyroSensor.readEulerRoll();
        Serial.print("Current roll: ");
        Serial.println(roll);

        message = roll + 6000;
        errMsg = sendValue(message);

        if(!errMsg)
            digitalWrite (LED, ledVal ^= 1); //Blink the built in led to confirm that data has been sent correctly
    }

    if(diffCheck(pitch, 'p')){
        pitch   = (int) gyroSensor.readEulerPitch();
        Serial.print("Current pitch: ");
        Serial.println(pitch);

        message = pitch + 4000;
        errMsg = sendValue(message);

        if(!errMsg)
            digitalWrite (LED, ledVal ^= 1); //Blink the built in led to confirm that data has been sent correctly 
    }

   if(diffCheck(potentValue, 'a')){
     potentValue = (int) analogRead(analogInPin);
     Serial.print("Potentiometer value: ");
     Serial.println(potentValue);

     message = potentValue + 8000;
     errMsg = sendValue(message);

     if(!errMsg)
         digitalWrite (LED, ledVal ^= 1); //Blink the built in led to confirm that data has been sent correctly
   }

}

int diffCheck(int prevValue, char val){
    //A controll character is sent in order to differentiate the different axes.
    //If a change has occured returns 1 otherwise 0
    if (val == 'h')
        if(prevValue == (int) gyroSensor.readEulerHeading())
            return 0;
        else
            return 1;
    else if(val == 'r')
        if(prevValue == (int) gyroSensor.readEulerRoll())
            return 0;
        else
            return 1;
    else if(val == 'p')
        if(prevValue == (int) gyroSensor.readEulerPitch())
            return 0;
        else
            return 1;
    else if(val == 'a')
        if(((int) analogRead(analogInPin) <  (prevValue + 5)) &&
        ((int) analogRead(analogInPin) > (prevValue - 5))) //A small filter is introduced to reduce flickering values
            return 0;
        else
          return 1;
}


int sendValue(int value){
    int returnVal;

    Wire.beginTransmission(15);         // Here 15 is the adress to the slave arduino, allowed values (10-127)
    Wire.write(highByte (value));       // I2C takes 8 bits of data per cycle, the int needs to be split into two cycles
    Wire.write(lowByte (value));        // First sends highByte and secondly lowByte results in a length of 2 cycles
    returnVal = Wire.endTransmission(); // endTransmission returns 0 on success

    return returnVal;
}