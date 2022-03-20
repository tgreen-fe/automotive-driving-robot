#include <PWMFreak.h>

/* (04/02/2022)
 * 
 * This code is to collect data from inputs and outputs of our robot and store the data in a csv file.
 * Data can be either printed to Serial in-situ, from which data is manually copied to csv
 * OR
 * Data can be stored in array in-situ. After data is collected in array, printed in Serial then copied to csv (maybe better data capture??)
 * 
 */

#define MAX_POT   460 //To be changed - maybe use callibrate()
#define MIN_POT   290 //To be changed - maybe use callibrate()
#define PED_IN    A0
#define MOTOR_PWM 9
#define MOTOR_DIR_PIN 8
#define T         100 //time between changing signals in (ms)


long pwm, acc, start;
int dir, mot_dir;


void setup() {

  setPwmFrequency(MOTOR_PWM, 1);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  Serial.begin(57600);
  randomSeed(36);
  acc = 5;
  dir = 1;

  start = millis();

}

void loop() {
  

  if ((pwm >= 255) or (pwm <= -255)){
//    acc = random(1, 5);
//    start = millis();
    dir *= -1;
    mot_dir = not mot_dir;
  }

  pwm += (acc*dir);



  Serial.print(millis());
  Serial.print(", ");
//  Serial.println(SMA(PED_IN, 5)); //Does this read any better?
  Serial.print(analogRead(PED_IN));
  Serial.print(", ");
  Serial.println(pwm);

  
  digitalWrite(MOTOR_DIR_PIN, mot_dir);
  analogWrite(MOTOR_PWM, abs(pwm));


//  if (millis() >= 5000){
//    while(1){}; 
//  }
}


double SMA(int reading_pin, double num_counts) {
  double sum = 0;
  
  for (double i = 0; i <= num_counts; i++) {
    sum += analogRead(reading_pin);
    delay(2);
  }
  
  return round((sum / num_counts));
}
