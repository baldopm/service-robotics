#include<Servo.h>
Servo leftServo;          // Define left servo
Servo servoRight;         // Define right servo

#include <QTRSensors.h>         //Pololu QTR Sensor Library

/*------ Arduino Line Follower Code----- */
/*-------definning Inputs------*/
#define echoF 6
#define trigpinF 7
#define echoL 8
#define trigpinL 9
#define echoR 4
#define trigpinR 5


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
#define ARRAY_SIZE 5;
int decisions[] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
int decisionsCount;
int nextMove;


void setup()
{
  leftServo.attach(LM);  // Set left servo to digital pin 4
  servoRight.attach(RM);  // Set right servo to digital pin 3

  Serial.begin(9600);    //Open serial port and set this baudrate

  pinMode(trigpinF, OUTPUT);
  pinMode(trigpinL, OUTPUT);
  pinMode(trigpinR, OUTPUT);
  pinMode(echoF, INPUT);
  pinMode(echoL, INPUT);
  pinMode(echoR, INPUT);

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

  }

  //Identify sideways Right Turn T-Junction
  if (position <= 1700 && sensorValues[0] > 900 && sensorValues[1] > 900 && sensorValues[2] > 900 && sensorValues[3] < 300 && sensorValues [4] < 300)
  {
    position = 9000; 
    forward();
    delay(50);
    if(sensorValues[0] > 900 && sensorValues[1] > 900 && sensorValues [0] < 500 && sensorValues[3] < 300 && sensorValues [4] < 300){
      position = 9000;  
      Serial.print("Sideways Right Turn T-Intersection");
      turnLeft();
      delay(2500);
      position = qtra.readLine(sensorValues);
    }else if(sensorValues[0] > 900 && sensorValues[1] > 900 && sensorValues[2] > 900 && sensorValues[3] < 300 && sensorValues [4] < 300){  
      turnRight();
      delay(150);
      Serial.println ("Right turn: ");
      Serial.println(position);
    }


    
//    stopRobot ();
//    delay(2000);
//
//    position = 9000;
//    
//    Serial.print("Sideways Right Turn T-Intersection");
//
//    turnRight();
//    delay(2500);
//
//    position = qtra.readLine(sensorValues);
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
//    Serial.println ("Distance to the wall: ");
//    Serial.println(distance);
//    if (distance <= 15)
//    {
//      position = 9000;
//      stopRobot();
//      delay(150);
//      turnAround (); //reverse and turn 180 degrees because we are at a wall
//      Serial.println ("Reverse: ");
//      position = qtra.readLine(sensorValues);
//    }
//    else
//    {
//      forward();
//      delay(3000);
//      turnRight();
//      delay(2000);
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
void findDistance(int &distanceF, int &distanceL, int &distanceR)
{
  // Clears the trigpin
  digitalWrite(trigpinF, LOW);
  digitalWrite(trigpinL, LOW);
  digitalWrite(trigpinR, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigpinF, HIGH);
  digitalWrite(trigpinL, HIGH);
  digitalWrite(trigpinR, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpinF, LOW);
  digitalWrite(trigpinL, LOW);
  digitalWrite(trigpinR, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long durationF = pulseIn(echoF, HIGH);
  long durationL = pulseIn(echoL, HIGH);
  long durationR = pulseIn(echoR, HIGH);
  // Calculating the distance
  distanceF = durationF * 0.034 / 2;
  distanceL = durationL * 0.034 / 2;
  distanceR = durationR * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  Serial.println("Distance Front: ");
  Serial.println(distanceF);
  Serial.println("Distance Left: ");
  Serial.println(distanceL);
  Serial.println("Distance Right: ");
  Serial.println(distanceR);
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
