#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo

#include <QTRSensors.h>         //Pololu QTR Sensor Library

/*------ Arduino Line Follower Code----- */
/*-------definning Inputs------*/
#define trigpin 7
#define echo 6

/*-------definning Outputs------*/
#define LM 3       // left motor
#define RM 2       // right motor

//Create QTR Sensor Line Following Object
//QTRSensorsRC qtrrc((unsigned char[]) {LS, MS, RS}, 3, 2500, QTR_NO_EMITTER_PIN);
//unsigned int sensorValues[3];

#define NUM_SENSORS             5  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             QTR_NO_EMITTER_PIN  // emitter is controlled by digital pin 2

// sensors 0 through 5 are connected to analog inputs 0 through 4, respectively
QTRSensorsAnalog qtra((unsigned char[]) {
  0, 1, 2, 3, 4
}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

// Keeping track of distance and turning decisions
int distance;
#define ARRAY_SIZE 5;
int decisions[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
int decisionsCount;
int nextMove;
long duration;

void setup()
{
  leftServo.attach(LM);  // Set left servo to digital pin 4
  servoRight.attach(RM);  // Set right servo to digital pin 3

  Serial.begin(9600);    //Open serial port and set this baudrate

  pinMode(trigpin, OUTPUT);
  pinMode(echo, INPUT);

  leftServo.write(90);
  servoRight.write(90);

  delay(5000);

  for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  {
    qtra.calibrate();       // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
  }

  // print the calibration minimum values measured when emitters were on
  //Serial.begin(9600);
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);
}

void loop()
{
  //use sensors to follow the line and calculate position
  //from right to left the position varies from 0 - 4000 with the middle at 2000
  qtra.read(sensorValues);             //Read Sensors
  unsigned int position = qtra.readLine(sensorValues);

  if (sensorValues[0] > 500 && sensorValues[1] > 500 && sensorValues[3] > 500 && sensorValues[4] > 500) // Identify Intersection
  {
    stopRobot();
    Serial.println("Intersection");
    delay(1000);
    forward();
    delay(150);
    stopRobot();
    delay(1000);
    if (sensorValues[2] > 400) // Identify T-Junction
    {
      Serial.println("T-Junction");
      turnLeft();
      delay(2000);
    }
//    else{
//      Serial.println("Cross-Junction");
//    turnRight();
//    delay(2000);
//      
//    }
  }
  
  if (position > 1800 && position < 2200 && sensorValues [0] < 500 && sensorValues [4] < 500) // position is around 2000 go forward
  {
    forward();
    Serial.println ("Forward: ");
    Serial.println(position);
  }
  //else if (sensorValues[0] < 500 && sensorValues[1] < 500 && sensorValues[2] > 500 && (sensorValues[3] > 500 || sensorValues[4] > 500))
  else if (position > 2200) // the line is on the right
  {
    turnRight();
    //delay(1000);
    Serial.println ("Left turn: " );
    Serial.println (position);
  }
    //else if ((sensorValues[0] > 500 || sensorValues[1] > 500) && sensorValues[2] > 500 && sensorValues[3] < 500 && sensorValues[4] < 500)
  else if (position < 1900)  // the line is on the left
  {
    turnLeft();
    //delay(1000);
    Serial.println ("Right turn: ");
    Serial.println(position);
  }  
  else if (sensorValues[0] < 500 && sensorValues[1] < 500 && sensorValues[2] < 500 && sensorValues[3] < 500 && sensorValues[4] < 500) //if all are on white
  //else if (position < 1900 && position > 2200)
  {
    //will either be a dead-end or the line is lost
    findDistance();
    Serial.println ("Distance to the wall: ");
    Serial.println(distance);
    if (distance <= 20)
    {
      stopRobot();
      delay(150);
      deadEnd (); //reverse and turn 180 degrees because we are at a wall
      Serial.println ("Reverse: ");
      Serial.println(position);
    }
    else
    {
      forward();
      delay(3000);
      turnRight();
      delay(2000);
    }
    Serial.print("All white");
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

//Distance to object using ultrasonic sensor
void findDistance()
{
  // Clears the trigpin
  digitalWrite(trigpin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigpin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(distance);
}

//Pick up the cylinders
void pickUp ()
{

}

//At a dead end reverse and turn around 180 degrees
void deadEnd ()
{
  stopRobot();
  delay(500);
  turnLeft();
  delay(1000);
  reverse();
  delay (1000);
  turnLeft ();
  delay(2000);
  reverse();
  delay(800);
  turnLeft();
  delay(2000);
  stopRobot();
}
