/*******************ActoBitty Line Follower: Sensor Test******************************
This code is for testing the sensors for line following Actobitty robot
using a pair of Pololu QTR1-RC sensors( http://www.pololu.com/product/2459)
and DRV8835 Motor driver ( http://www.pololu.com/product/22511).

Run this to see the Black and White reflectance values in the Serial monitor.
Move each sensor over the dark and light parts. High Value shows darker line.

Created by: Parth Kumar
Date      : Sept 12th 2014
Copyright : Robotzone LLC.
*************************************************************************/

#include <QTRSensors.h>         //Pololu QTR Sensor Library

//***********DEFINE PARAMETERS*************************
#define LeftSensePin    10    //Left Line Sensor Pin
#define middleLeftPin   9
#define middleRightPin  11
#define RightSensePin   8    //Right Line Sensor Pin 
//*******************************************************

//Create QTR Sensor Line Following Object
QTRSensorsRC qtrrc((unsigned char[]) {LeftSensePin, middleLeftPin, RightSensePin},3, 2500, QTR_NO_EMITTER_PIN);
unsigned int sensorValues[3];

void setup()
{
  Serial.begin(9600);    //Open serial port and set this baudrate
  
}

void loop()
{
  qtrrc.read(sensorValues);             //Read Sensors
  Serial.print("Left: ");
  Serial.print(sensorValues[0]);
  Serial.print("\t Middle: ");
  Serial.print(sensorValues[1]);
  Serial.print("\t Right: ");
  Serial.println(sensorValues[2]);
  delay(100);
}
