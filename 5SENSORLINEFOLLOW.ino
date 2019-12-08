#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo

#include <QTRSensors.h>         //Pololu QTR Sensor Library

/*------ Arduino Line Follower Code----- */
/*-------definning Inputs------*/
#define echoF 6
#define trigF 7

#define echoL 8
#define trigL 9

#define echoR 4
#define trigR 5

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
int decisions[] = {2, 2, 2, 1, 2, 2, 1, 1, 2, 1, 2, 1, 2, 2};
int decisionsCount;
int nextMove;
int cylinder = 0;

long duration1;
long duration2;
long duration3;

int frontDist;
int rightDist;
int leftDist;

boolean isForward;
boolean isTurning;
boolean isFollowing;

void setup()
{
  leftServo.attach(LM);  // Set left servo to digital pin 4
  servoRight.attach(RM);  // Set right servo to digital pin 3

  Serial.begin(9600);    //Open serial port and set this baudrate

  pinMode(trigF, OUTPUT);
  pinMode(echoF, INPUT);
  pinMode(trigL, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(trigR, OUTPUT);
  pinMode(echoR, INPUT);

  leftServo.write(90);
  servoRight.write(90);

  delay(1000);

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
  forward();
  delay(2000);
}

void loop()
{
  //use sensors to follow the line and calculate position
  //from right to left the position varies from 0 - 4000 with the middle at 2000
  qtra.read(sensorValues);             //Read Sensors
  unsigned int position = qtra.readLine(sensorValues);
  isFollowing = true;

  //if all the sensors are on the white area
  if (sensorValues[0] < 500 && sensorValues[1] < 300 && sensorValues[2] < 300 && sensorValues[3] < 300 && sensorValues[4] < 300)
  {
    Serial.print("All white IS FORWARD");
    stopRobot();
    delay(500);

    //will either be a dead-end or the line is lost
    findFrontDist();
    findLeftDist ();
    findRightDist();
    
    if (frontDist <= 15 && leftDist <= 11 && rightDist <= 11)
    {
      position = 9000;
      stopRobot ();
      delay(1000);
      turnAround (); //turn 180 degrees because we are at a wall
      delay(2500);
    }
    //else if (frontDist > 11)
    else if (frontDist > 16 && frontDist < 35 && rightDist > 15 && leftDist <=14 && leftDist >6)
    {
      position = 9000;
      
      stopRobot();
      delay(3000);
      forward ();
      delay(2000);
      turnRight();
      delay(1500);
      forward();
      delay(1600);

      position = qtra.readLine(sensorValues);

    }
    else if (frontDist > 16 && frontDist < 35 && leftDist > 15 && rightDist <=14 && rightDist >6)
    {
      position = 9000;
      
      stopRobot();
      delay(3000);
      forward ();
      delay(2000);
      turnLeft();
      delay(1300);
      forward();
      delay(1600);

      position = qtra.readLine(sensorValues);

    }
  }

  if (sensorValues[0] > 500 && sensorValues[1] > 500 && sensorValues [2] > 500 && sensorValues[3] > 500 && sensorValues[4] > 500) // Identify Intersection
  {
    stopRobot ();
    delay(1000);
    Serial.print("Intersection");

    position = 9000;

    forward();
    delay(350);

//    turnLeft();
//    delay(1200);

    path();

    position = qtra.readLine(sensorValues);
  }

  if (position >= 1600 && position <= 2400 && sensorValues [0] < 500 && sensorValues [4] < 500 && position < 5000) // position is around 2000 go forward
  {
    isFollowing = true;
    forward();
    Serial.println ("Forward: ");
    Serial.println(position);
  }

  if (position < 1600)  //the line is on the right
  {
    //The two rightmost sensors will be on the line
    if (sensorValues [0] > 700 && sensorValues [1] > 700)
    {
      stopRobot();
      delay(500);

      forward();
      delay(500);
      qtra.read(sensorValues);       //Read Sensors

      // If one of the three middle sensors is on a line we are at a right turn T-Junction
      if (sensorValues [4] < 300 && sensorValues [2] > 800 || sensorValues [4] < 300 && sensorValues [1] > 800 || sensorValues [4] < 300 && sensorValues [3] > 800)
      {
        position = 9000;

        stopRobot();
        delay(1000);
        path();

        position = qtra.readLine(sensorValues);
      }
      //Otherwise we can turn fully right
      else
      {
        turnRight();
        delay(1150);
        forward();
        delay(350);

        position = qtra.readLine(sensorValues);
      }
    }
    //If there is a misalignment while going straight it can be corrected
    else
    {
      turnRight();
      delay(100);
    }

    Serial.println ("Right turn: ");
    Serial.println(position);
  }

  if (position > 2600 && position < 5000) //the line is on the left
  {
    //The two leftmost sensors should be on the line
    if (sensorValues [3] > 700 && sensorValues [4] > 700)
    {
      stopRobot();
      delay(500);

      //Go slightly forward
      forward();
      delay(350);
      qtra.read(sensorValues);  //Read Sensors

      // If one of the three middle sensors is on a line we are at a left Turn T-Junction
      if (sensorValues [4] < 300 && sensorValues [2] > 800 || sensorValues [4] < 300 && sensorValues [1] > 800 || sensorValues [4] < 300 && sensorValues [3] > 800)
      {
        position = 9000;

        stopRobot();
        delay(1000);
        path();

        position = qtra.readLine(sensorValues);
      }
      //Otherwise we can fully turn left if there is no other option
      else
      {
        turnLeft();
        delay(1150);
        forward();
        delay(350);

        position = qtra.readLine(sensorValues);
      }
    }
    //If there is a slight misalignment while going straight it can be corrected
    else
    {
      turnLeft();
      delay(95);
    }

    Serial.println ("Left turn: ");
    Serial.println(position);
  }

  //as a proof of the exit path
//  if(
//  decisions[decisionsCount]==6){
//    cylinder=3;
//  }
  
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
  //changed right from 90 to 0
  servoRight.write(0);
}
void turnLeft() {
  leftServo.write(180);
  //changed left from 90 to 180
  servoRight.write(180);
}

void stopRobot() {
  leftServo.write(90);
  servoRight.write(90);
}

//Distance to object using ultrasonic sensor
void findFrontDist()
{
  // Clears the trigpin
  digitalWrite(trigF, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigF, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigF, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration1 = pulseIn(echoF, HIGH);
  // Calculating the distance
  frontDist = duration1 * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance: ");
  Serial.println(frontDist);
}

void findLeftDist()
{
  // Clears the trigpin
  digitalWrite(trigL, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigL, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigL, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoL, HIGH);
  // Calculating the distance
  leftDist = duration2 * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Left Distance: ");
  Serial.println(leftDist);
}

void findRightDist()
{
  // Clears the trigpin
  digitalWrite(trigR, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigR, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration3 = pulseIn(echoR, HIGH);
  // Calculating the distance
  rightDist = duration3 * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.print("Right Distance: ");
  Serial.println(rightDist);
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
}

void path()
{
  //From the decision matrix choose the next move at an intersection
        if(cylinder == 3){
          exit_path();
        }else{
        nextMove = decisions[decisionsCount];
        decisionsCount++;
    
        if (nextMove == 1) {
          forward();
        }
        if (nextMove == 2) {
          turnLeft();
          delay(1200);
        }
        if (nextMove == 3) {
          turnRight();
          delay(1200);
        }
        }
}

int j=0;
void exit_path()
{
  if(decisions[decisionsCount]==3){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ forward(); } //at first cross
    else if (j == 2){ turnRight(); } //at T junction
    //Exit found
    }
  if(decisions[decisionsCount]==4){
    j = j + 1;
    if (j == 1){ turnLeft(); } //at first T
    else if (j == 2){ turnRight(); } //at T junction
    else if (j == 3){ turnLeft(); } //at cross junction
    else if (j == 4){ forward(); } //at cross junction
    else if (j == 5){ turnLeft(); } //at T junction
    //Exit found  
    }
  if(decisions[decisionsCount]==5){
    j = j + 1;
    if (j == 1){ turnRight(); } //at T junction
    else if (j == 2){ turnLeft(); } //at cross junction
    else if (j == 3){ forward(); } //at cross junction
    else if (j == 4){ turnLeft(); } //at T junction
    //Exit found  
    }
  if(decisions[decisionsCount]==6){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ forward(); } //at T junction
    else if (j == 2){ turnLeft(); } //at cross junction
    else if (j == 3){ forward(); } //at cross junction
    else if (j == 4){ turnLeft(); } //at T junction
    //Exit found
    }
  if(decisions[decisionsCount]==7){
    j = j + 1;
    if (j == 1){ turnLeft(); } //at cross junction
    else if (j == 2){ forward(); } //at cross junction
    else if (j == 3){ turnLeft(); } //at T junction
    //Exit found
    }
  if(decisions[decisionsCount]==8){
    j = j + 1;
    if (j == 1){ turnLeft(); } //at cross junction
    else if (j == 2){ turnRight(); } //at T junction
    //Exit found
    }
  if(decisions[decisionsCount]==10){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ turnLeft(); } //at T junction
    //Exit found
    }
  if(decisions[decisionsCount]==11){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ turnRight(); } //at cross junction
    else if (j == 2){ turnLeft(); } //at T junction
    //Exit found
    }
  if(decisions[decisionsCount]==12){
    if (j==0){reverse();}
    j = j + 1;
    if (j == 1){ turnLeft(); } //at cross junction
    else if (j == 2){ turnLeft(); } //at T junction
    //Exit found
    } 
}
