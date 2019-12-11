// Include the Servo library 
#include <Servo.h> 
// Declare the Servo pin 
int servoPinG = 12; //To be changed
int servoPinA = 11; 
boolean grip = false;
unsigned long button_time = 0;
unsigned long last_button_time = 0;
Servo servoG; 
Servo servoA;
Servo leftServo;          // Define left servo
Servo servoRight;
#define LM 3       // left motor
#define RM 13      // right motor
void setup() { 
   // We need to attach the servo to the used pin number 
   Serial.begin(9600);
   leftServo.attach(LM);
   servoRight.attach(RM);
   servoG.write(50);
   servoA.write(80);
   servoG.attach(servoPinG); 
   servoA.attach(servoPinA);

   attachInterrupt(digitalPinToInterrupt(2), onPressed, RISING);
   
}
void loop(){ //basically just wait untill button is pressed, this should be pretty easy to implement in the code
  if(grip){
    leftServo.detach(); //just tried this since my wheels were drifting
    servoRight.detach(); //just tried this since my wheels were drifting
    pickUp();
  }
}

void onPressed(){
  button_time = millis();
  if(button_time - last_button_time > 250){
    grip = true;
    last_button_time = button_time;
  }
}

void pickUp(){ // do the part of picking it ip
   servoG.write(60); // Open gripper
   delay(2000);
   int pos = 0;
   
   for (pos = 80; pos <= 168; pos+=1){
    servoA.write(pos);
    delay(10);
   }
    // Down position
   delay(1000);
   servoG.write(28); // Close gripper
   delay(1000);
   
   for(pos = 168; pos >= 65; pos-=1){
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
