#include <Servo.h>
  
  // Servos
  Servo gripper;
  Servo arm_gripper;
  Servo leftServo;
  Servo rightServo;



const int power = 250;
const int iniMotorPower = 250;
const int adj = 100;
float adjTurn = 9;
int extraInch = 200;
int adjGoAndTurn = 950;
int pos = 3600;

//Gripper function
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
}


int row, row_distance, col, col_distance; 

boolean processMotors() {
  col_distance = col - curCol;
  row_distance = row - curRow;
  return abs(col_distance) > 0 || abs(row_distance) > 0;
}

void setWheelDirection(int dir) {
  switch(dir) {
    // Foward
    case 0:
      leftServo.writeMicroseconds(1500 - (power+adj));
      rightServo.writeMicroseconds(1500 + power);
    break;
        
    // Left
    case 1:
      rotate (LEFT, 90);
    break;
    
    // Back
    case 2:
      motorStop(true);
      rotate (LEFT, 180);
    break;
    
    // Right
    case 3:
      rotate (RIGHT, 90);
    break;
  }
}


// Move forward or backward a certain number of steps REVISAR CON EL MOTOR LOS PASOS/MICROSEG
void move(int steps, boolean forward){
  setWheelDirection(forward ? 0 : 2);
  leftServo.writeMicroseconds(1500 - (power+adj));
  rightServo.writeMicroseconds(1500 + power);
  delay(steps);
  lastAction = ACTION_MOVE;
}  

// Rotate left or right a certain number of degrees
void rotate(int direction, float degrees) {
  leftServo.writeMicroseconds(1500 - (iniMotorPower+adj)*direction);
  rightServo.writeMicroseconds(1500 - iniMotorPower*direction);
  delay (round(adjTurn*degrees+1));
  motorStop(true);
  
  Serial.print("ROTATING " + String(direction) + String(degrees) + " degrees ");

  // Calculate number of steps
  //float fract = float(360.0) / degrees;
  //int steps = int(STEPS_360 / fract);
  //delay(steps);
   
  // Set last action to rotation
  lastAction = ACTION_ROTATE;
}

// Stop the motors
void motorStop(bool stop){
  if (stop == true){
  leftServo.writeMicroseconds(1500);
  rightServo.writeMicroseconds(1500);
  delay(200);
  }
}

// Move it - NEEDS EDITING
void moveTo(int row, int col) {
  // Store the current values and set new pos
  lastCol = curCol;
  lastRow = curRow;
  curCol = col;
  curRow = row;

  move(REVS_SQUARE, true);
}
