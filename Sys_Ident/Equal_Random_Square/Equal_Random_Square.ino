#include <PWMFreak.h>

/* (04/02/2022)

   This code is to collect data from inputs and outputs of our robot and store the data in a csv file.
   Data can be either printed to Serial in-situ, from which data is manually copied to csv
   OR
   Data can be stored in array in-situ. After data is collected in array, printed
   in Serial then copied to csv (maybe better data capture??)

*/

#define MAX_POT       460 //To be changed - maybe use callibrate()
#define MIN_POT       290 //To be changed - maybe use callibrate()
#define PED_IN        A0
#define MOTOR_PWM     9
#define MOTOR_DIR_PIN 8
#define T             50 //time between changing signals in (ms)



long pwm, V, start, start2;
int dir, mot_dir;
int min_ped, max_ped, max_pwm;


void setup() {

  setPwmFrequency(MOTOR_PWM, 1);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  Serial.begin(57600);
  randomSeed(9);
  dir = 1;
  mot_dir = HIGH;

  start = millis();
  start2 = millis();

  min_ped = 340;
  max_ped = 900;
  max_pwm = 100;

}

void loop() {



  if (millis() - start2 >= (2 * T)) {
    dir *= -1; // change direction
    pwm = random(40, max_pwm);
    pwm = pwm * dir;
    mot_dir = not mot_dir;
    start2 = millis();
    start = millis();
  }

  if (millis() - start >= T) {
    dir *= -1; // change direction
    pwm = pwm * dir;
    mot_dir = not mot_dir;
    start = millis();
  }


  Serial.print(millis());
  //  Serial.print(max_pwm);
  //  Serial.print(", ");
  //  Serial.print(-max_pwm);
  Serial.print(", ");
  //  Serial.print(SMA(PED_IN, 20)); //Does this read any better?
  Serial.print(analogRead(PED_IN));
  Serial.print(", ");
  Serial.println(pwm);
  //  Serial.println(mot_dir);


  digitalWrite(MOTOR_DIR_PIN, mot_dir);
  analogWrite(MOTOR_PWM, abs(pwm));


  if (millis() >= 5000) {
    while (1) {
      analogWrite(MOTOR_PWM, 0);
    };
  }
}


double SMA(int reading_pin, double num_counts) {
  double sum = 0;

  for (double i = 0; i <= num_counts; i++) {
    sum += analogRead(reading_pin);
    delay(2);
  }

  return sum / num_counts;
}
