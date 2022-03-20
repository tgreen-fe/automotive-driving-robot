#include <PWMFreak.h>


/*
    AUTHOR       : Mahmoud GABR & Thomas Green
    PROJECT      : Automotive Driving Robot GDP - Group 11, University of Southampton
    DATE STARTED : 21/02/2022

    
    NOTES:
    This code is used to read the position of the prototype pedal using the built-in hall effect
    sensors and the position of the potentiometer when mounted.

    Pedal Sensor Pinout:
    PWR (GREEN)
    GND (WHITE)
    SIG (GREY)

    Pot Pinout:
    PWR (...) - Left-Most pin looking at back of pot
    GND (...)
    SIG (...)


    To Do:
    - Does changing PWM freq make any difference (LESS NOISE, LESS BACKLASH, ...?)
    - Remove any Serial.print() & Delay()
    - Maybe find minimum PWM req to push pedal
    - Need to consider exageration in min max values due to wood bending
    - Adjust speed change when approaching limits
    

*/



#define POT_IN A0
#define PED_IN A1
#define MOTOR_DIR_PIN 8
#define MOTOR_PWM     9

double motor_dir;                       //Bool value for direction of rotation of motor (looking onto D-shaft: 0 = CW & 1 = CCW)
double sped = 50;                      //PWM value speed of rotation of motor
double max_read, min_read, curr_read;   //max & min pot reading @ full throttle & full depress respectively


void setup() {
  
  setPwmFrequency(MOTOR_PWM, 1); //pin 9 base freq is 31,250 Hz
  
  Serial.begin(57600);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);

  callibrate();
  min_read += 20;
  max_read -= 10;

}


void loop() {

  //max  pot read is ...
  // min pot read is ...


  if (analogRead(POT_IN) <= min_read) {
    motor_dir = LOW;
    sped = 50;
  }
  
  digitalWrite(MOTOR_DIR_PIN, motor_dir);
  analogWrite(MOTOR_PWM, abs(sped));
  if (motor_dir == LOW and analogRead(POT_IN) >= min_read + 50) sped = 50;

  if (analogRead(POT_IN) >= max_read) motor_dir = HIGH;
  if (motor_dir == HIGH and analogRead(POT_IN) <= min_read + 50) sped = 50;
  digitalWrite(MOTOR_DIR_PIN, motor_dir);
  analogWrite(MOTOR_PWM, abs(sped));

  


  Serial.println(analogRead(POT_IN));
//  Serial.println(sped);

}

void callibrate(void) {

  double cal_speed;

  //Finding minimum speed required to move crank
  
  for(cal_speed=30; cal_speed<255; cal_speed++){
    
    motor_dir = LOW; //forward
    analogWrite(MOTOR_PWM, cal_speed);
    digitalWrite(MOTOR_DIR_PIN, motor_dir);

    curr_read = analogRead(POT_IN);
    max_read = curr_read;
    delay(1000);
    curr_read = analogRead(POT_IN);

    if(curr_read > max_read+55){
      Serial.print("Min speed is:");
      Serial.println(cal_speed);
      break;
      motor_dir = HIGH; //backward
      analogWrite(MOTOR_PWM, cal_speed);
      digitalWrite(MOTOR_DIR_PIN, motor_dir);
      delay(500);
      analogWrite(MOTOR_PWM, 0);
    }
    else{
      motor_dir = HIGH; //backward
      analogWrite(MOTOR_PWM, cal_speed);
      digitalWrite(MOTOR_DIR_PIN, motor_dir);
      delay(500);
      analogWrite(MOTOR_PWM, 0);
    }
    Serial.println(cal_speed);
    
  }

  //End of finding minimum speed

  
  //delay(2000);
  motor_dir = LOW; //forward
  analogWrite(MOTOR_PWM, cal_speed);
  digitalWrite(MOTOR_DIR_PIN, motor_dir);

  while (true) {
    curr_read = analogRead(POT_IN);
    max_read = curr_read;
    delay(40);
    curr_read = analogRead(POT_IN);
    if (max_read == curr_read) {
      analogWrite(MOTOR_PWM, 0);
      delay(1000);
      break;
    }
    else {
      Serial.println(max_read);
      continue;
    }
  }

  motor_dir = HIGH; //backwards
  analogWrite(MOTOR_PWM, cal_speed);
  digitalWrite(MOTOR_DIR_PIN, motor_dir);

  while (true) {
    curr_read = analogRead(POT_IN);
    min_read = curr_read;
    delay(40);
    curr_read = analogRead(POT_IN);
    if (min_read == curr_read) {
      analogWrite(MOTOR_PWM, 0);
      break;
    }
    else {
      Serial.println(min_read);
      continue;
    }
  }

  Serial.print("Max Read is");
  Serial.println(max_read);
  Serial.print("Min Read is");
  Serial.println(min_read);
  delay(2000);

}
