#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo
Servo gripper;
Servo arm_gripper;

#include <QTRSensors.h>         //Pololu QTR Sensor Library


/*------ Arduino Line Follower Code----- */
/*-------definning Inputs------*/
#define LS       // left sensor
#define MS 9
#define RS 8      // right sensor

#define trigpin 12
#define echo 13

/*-------definning Outputs------*/
#define LM 3       // left motor
#define RM 2       // right motor

//Create QTR Sensor Line Following Object
//QTRSensorsRC qtrrc((unsigned char[]) {LS, MS, RS}, 3, 2500, QTR_NO_EMITTER_PIN);
//unsigned int sensorValues[3];

#define NUM_SENSORS             5  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             QTR_NO_EMITTER_PIN  // emitter is controlled by digital pin 2

// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {
  0, 1, 2, 3, 4
}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

void setup()
{
  leftServo.attach(LM);  // Set left servo to digital pin 4
  servoRight.attach(RM);  // Set right servo to digital pin 3
  gripper.attach(9);  // attaches the servo on digital pin 9 
  arm_gripper.attach(10);  // attaches the servo on digital pin 10

  Serial.begin(9600);    //Open serial port and set this baudrate

  pinMode(trigpin, OUTPUT);
  pinMode(echo, INPUT);

  leftServo.write(90);
  servoRight.write(90);

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

int distance;
int cylinder = 0;
void loop()
{
  unsigned int position = qtra.readLine(sensorValues);

  if (position > 1500 && position < 2500) // position is around 1000
  {
    forward();
    Serial.println ("Forward: ");
    Serial.println(position);
  }
  else if (sensorValues[0] < 500 && sensorValues[2] < 500 && sensorValues[1] < 500 && sensorValues[3] < 500 && sensorValues[4] < 500) // stop if all are on white
  {
    //stopRobot ();
    Serial.print("All white");
    //ultrasonic sensor
    if (distance_front > dist_limit && distance_right > dist_limit && distance_left > dist_limit){
      foward();
    } else if(distance_front < dist_limit && distance_right < dist_limit && distance_left < dist_limit){
      reverse();
    } else if(distance_front < dist_limit && distance_right < dist_limit && distance_left > dist_limit){
      turnLeft();
      delay(1000);//MODIFY
      foward();
    } else if(distance_front < dist_limit && distance_right > dist_limit && distance_left < dist_limit){
      turnRight();
      delay(1000);//MODIFY
      foward();
    }
  }
  else if (sensorValues[0] > 500 && sensorValues[2] > 500 && sensorValues[1] > 500 && sensorValues[3] > 500 && sensorValues[4] > 500) // stop if all are on black
  {
//    stopRobot ();
//    Serial.print("All black stop");
      forward();
      delay(50);
      if (sensorValues[0] < 500 && sensorValues[2] < 500 && sensorValues[1] < 500 && sensorValues[3] < 500 && sensorValues[4] < 500) // stop. T junction
      {
        Serial.print("All white");
        reverse();
        delay(50);
        path();
      }
      else if (position > 1500 && position < 2500) // Cross junction
      {
        path();
      }
  }
  else if (position > 3500) // the line is on the left
  {
    forward();
    delay(50);
    path();
    //turnRight();
    delay(150);
    Serial.println ("Left turn: " );
    Serial.println (position);
  }
  else if (position < 2000)  // the line is on the right
  {
    forward();
    delay(50);
    path();
    //turnLeft();
    delay(150);
    Serial.println ("Right turn: ");
    Serial.println(position);
  }

  //if proximity sensor find a cylinder
  //stopRobot();
  //grip();
  //cylinder = cylinder + 1;
  //if(cylinder == 3){exit_path();}
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

void grip(){
  int pos_grip=0;
  int pos_arm=0;
  for (pos_arm = 95; pos_arm >= 0; pos_arm -= 1) { // arm goes from 95 degrees to 0 degrees
    // in steps of 1 degree
    gripper.write(pos_arm);              // tell servo to go to position in variable 'pos_grip'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos_grip = 0; pos_grip <= 45; pos_grip += 1) { //gripper close
    // in steps of 1 degree
    gripper.write(pos_grip);              // tell servo to go to position in variable 'pos_grip'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos_arm = 0; pos_arm <= 95; pos_arm += 1) { // arm goes from 0 degrees to 95 degrees
    // in steps of 1 degree
    gripper.write(pos_arm);              // tell servo to go to position in variable 'pos_grip'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos_grip = 45; pos_grip >= 0; pos_grip -= 1) { //gripper open
    // in steps of 1 degree
    gripper.write(pos_grip);              // tell servo to go to position in variable 'pos_grip'
    delay(15);                       // waits 15ms for the servo to reach the position
  } 
}#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo

#include <QTRSensors.h>         //Pololu QTR Sensor Library


/*------ Arduino Line Follower Code----- */
/*-------definning Inputs------*/
#define LS       // left sensor
#define MS 9
#define RS 8      // right sensor

#define trigpin 12
#define echo 13

/*-------definning Outputs------*/
#define LM 3       // left motor
#define RM 2       // right motor

//Create QTR Sensor Line Following Object
//QTRSensorsRC qtrrc((unsigned char[]) {LS, MS, RS}, 3, 2500, QTR_NO_EMITTER_PIN);
//unsigned int sensorValues[3];

#define NUM_SENSORS             5  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             QTR_NO_EMITTER_PIN  // emitter is controlled by digital pin 2

// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {
  0, 1, 2, 3, 4
}, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];

void setup()
{
  leftServo.attach(LM);  // Set left servo to digital pin 4
  servoRight.attach(RM);  // Set right servo to digital pin 3

  Serial.begin(9600);    //Open serial port and set this baudrate

  pinMode(trigpin, OUTPUT);
  pinMode(echo, INPUT);

  leftServo.write(90);
  servoRight.write(90);

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

int distance;
void loop()
{
  unsigned int position = qtra.readLine(sensorValues);

  if (position > 1500 && position < 2500) // position is around 1000
  {
    forward();
    Serial.println ("Forward: ");
    Serial.println(position);
  }
  else if (sensorValues[0] < 500 && sensorValues[2] < 500 && sensorValues[1] < 500) // stop if all are on white
  {
    //stopRobot ();
    Serial.print("All white");
  }
  else if (sensorValues[0] > 500 && sensorValues[2] > 500 && sensorValues[1] > 500) // stop if all are on black
  {
    stopRobot ();
    Serial.print("All black stop");
  }
  else if (position > 3500) // the line is on the left
  {
    turnRight();
    delay(150);
    Serial.println ("Left turn: " );
    Serial.println (position);
  }
  else if (position < 2000)  // the line is on the right
  {
    turnLeft();
    delay(150);
    Serial.println ("Right turn: ");
    Serial.println(position);
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
