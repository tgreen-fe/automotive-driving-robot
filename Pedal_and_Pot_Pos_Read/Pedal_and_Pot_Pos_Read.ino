/*
    AUTHOR       : Mahmoud GABR & Thomas Green
    PROJECT      : Automotive Driving Robot GDP - Group 11, University of Southampton
    DATE STARTED : 21/02/2022

    
    NOTES:
    This code is used to read the position of the prototype pedal using the built-in hall effect
    sensors and the position of the potentiometer when mounted.

    MAIN HALL EFFECT SENSOR PINOUT:
    POWER(GREEN) --> 5V
    GROUND(WHITE) --> GND
    SENSE(GREY) --> A1


*/

#define HALL_MAIN A1
#define POT_IN A5


float sensePed, sensePot;

void setup() {
  Serial.begin(57600);
  Serial.print("Main_Ped_Sens:, Pot:");
  Serial.println();
  
  pinMode(HALL_MAIN, INPUT);
  pinMode(POT_IN, INPUT);
  delay(1000);
}

void loop() {

  //Read the sense pin values and store as floats. These values will be between 0 - 1023
  //sensePed = SMA(HALL_MAIN, 25.0);
  //sensePot = SMA(POT_IN, 25.0);
  
  // Printing the postitions for main and backup sensor
  Serial.print(HALL_MAIN);
  Serial.print(", ");
  //Serial.print(sensePot);
  //Serial.println();
  
}



double SMA(int reading_pin, double num_counts) {
  double sum = 0;
  
  for (double i = 0; i <= num_counts; i++) {
    sum += analogRead(reading_pin);
    delay(2);
  }
  
  return sum / num_counts;
}
