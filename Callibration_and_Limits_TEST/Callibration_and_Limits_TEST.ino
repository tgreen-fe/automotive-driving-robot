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
    - Write Code to see if we can get precise measurments of speed oscillations at different PWM
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

double motor_dir;                       //Bool - for direction of rotation of motor (looking onto D-shaft: 0 = CW & 1 = CCW)
double sped = 100;                      //PWM - speed of rotation of motor
double acc = 1;                        //PWM - acceleration of motor (i.e., amount of PWM added per loop when accelerating/decel)
double max_read, min_read, curr_read;   //max & min pot reading @ full throttle & full depress respectively


void setup() {

  //  setPwmFrequency(MOTOR_PWM, 1); //pin 9 base freq is 31,250 Hz

  Serial.begin(57600);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);

  callibrate();
  min_read += 50; //compensating for wood bend
  max_read -= 50;  //compensating for wood bend
  motor_dir = LOW;

}


void loop() {

  //max  pot read is ...
  // min pot read is ...

  if (motor_dir == LOW and analogRead(POT_IN) <= max_read - 20 and sped <= 254)
  { sped += acc;  //accelerate towards full throttle
    Serial.println("If 1");
    Serial.print(motor_dir);
    Serial.print("\t");
    Serial.println(sped);
  }
  if (motor_dir == LOW and analogRead(POT_IN) >= max_read - 20 and sped >= 90) { //decelerate approaching full throttle
    sped -= 2*acc;
    Serial.println("If 2");
    Serial.print(motor_dir);
    Serial.print("\t");
    Serial.println(sped);
  }
  if (motor_dir == LOW and analogRead(POT_IN) >= max_read) {
    motor_dir = HIGH;                  //change direction to unpress
    Serial.println("If 3");
    Serial.print(motor_dir);
    Serial.print("\t");
    Serial.println(sped);
  }
  if (motor_dir == HIGH and analogRead(POT_IN) <= min_read + 20) {
    sped -= 2*acc;                 //decelerate approaching full dethrottle
    Serial.println("If 4");
    Serial.print(motor_dir);
    Serial.print("\t");
    Serial.println(sped);
  }
  if (motor_dir == HIGH and analogRead(POT_IN) <= min_read) {
    motor_dir = LOW;
    Serial.println("If 5");
    Serial.print(motor_dir);
    Serial.print("\t");
    Serial.println(sped);
  }

  if (motor_dir == LOW and analogRead(POT_IN) <= min_read) {
    digitalWrite(MOTOR_DIR_PIN, motor_dir);
    analogWrite(MOTOR_PWM, abs(sped));
    Serial.println("If 6");
    Serial.print(motor_dir);
    Serial.print("\t");
    Serial.println(sped);
  }


  //  if (analogRead(POT_IN) <= min_read) {
  //    motor_dir = LOW;
  //    sped = 255;
  //  }

  digitalWrite(MOTOR_DIR_PIN, motor_dir);
  analogWrite(MOTOR_PWM, abs(sped));




  Serial.println(analogRead(POT_IN));
  //  Serial.println(sped);

}


void callibrate(void) {

  double cal_speed = 45;

  motor_dir = LOW; //forward
  analogWrite(MOTOR_PWM, cal_speed);
  digitalWrite(MOTOR_DIR_PIN, motor_dir);

  while (true) {
    curr_read = analogRead(POT_IN);
    max_read = curr_read;
    delay(50);
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
    delay(50);
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
  delay(3000);

}
