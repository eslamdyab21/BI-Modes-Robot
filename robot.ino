#include <analogWrite.h>

//for moileApp
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "RCFcE-uKKilfY_dMjUVYI9Ffb8XAfBYs";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Eslam";
char pass[] = "20302030";

//for ultrasonic
#define ULTRASONIC_TRIG_PIN     33   // pin TRIG 
#define ULTRASONIC_ECHO_PIN     32 // pin ECHO 
long duration, distance;
                                                                                                                                                                                                   
// pin values for mobile app
int pinValue1=0;
int pinValue2=0;
int pinValue3=0;
int pinValue4=0;
int selfDrivingModeButton=0;
int hardTurningModeButton=0;

// Motor A
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 

// Motor B
int motor2Pin1 = 21; 
int motor2Pin2 = 19; 
int enable1Pin2 = 18; 

const int ledPin = 5;

// Setting PWM properties
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 170;

void setup() {
  // sets the pins as outputs:
  
  Blynk.begin(auth, ssid, pass);
  //for ultrasonic
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);

  pinMode (ledPin, OUTPUT);

  //setup motorA pins
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);

  //setup motorB pins
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable1Pin2, OUTPUT);
  
  // configure pwm for motor
  ledcSetup(pwmChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable1Pin2, pwmChannel);

  Serial.begin(115200);

}

void loop() {
  
  //For Ultrasonic
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print(distance);
  Serial.println(" cm");
  
  if (selfDrivingModeButton == 1) {
    checkdistance();
  }
  

  //For Mobileapp
  Blynk.run();
}

// this function stops the robot when distance <=23 and then make it go backword then turn right then procced forward when distance >23
void checkdistance(){
  if (distance<=23){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW); 

    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW); 

    delay(2000);
    //Go backword for 1.5 seconds
    backword();
    delay(1500);
    //stop for 1.5 seconds
    stopp(pinValue1,pinValue2,pinValue3,pinValue4);
    delay(1500);
    // consider changing this part to somthing that have the abality to determin to turn left or wright 
    // maybe two IR sensors one at the right and the other on the left?
    //Turn right for one second
    right();
    delay(1000);
    //stop for 1.5 seconds
    stopp(pinValue1,pinValue2,pinValue3,pinValue4);
    delay(1500);
  }
  else{
    forward();
  }
  
}

void forward()
{
  analogWrite(enable1Pin,dutyCycle);
  analogWrite(enable1Pin2,dutyCycle);
  
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW); 

  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH); 
}

void backword()
{ 
  analogWrite(enable1Pin,dutyCycle);
  analogWrite(enable1Pin2,dutyCycle);
  
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);

  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW); 
}

void right()
{
  
  if ((distance<=23 && selfDrivingModeButton==1) || hardTurningModeButton ==1){
    analogWrite(enable1Pin,dutyCycle);
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin2, LOW); 
  }
  else{
    analogWrite(enable1Pin,dutyCycle);
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor2Pin1, LOW);
  
    analogWrite(enable1Pin2,dutyCycle-35);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin2, HIGH); 
  }
}

void left()
{
  if ((distance<=23 && selfDrivingModeButton==1) || hardTurningModeButton ==1){
    analogWrite(enable1Pin,dutyCycle);  
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin2, HIGH); 
  }
  else{
  analogWrite(enable1Pin,dutyCycle-35);
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor2Pin1, LOW); 

  analogWrite(enable1Pin2,dutyCycle);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin2, HIGH); 
  }
}

void stopp(int pin1, int pin2, int pin3, int pin4)
{
  if (pin1==0 && pin2==0 && pin3==0 && pin4==0){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor2Pin1, LOW); 
  
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin2, LOW);   
  }
}

//Forward Button
BLYNK_WRITE(V1) {
 pinValue1 = param.asInt(); // Assigning incoming value from pin V1 to a variable

 if (selfDrivingModeButton==0){
  if (pinValue1 == 1) {
    forward(); 
  } 
  else{
    stopp(pinValue1,pinValue2,pinValue3,pinValue4);
  }
 }
 
}

//Backward Button
BLYNK_WRITE(V2) {
 pinValue2 = param.asInt(); // Assigning incoming value from pin V2 to a variable

 if (selfDrivingModeButton==0){
  if (pinValue2 == 1) {
    backword(); 
  } 
  else{
    stopp(pinValue1,pinValue2,pinValue3,pinValue4);
  }
 }
 
}

//Right Button
BLYNK_WRITE(V3) {
 pinValue3 = param.asInt(); // Assigning incoming value from pin V3 to a variable

 if (selfDrivingModeButton==0){
   if (pinValue3 == 1) {
    right(); 
  }
  else{
    stopp(pinValue1,pinValue2,pinValue3,pinValue4);
  }
 }

}

//Left Button
BLYNK_WRITE(V4) {
 pinValue4 = param.asInt(); // Assigning incoming value from pin V4 to a 

 if (selfDrivingModeButton==0){
  if (pinValue4 == 1) {
    left(); 
  } 
  else{
    stopp(pinValue1,pinValue2,pinValue3,pinValue4);
  }
  
 }
  
}

//Speed control slider
BLYNK_WRITE(V6){  
  int value = param.asInt();  
  dutyCycle = value;  
}

// Self driving mode button
BLYNK_WRITE(V5) {
 selfDrivingModeButton = param.asInt(); // Assigning incoming value from pin V5 to a variable
 
 if (selfDrivingModeButton == 1) {
    digitalWrite(ledPin, HIGH); // Turn LED on.
    checkdistance(); 
  } else {
    digitalWrite(ledPin, LOW); // Turn LED off.
    stopp(pinValue1,pinValue2,pinValue3,pinValue4);
 }
}

// Hard right or left turning mode button
BLYNK_WRITE(V7) {
 hardTurningModeButton = param.asInt(); // Assigning incoming value from pin V7 to a variable
}
