#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo

#include <QTRSensors.h>         //Pololu QTR Sensor Library

/*------ Arduino Line Follower Code----- */
/*-------definning Inputs------*/
#define trigpin 7
#define echo 6

/*-------definning Outputs------*/
#define LM 2       // left motor
#define RM 3       // right motor

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
  Serial.begin(9600);
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
    stopRobot ();
    delay(2000);
    Serial.print("Intersection");
    
    position = 9000;
    
    turnLeft();
    delay(2500);

    position = qtra.readLine(sensorValues);
    
    //From the decision matrix choose the next move at an intersection
//    nextMove = decisions[decisionsCount];
//    decisionsCount++;
//    
//    if (nextMove == 1) {
//      forward();
//    }
//    if (nextMove == 2) {
//      turnLeft();
//      delay(150);
//    }
//    if (nextMove == 3) {
//      turnRight();
//      delay(150);
//    }

  }

  //Identify sideways Right Turn T-Junction
  if (sensorValues[0] > 900 && sensorValues[1] > 900 && sensorValues[2] > 900 && sensorValues[3] < 300 && sensorValues [4] < 300)
  {
    stopRobot ();
    delay(2000);

    position = 9000;
    
    Serial.print("Sideways Right Turn T-Intersection");

    turnRight();
    delay(2500);

    position = qtra.readLine(sensorValues);
  }
  
  if (position > 1700 && position < 2300 && sensorValues [0] < 500 && sensorValues [4] < 500 && position < 5000) // position is around 2000 go forward
  {
    forward();
    Serial.println ("Forward: ");
    Serial.println(position);
  }

  if (position >= 2500 && position < 5000) // the line is on the left
  {
    turnLeft();
    delay(150);
    Serial.println ("Left turn: " );
    Serial.println (position);
  }
  
  if (position <= 1700)  // the line is on the right
  {
    turnRight();
    delay(150);
    Serial.println ("Right turn: ");
    Serial.println(position);
  }  
  
//  if (sensorValues[0] < 500 && sensorValues[1] < 500 && sensorValues[2] < 500 && sensorValues[3] < 500 && sensorValues[4] < 500) //if all are on white
//  {
//    //will either be a dead-end or the line is lost
//    findDistance();
//    if (distance <= 15)
//    {
//      position = 9000;
//      stopRobot ();
//      delay(2000);
//      turnAround (); //reverse and turn 180 degrees because we are at a wall
//      position = qtra.readLine(sensorValues);
//    }
//      Serial.print("All white");
//   }
}

// Motion routines for forward, reverse, turns, and stop
void forward() {
  leftServo.write(0);
  servoRight.write(180);
}

void reverse() {
  leftServo.write(180);
  servoRight.write(0);
}

void turnRight() {
  leftServo.write(0);
  servoRight.write(90);
}
void turnLeft() {
  leftServo.write(90);
  servoRight.write(180);
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

void turnAround()
{
   servoRight.write(0);
   leftServo.write (0);
   delay (1500);
}
