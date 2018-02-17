/*** Needed librarys ***/
#include <Servo.h>
#include <Wire.h>

/*** Defenitions ***/
#define LED 13
#define PWMPin1 3
#define PWMPin2 5

/*** Object initialization ***/
Servo pitchServo;
Servo rollServo;

/*** Global variabled ***/
int ledVal = 0;
int pitchOutput = 0;
int rollOutput = 0;

void setup(){
  Serial.begin(9600);

  pitchServo.attach(PWMPin1);
  pitchServo.write(90);
  rollServo.attach(PWMPin2);
  rollServo.write(90);
  Wire.begin(15);
  Wire.onReceive(receiveEvent);

  pinMode(LED, OUTPUT);

  delay(40);    
}

void loop(){

}

void receiveEvent (int length){
    // Only integers will be sent on the I2C i.e 16 bits of data
    int result = 0;

    if (length == 4){ // Length will always be 2 since the 16 bits is sent in two data cycles
        result = Wire.read();
        result <<= 8; // bit shift the first 8 bits eight positions forward
        result |= Wire.read();
        pitchOutput = result;

        result = 0;
        result = Wire.read();
        result <<= 8;
        result |= Wire.read();
        rollOutput = result;

        digitalWrite (LED, ledVal ^= 1);

        if(pitchOutput < 90)
            pitchOutput = pitchOutput - 12;

        if(pitchOutput > 90)
            pitchOutput = pitchOutput + 2;
        
        Serial.print("New pitch output: ");
        Serial.print(pitchOutput);
        Serial.print("   New roll output: ");
        Serial.println(rollOutput);

        while (Wire.available() > 0) // Gets rid of any spare junk on the I2C wire
            Wire.read();

        pitchServo.write(pitchOutput);
        delay(5);
        rollServo.write(rollOutput);
        delay(5);
    }

}