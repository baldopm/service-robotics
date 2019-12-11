#include<Servo.h>

#define interruptPin 2 //interrupt sensor

Servo gripper;
Servo arm_gripper;

volatile int buttonState = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  gripper.write(60);
  arm_gripper.write(60);
  gripper.attach(12);  // attaches the servo on digital pin 9 
  arm_gripper.attach(11);  // attaches the servo on digital pin 10
  //pinMode(interruptPin, INPUT_PULLUP); //Interrupt sensor
  //attachInterrupt(digitalPinToInterrupt(interruptPin), cylinder_found, FALLING);
}

int cylinder=0;
int counter=0;

void loop() {
  arm_gripper.write(168);
  delay(2000);
  gripper.write(28);
  delay(2000);
  arm_gripper.write(60);
  delay(2000);
  gripper.write(60);
  delay(2000);


  
  if(counter!=0){
  //grip();
  }
}

void cylinder_found (){
    
    counter++;
    delay(1000);
    
    
    if (counter>=3){
      cylinder++;
      counter=0;
    }
    Serial.println(cylinder);
    
   //buttonState = digitalRead(interruptPin);
   
  //motorStop(true);
  //Gripper
  //grip();
  //motorStop(false);
}

void grip(){
  int pos_grip;
  int pos_arm;
  for (pos_arm = 60; pos_arm <= 168; pos_arm += 1) { // arm goes from the box (60 degrees) to the floor (168 degrees) to catch the cylinder
    // in steps of 1 degree
    arm_gripper.write(pos_arm);              // tell servo to go to position in variable 'pos_grip'
    delay(10);
  }
  delay(1500);                       // waits 15ms for the servo to reach the position
  //for (pos_grip = 60; pos_grip >= 25; pos_grip -= 1) { //close gripper 
    // in steps of 1 degree
    gripper.write(28);              // tell servo to go to position in variable 'pos_grip'

  //}
  delay(2500);                       // waits 15ms for the servo to reach the position
  for (pos_arm = 168; pos_arm >= 60; pos_arm -= 1) { // arm goes from the floor (180 degrees) to leave the cylinder in the box (45 degrees)
    // in steps of 1 degree
    arm_gripper.write(pos_arm);              // tell servo to go to position in variable 'pos_grip'
    delay(10);
  }
  delay(1500);                       // waits 15ms for the servo to reach the position
  //for (pos_grip = 25; pos_grip <= 60; pos_grip += 1) { //open gripper 
    // in steps of 1 degree
    gripper.write(60);              // tell servo to go to position in variable 'pos_grip'

  //} 
  delay(2500);                       // waits 15ms for the servo to reach the position
}
