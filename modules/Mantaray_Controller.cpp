/*** Needed librarys ***/
#include "NineAxesMotion.h"
#include <Wire.h>

/*** Definitions ***/
#define LED 13

/*** Object initialization ***/
NineAxesMotion gyroSensor;

/*** Global variables ***/
unsigned long lastStreamTime = 0;
const int streamPeriod = 20;

int pitch = 0;
int pitchOutput = 0;
int roll = 0;
int rollOutput = 0;
bool newValues = false;
bool errMsg = false;

int ledVal = 0;

void setup(){
    Serial.begin(9600);
    I2C.begin();
    Wire.begin();

    gyroSensor.initSensor();
    gyroSensor.setOperationMode(OPERATION_MODE_NDOF);
    gyroSensor.setUpdateMode(MANUAL);

    delay(40);
}

void loop(){

    if((millis() - lastStreamTime) >= streamPeriod){
        lastStreamTime = millis();
        updateSensors();
        readSensors();
    }

    if(newValues){
       errMsg = sendValues();

        if(!errMsg)
            digitalWrite(LED, ledVal ^= 1);

        newValues = false;
    }
}

void updateSensors(){
    gyroSensor.updateEuler();
    gyroSensor.updateCalibStatus();
}

void readSensors(){
    bool errMsg = false;

    if(pitch != (int) gyroSensor.readEulerPitch()){
        pitch = (int) gyroSensor.readEulerPitch();
        pitchOutput = map(pitch, -179, 179, 0, 180);
        newValues = true;
    }

    if(roll != (int) gyroSensor.readEulerRoll()){
        roll = (int) gyroSensor.readEulerRoll();
        rollOutput = map(roll, -90, 90, 0, 180);
        newValues = true;
    }
}

int sendValues(){
    int returnVal;

    Wire.beginTransmission(15);
    Wire.write(highByte (pitchOutput));
    Wire.write(lowByte (pitchOutput));
    Wire.write(highByte (rollOutput));
    Wire.write(lowByte (rollOutput));
    returnVal = Wire.endTransmission();

    return returnVal;
}