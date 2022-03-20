#include <PWMFreak.h>


/* At max speed 255 PWM, freq = 3Hz. So fastest
  expected time to go from full depress to full
  throttle is (1/3 * 0.5 * 1000 = approx. 170 ms) @ 255 PWM.

  Also consider two things:

      - Motion may start from midpoint so time
      to full throttle could be less.
      - When moving backwards, time taken is
      less due to pedal force.

  Hence, we specify that time
  between changing of binary signal should not exceed 170ms*/

#define MOTOR_PWM     9
#define MOTOR_DIR_PIN 8
#define POT_PIN       A0

double start, Ts, sped;
bool motor_dir;

void setup() {


  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits


  //    ADCSRA |= bit (ADPS2);                               //  16
  //    ADCSRA |= bit (ADPS0) | bit (ADPS2);                 //  32
  //  ADCSRA |= bit (ADPS1) | bit (ADPS2);                 //  64
  ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);       // 128 DEFAULT

  setPwmFrequency(MOTOR_PWM, 1);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);
  Serial.begin(57600);
  randomSeed(2020);
  sped = 150;
  motor_dir = LOW;
  start = millis();
  Ts = random(100, 140);
}


void loop() {
  // put your main code here, to run repeatedly:

  if (((millis() - start) >= Ts)) {
    Ts = random(100, 140);
    start = millis();
    sped *= -1;
    motor_dir = !motor_dir;
  }

  digitalWrite(MOTOR_DIR_PIN, motor_dir);
  analogWrite(MOTOR_PWM, abs(sped));

  Serial.print(millis());
  Serial.print(",");
  Serial.print(analogRead(POT_PIN));
  Serial.print(",");
  Serial.println(sped);

  if (millis() >= 1000) {
    digitalWrite(MOTOR_DIR_PIN, LOW);
    analogWrite(MOTOR_PWM, abs(0));

    //    Serial.println(millis());
    while (1) {
    }
  }
}
