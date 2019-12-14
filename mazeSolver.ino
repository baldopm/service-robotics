#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo

Servo servoG;
Servo servoA;

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
#define LM 3       // left motor
#define RM 13       // right motor

// Declare the Servo pin
#define servoPinG 12 //To be changed
#define servoPinA 11

#define interruptPin 2 //interrupt sensor

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
int interTurns[] = {2, 2, 3, 2, 1, 2, 2, 2, 2, 2, 2, 2, 2};
int intersectionCount = 0;
int deadCount = 0;

int rightJunction[] = {3, 1, 1, 1};
int rightCount = 0;

int leftJunction[] = {2, 1, 2};
int leftCount = 0;

int nextMove;
int whiteCount = 0;

long duration1;
long duration2;
long duration3;

int frontDist;
int rightDist;
int leftDist;

boolean isForward;
boolean isTurning;
boolean isFollowing;

boolean firstCylinder = false; //first cylinder hasn't been caught
int cylinder = 0;
int counter = 0;

boolean grip = false;
unsigned long button_time = 0;
unsigned long last_button_time = 0;

void setup()
{
  leftServo.attach(LM);  // Set left servo to digital pin 4
  servoRight.attach(RM);  // Set right servo to digital pin 3

  servoG.attach(servoPinG);
  servoA.attach(servoPinA);

  servoG.write(50);
  servoA.write(80);

  //Attach button switch
  attachInterrupt(digitalPinToInterrupt(2), onPressed, RISING);

  Serial.begin(9600);    //Open serial port and set this baudrate

  pinMode(trigF, OUTPUT);
  pinMode(echoF, INPUT);
  pinMode(trigL, OUTPUT);
  pinMode(echoL, INPUT);
  pinMode(trigR, OUTPUT);
  pinMode(echoR, INPUT);

  stopRobot();
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

  leftServo.write(90);
  servoRight.write(90);
  delay(1000);

  forward();
  delay(300);
}

void loop()
{
  //use sensors to follow the line and calculate position
  //from right to left the position varies from 0 - 4000 with the middle at 2000
  qtra.read(sensorValues);             //Read Sensors
  unsigned int position = qtra.readLine(sensorValues);
  isFollowing = true;

  if (grip)
  {
    if (firstCylinder == false)
    {
      stopRobot();
      delay(50);
      leftServo.detach(); //just tried this since my wheels were drifting
      servoRight.detach(); //just tried this since my wheels were drifting
      pickUpAndHold();
    }
    else
    {
      stopRobot();
      delay(50);
      leftServo.detach(); //just tried this since my wheels were drifting
      servoRight.detach(); //just tried this since my wheels were drifting
      pickUp();
    }
  }

  if (sensorValues[0] > 500 && sensorValues[1] > 500 && sensorValues [2] > 500 && sensorValues[3] > 500 && sensorValues[4] > 500) // Identify Intersection
  {
    Serial.print("Intersection");

    position = 9000;

    //From the decision matrix choose the next move at an intersection
    nextMove = interTurns[intersectionCount];
    intersectionCount++;

    stopRobot();
    delay(300);

    //if count exceeds array size always turn left
    if (intersectionCount == 13)
    {
      nextMove = 2;
    }

    if (deadCount == 3)
    {
      forward();
      delay(500);
      nextMove = 0;
    }
    if (nextMove == 1) {
      forward();
      delay(500);
    }
    if (nextMove == 2) {
      forward();
      delay(500);
      turnLeft();
      delay(1200);
    }
    if (nextMove == 3) {
      forward();
      delay(500);
      turnRight();
      delay(1200);
    }
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
      delay(50);
      forward();
      delay(500);
      qtra.read(sensorValues);       //Read Sensors
      // If one of the three middle sensors is on a line we are at a right turn T-Junction
      if (sensorValues [4] < 500 && sensorValues [2] > 500 || sensorValues [4] < 500 && sensorValues [1] > 600 || sensorValues [4] < 500 && sensorValues [3] > 600)
      {
        nextMove = rightJunction [rightCount];
        rightCount ++;

        if (rightCount == 4)
        {
          nextMove = 1;
        }
        if (deadCount == 4)
        {
          nextMove = 3;
        }

        stopRobot();
        delay(100);
        position = 9000;

        if (nextMove == 1) {
          forward();
          delay(500);
        }
        if (nextMove == 3) {
          turnRight();
          delay(1200);
        }
        position = qtra.readLine(sensorValues);
      }
      //Otherwise we can turn fully right
      else
      {
        turnRight();
        delay(1200);

        position = qtra.readLine(sensorValues);
      }
    }
    //If there is a misalignment while going straight it can be corrected
    else
    {
      turnRight();
      delay(85);
    }

    Serial.println ("Right turn: ");
    Serial.println(position);
  }

  if (position > 2500 && position < 5000) //the line is on the left
  {
    //The two leftmost sensors should be on the line
    if (sensorValues [3] > 700 && sensorValues [4] > 700)
    {
      //Go slightly forward
      stopRobot();
      delay(50);
      forward();
      delay(500);

      qtra.read(sensorValues);       //Read Sensors
      // If one of the three middle sensors is on a line we are at a left Turn T-Junction
      if (sensorValues [0] < 500 && sensorValues [2] > 500 || sensorValues [0] < 300 && sensorValues [1] > 600 || sensorValues [0] < 300 && sensorValues [3] > 600)
      {
        position = 9000;
        nextMove = leftJunction [leftCount];
        leftCount++;

        if (leftCount == 3)
        {
          leftCount = 0;
        }

        if (deadCount == 4)
        {
          nextMove = 2;
        }

        stopRobot();
        delay(100);

        if (nextMove == 1) {
          forward();
          delay(500);
        }
        if (nextMove == 2) {
          turnLeft();
          delay(1200);
        }
        position = qtra.readLine(sensorValues);
      }
      //Otherwise we can fully turn left if there is no other option
      else
      {
        turnLeft();
        delay(1150);

        position = qtra.readLine(sensorValues);
      }
    }
    //If there is a slight misalignment while going straight it can be corrected
    else
    {
      turnLeft();
      delay(85);
    }

    Serial.println ("Left turn: ");
    Serial.println(position);
  }

  //if all the sensors are on the white area
  if (sensorValues[0] < 300 && sensorValues[1] < 300 && sensorValues[2] < 300 && sensorValues[3] < 300 && sensorValues[4] < 300)
  {
    Serial.print("All white IS FORWARD");
    stopRobot();
    delay(300);

    //will either be a dead-end or the line is lost
    findFrontDist();
    findLeftDist ();
    findRightDist();

    stopRobot();
    delay(10);

    if (frontDist <= 15 && leftDist <= 11 && rightDist <= 11)
    {
      position = 9000;

      turnAround (); //turn 180 degrees because we are at a wall
      delay(2450);
      deadCount++;

      position = qtra.readLine(sensorValues);
    }
    else if (frontDist > 16 && leftDist <= 14 || frontDist == 0 && leftDist <= 14)
    {
      position = 9000;

      findFrontDist();
      while (frontDist >= 10 || frontDist == 0)
      {
        Serial.println (frontDist);
        forward();
        delay(100);
        findFrontDist();
        findLeftDist();
        if (leftDist < 11)
        {
          turnRight();
          delay(10);
        }
      }

      turnRight();
      delay(1200);

      findFrontDist();
      stopRobot();
      delay(50);
      while (frontDist >= 20 || frontDist == 0)
      {
        forward();
        delay(100);
        findFrontDist();
        findLeftDist();
        findRightDist();
        qtra.read(sensorValues);
        if (leftDist < 11)
        {
          turnRight();
          delay(10);
        }
        if (rightDist <= 11 || rightDist == 0)
        {
          turnLeft();
          delay(20);
        }
        if (sensorValues [1] > 500 || sensorValues [2] > 500 || sensorValues [3] > 500)
        {
          break;
        }
      }
      position = qtra.readLine(sensorValues);
    }

    if (firstCylinder == false && frontDist <= 15 && leftDist <= 11 && rightDist >= 20 || firstCylinder == false && frontDist == 0 && leftDist <= 11 && rightDist > leftDist)
    {
      stopRobot();
      delay(50);
      leftServo.detach(); //just tried this since my wheels were drifting
      servoRight.detach(); //just tried this since my wheels were drifting
      leaveCylinder();
    }
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
    leftServo.write(91);
    servoRight.write(92);
  }

  void turnAround()
  {
    servoRight.write(0);
    leftServo.write (0);
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

  void onPressed()
  {
    button_time = millis();
    if (button_time - last_button_time > 250)
    {
      grip = true;
      last_button_time = button_time;
    }
  }

  void pickUpAndHold ()
  {
    servoG.write(80); // Open gripper
    delay(2000);
    int pos = 0;

    for (pos = 80; pos <= 168; pos += 1) {
      servoA.write(pos);
      delay(10);
    }
    // Down position
    delay(1000);
    servoG.write(28); // Close gripper
    delay(1000);

    for (pos = 168; pos >= 65; pos -= 1) {
      servoA.write(pos);
      delay(10);
    }

    delay(2000);
    //servoG.write(40); //open gripper
    delay(1000);
    grip = false; // no need to grip any more
    leftServo.attach(LM);  //re-attach the servos
    servoRight.attach(RM);
    //position = 9000;

    turnAround (); //turn 180 degrees because we are at a wall
    delay(2600);

    //position = qtra.readLine(sensorValues);
  }

  // do the part of picking it up
  void pickUp()
  {
    servoG.write(80); // Open gripper
    delay(2000);
    int pos = 0;

    for (pos = 80; pos <= 168; pos += 1) {
      servoA.write(pos);
      delay(10);
    }
    // Down position
    delay(1000);
    servoG.write(28); // Close gripper
    delay(1000);

    for (pos = 168; pos >= 65; pos -= 1) {
      servoA.write(pos);
      delay(10);
    }

    delay(2000);
    servoG.write(40); //open gripper
    delay(1000);
    grip = false; // no need to grip any more
    leftServo.attach(LM);  //re-attach the servos
    servoRight.attach(RM);
  }

  void leaveCylinder()
  {
    reverse();
    delay(100);

    int pos = 0;
    for (pos = 65; pos <= 168; pos += 1) {
      servoA.write(pos);
      delay(10);
    }
    // Down position
    delay(1000);
    servoG.write(40); // Close gripper
    delay(1000);

    for (pos = 168; pos >= 65; pos -= 1) {
      servoA.write(pos);
      delay(10);
    }
    firstCylinder = true;
    leftServo.attach(LM);  //re-attach the servos
    servoRight.attach(RM);
    //position = 9000;

    turnAround (); //turn 180 degrees because we are at a wall
    delay(2400);

    //position = qtra.readLine(sensorValues);
  }
