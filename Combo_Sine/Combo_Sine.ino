/*

    AUTHOR       : Mahmoud GABR
    PROJECT      : GDP - Group 11, University of Southampton
    DATE STARTED : 28/01/2022
    Revision     : 3


    NOTES:
    This code is the first step where the PID controls the position of the motor shaft using the feedback from the
    pot mounted (somehow) to the motor's shaft. There is another a copy of this sketchbook called "Closed_Loop_Pedal"
    where the PID controls the position of the throttle pedal by using the feedback from the pedal's own sensor.


    TO DO:
    Below are further developments that should/could be added to the code.

    1. Add Interrupt Service Routine (ISR) for E-Stop ----------------------------------------------------------> DONE
    2. (Maybe) Change setpoint such that it's % of full throttle (i.e. 0-100)-----------------------------------> DONE
    3. Understand <Oversampling.h> AND <SmoothADC.h> to choose one
    4. Find a way to wire up a pull-up resistor with a button for E-Stop
    5. IMPORTANT: Code is setup and PID tuned such that it controls position of motor pot.
       But setpoint of motor pot is calculated from the user-given % of full-throttle (setpoint)
       Hence, we need to have the TF relating pot position to pedal position (probs through Sys-ID method)
    6. TEST if code still works after uncommenting ISR and having setpoint as % (i.e., after removing
       changes done in initial testing)
    7. TEST performance with prescaler set to 8 instead of 1
    8. TEST different PID sample times

    After 14/2/2022
    Pot reading at vertical is 807

*/

/* DIRECTIONALITY
    Looking onto the pot's knob:
        - if knob rotate CW (motor CCW), pid_in goes UP.
        - if knob rotate CCW (motor CW), pid_in goes DOWN

        donc: if overshoot, rotate motor CW. and: if undershoot roatet motor CCW
*/


#include <PWMFreak.h>
#include <PID_v1.h>

#define POT_PIN       A0                    //pin reading feedback from pot connected to motor shaft
#define MOTOR_DIR_PIN 8                     //pin controlling direction of motor rotation (digital)
#define MOTOR_PWM     9                     //pin controlling motor speed (pin adjusting freq must not affect delay/millis fn)
#define E_STOP_PIN    2                     //pin for attachInterrupt() to control e_stop button status
#define MAX_PER       100
#define MIN_PER       0
#define MARGIN        2                   //erro less than MARGIN is ignored and set to 0

#define HALL_MAIN A1
#define HALL_BACKUP A2
#define VOLTAGE 5
#define MAX_MAIN_VOLTAGE 4.30               //These values need to be found experimentally, they corrospond to the
#define MIN_MAIN_VOLTAGE 1.59               //senseMain and senseBackup voltage when the pedal is fully depressed(MAX)
#define MAX_BACKUP_VOLTAGE 4.12             //and fully released(MIN).
#define MIN_BACKUP_VOLTAGE 1.57


volatile bool power_off = false;
byte motor_dir;                           //(0) -> forward           ; (1) -> reverse
byte res;                                 //resolution of sine wave
float f;
double perce_set;                         //desired shaft position as % of the 240 deg max pot rotation
double max_read, min_read, curr_read;     //(FOR Calibration - max & min pot reading @ full throttle & full depress respectively

double pid_set, pid_in, pid_out;          //pid_setpoint (0-1024), input (0-1024) & output (0-255, changeable if needed)

//double Kp = 25, Ki = 175., Kd = 0.1;    //specifying values of PID gains. Best values so far {Kp = 10.2, Ki = 120., Kd = 0.19}

//double Kp = 30., Ki = 0., Kd = 0.04;    //Testing of Tuning algo
//double Kp = 17., Ki = 0., Kd = 0.01;      //Testing of Tuning algo - Step 4
double Kp = 11.1, Ki = 0.093, Kd = 0;    //BEST SO FAR (14/3/22)


PID myPID(&pid_in, &pid_out, &pid_set,
          Kp, Ki, Kd, DIRECT);            //creating PID Object

double t, start, sine_set, start_mill;

int count;
void setup() {

  delay(500);

  Serial.begin(57600);
  Serial.println();


  /*plotting legends */
  //  Serial.print("SET_PERC:,SETPOINT:,CURRENT_POSITION:,OUPUT:");
  Serial.println();


  pinMode(POT_PIN, INPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(E_STOP_PIN, INPUT_PULLUP);

  /* Kill Switch */
  attachInterrupt(digitalPinToInterrupt(E_STOP_PIN), standby, LOW);
  myPID.SetOutputLimits(-255, 255); //having -ve output allows system to reverse output when overshot instead of just turning off


  //analogReference(INTERNAL);              //DEFAULT Seems to work better! INTERNAL to use 1.1 V internal reference, DEFAULT to use 5V Vcc
  setPwmFrequency(MOTOR_PWM, 1);            //set prescaler to 8 i.e., PWM freq is 7812 Hz pins 5&6 (OR 3906 Hz other pins)
  burn_8_readings();                        //burns the first few analog readings which are likely innacurate

  Serial.setTimeout(50);

  myPID.SetMode(AUTOMATIC);                 //turn PID on in Automatic mode
  myPID.SetSampleTime(10);                  //the default is 100 ms, but reduced is better performance?

  pid_in = analogRead(POT_PIN);

  calibrate();
  min_read += 10;
  max_read -= 10;
  perce_set = 0;
  pid_set = percToAnalog(perce_set);        //map initial setpoint from 0-100 onto 0-240

  /* To compensate for bending of wood in backward callibration*/
  //  while (analogRead(POT_PIN) < min_read) {
  //    digitalWrite(MOTOR_DIR_PIN, LOW);
  //    analogWrite(MOTOR_PWM, abs(42));
  //  }

  /* ensure idle start of motor */
  digitalWrite(MOTOR_DIR_PIN, LOW);
  analogWrite(MOTOR_PWM, abs(0));

  //  Serial.print("SETPOINT:,SET_PERC:,CURRENT_POSITION:,OUPUT:");
  Serial.println("SETPOINT:,CURRENT_POSITION:");
  //  Serial.println();

  t = 0;
  f = 3.0;
  // Runtime = 4000 ms; speeds [-255, 255]
  res = 1;
  start = millis();
  count = 0;
//    max_read = 616 - 10; min_read = 527 + 10;
}




void loop() {

  //  if (count == 2) while (1) {};

  if (count == 0) {
    t = 0;
    start_mill = millis();
    start = micros();
    count++;
  }



  /* setpoint as sine wave, freq. "f" & resolution "res" */
  sine_set = 50 + (50 * sin((2 * M_PI * (f / 6) * t * M_PI / 180)));
  t += 1;
  //  if (millis() - start_mill >= res) {
  //    t += 1;
  //          start_mill = millis();
  //  }
  pid_set = percToAnalog(sine_set);


  if (millis() - start_mill >= 40000) {
    digitalWrite(MOTOR_DIR_PIN, LOW);
    analogWrite(MOTOR_PWM, abs(0));
    while (1) {};
  }


  /* take input & check it's within range and transform to pid_set */
  if (Serial.available())
  {
    perce_set = Serial.parseFloat(); //original
    if (perce_set > MAX_PER or perce_set < MIN_PER) Serial.println("Input out of range (0-100)");
    else pid_set = percToAnalog(perce_set);
  }


  /* Reading current pot (or pedal) position */
  //    pid_in = SMA(POT_PIN, 25.0);
  pid_in = analogRead(POT_PIN);
  //  pid_in = analogRead(HALL_MAIN);


  /* Set error to zero if within margin to avoid fluctuations */
  if (abs(pid_in - pid_set) <= MARGIN) {
    pid_in = pid_set;
  }

  /* Compute PID output and write it to motor */
  myPID.Compute(); // N.B: In Original, used to compute inside if-stat and/or else-stat independently


  if (pid_out > 0) motor_dir = LOW;       //turn pot CCW (I THINK)
  else if (pid_out < 0) motor_dir = HIGH; // CW (I THINK)
  digitalWrite(MOTOR_DIR_PIN, motor_dir);
  analogWrite(MOTOR_PWM, abs(pid_out));

  while (power_off) {}

  /* For testing behaviour */
  //pid_in = min_read+5;
  //if (millis() > 2500) pid_in = 700;
  //if (millis() > 5500) pid_in = 300;
  //pid_in = random(min_read, max_read);

  /* Plotting (only when no other Serial values are printed) */
  //    Serial.print(millis() - start_mill);
  //    Serial.print(",");
  //  Serial.print(micros() - start);
  //  Serial.print(",");
  //    Serial.print(perce_set);
  //    Serial.print(", ");
  Serial.print(pid_set);
  Serial.print(", ");
  //  Serial.println(sine_set);
  //  Serial.println(",");
  Serial.print(pid_in);
  //    Serial.print(",");
  //  Serial.print(pid_out);
  //  Serial.print(",");
  //  Serial.print(power_off);
  Serial.println();


}





double analogToPerc(double analog_val) { //assumes analog_val is between (0-1024)
  return (analog_val * 100.0) / 1024.0;
}


/****************************************************************/


double percToAnalog(double perc_val) { //assumes perc_val is between (0-100)
  /**************************************************************************************************
     The PID controller will control the angle of th e shaft of the motr between approx. 0 to 20 deg.
     However, it will read input pot values between 0 to 1024 and by default, output is between 0-255
     Since we specify our setpoint as pedal position/angle in %, we must first map it to range
     from 0-1024, so the PID can compare it to the pot reading.
   **************************************************************************************************/
  return floor((perc_val / 100.0) * (max_read - min_read)) + min_read;
}


/****************************************************************/


double SMA(int reading_pin, double num_counts) {
  double sum = 0;

  for (double i = 0; i <= num_counts; i++) {
    sum += analogRead(reading_pin);
    delay(2);
  }

  return sum / num_counts;
}

void burn_8_readings(void) {
  // burns the first few analog readings which R likely innacurate
  int i;
  for (i = 0; i < 8; i++) {
    analogRead(POT_PIN);
  }
}


/****************************************************************/


void standby() {
  Serial.print("E-Stop Activated. Pin2 = ");
  Serial.println(digitalRead(power_off));
  while (pid_in > min_read) {
    pid_in = analogRead(POT_PIN);
    digitalWrite(MOTOR_DIR_PIN, HIGH);
    analogWrite(MOTOR_PWM, 80);
  }

  digitalWrite(MOTOR_DIR_PIN, 0);
  analogWrite(MOTOR_PWM, 0);

  power_off = false; // this allows for turning back on once stopped
  //  exit(0);
}


/****************************************************************/


void calibrate(void) {

  double cal_speed = 40;

  motor_dir = LOW; //forward
  analogWrite(MOTOR_PWM, cal_speed);
  digitalWrite(MOTOR_DIR_PIN, motor_dir);

  while (true) {
    curr_read = analogRead(POT_PIN);
    max_read = curr_read;
    delay(30);
    curr_read = analogRead(POT_PIN);
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
  analogWrite(MOTOR_PWM, cal_speed - 15);
  digitalWrite(MOTOR_DIR_PIN, motor_dir);

  while (true) {
    curr_read = analogRead(POT_PIN);
    min_read = curr_read;
    delay(40);
    curr_read = analogRead(POT_PIN);
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
  delay(500);

}
