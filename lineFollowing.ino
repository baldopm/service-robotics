#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo

#include <QTRSensors.h>         //Pololu QTR Sensor Library


/*------ Arduino Line Follower Code----- */
/*-------definning Inputs------*/
#define LS 10      // left sensor
#define MS 9
#define RS 8      // right sensor

#define trigpin 12
#define echo 13

/*-------definning Outputs------*/
#define LM 3       // left motor
#define RM 2       // right motor

//Create QTR Sensor Line Following Object
QTRSensorsRC qtrrc((unsigned char[]) {LS, MS, RS}, 3, 2500, QTR_NO_EMITTER_PIN);
unsigned int sensorValues[3];

void setup()
{
  leftServo.attach(LM);  // Set left servo to digital pin 4
  servoRight.attach(RM);  // Set right servo to digital pin 3

  Serial.begin(9600);    //Open serial port and set this baudrate

  pinMode(trigpin, OUTPUT);
  pinMode(echo, INPUT);

  leftServo.write(90);
  servoRight.write(90);


  // then start calibration phase and move the sensors over both
  // reflectance extremes they will encounter in your application:
//  int i;
//  for (i = 0; i < 250; i++)  // make the calibration take about 5 seconds
//  {
//    qtrrc.calibrate();
//    delay(20);
//  }

  delay(500);
  for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  {
    qtrrc.calibrate();       // reads all sensors 10 times at 2500 us per read (i.e. ~25 ms per call)
  }
  
  // print the calibration minimum values measured when emitters were on
  for (int i = 0; i < 3; i++)
  {
    Serial.print(qtrrc.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < 3; i++)
  {
    Serial.print(qtrrc.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);

}

int distance;
void loop()
{
  qtrrc.read(sensorValues);             //Read Sensors

  int position = qtrrc.readLine(sensorValues);
  int error = position - 1000;
//
//  Serial.print ("Position: ");
//  Serial.print(position);
//  Serial.print ("\n");
//
//  Serial.print("Left: ");
//  Serial.print(sensorValues[0]);
//  Serial.print("Middle: ");
//  Serial.print(sensorValues[1]);
//  Serial.print("\t Right: ");
//  Serial.println(sensorValues[2]);

  findDistance();
//  Serial.print ("Distance: ");
//  Serial.print(distance);
  
  if (distance > 10)
  {
    if (position > 500 && position < 1000) // position is around 1000
    {
      forward();
      Serial.println ("Forward: ");
      Serial.println(position);
    }
    else if (sensorValues[0] < 500 && sensorValues[2] < 500 && sensorValues[1] < 500) // stop if all are on white
    {
      //stopRobot ();
      Serial.print("All white");
      qtrrc.readLine(sensorValues);    
      }
    else if (sensorValues[0] > 500 && sensorValues[2] > 500 && sensorValues[1] > 500) // stop if all are on black
    {
      stopRobot ();
      Serial.print("All black stop");
    }
    else if (position < 500) // the line is on the left
    {
      turnRight();
      delay(150);
      Serial.println ("Left turn: " );
      Serial.println (position);
    }
    else if (position > 1000)  // the line is on the right
    {
      turnLeft();
      delay(150);
      Serial.println ("Right turn: ");
      Serial.println(position);
    }
  }
  //  if(sensorValues[0]<500 && sensorValues[2]<500 && sensorValues[1]>500)     // Move Forward
  //  {
  //    forward();
  //  }
  //
  //  if(sensorValues[0]<500 && sensorValues[2]>500 && sensorValues[1]>500)     // Turn right
  //  {
  //    turnRight();
  //  }
  //
  //  if(sensorValues[0]>500 && sensorValues[2]<500 && sensorValues[1]>500)     // turn left
  //  {
  //    turnLeft();
  //  }
   else if (distance < 10)
   {
    //stops both servos
    stopRobot();
    delay(1000);
    //sets both servos to full speed in opposite direction
    //servoLeft.write(backward);
    //servoRight.write(forward);
    //delay(2000);
    //sets only the left servo to rotate so the robot turns
    //turnLeft();
    //delay(1500);
   }
}

// Motion routines for forward, reverse, turns, and stop
void forward() {
  leftServo.write(180);
  servoRight.write(0);
}

void reverse() {
  leftServo.write(0);
  servoRight.write(180);
}

void turnRight() {
  leftServo.write(180);
  servoRight.write(90);
}
void turnLeft() {
  leftServo.write(90);
  servoRight.write(0);
}

void stopRobot() {
  leftServo.write(90);
  servoRight.write(90);
}

void findDistance()
{
  int duration;
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);

  duration = pulseIn(echo, HIGH);
  distance = (1 / 29.1) * (duration / 2);
  delay(60);
}
