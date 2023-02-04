#include <Zumo32U4.h>
#include <Zumo32U4Motors.h>

Zumo32U4Motors motors;

#define MOTOR_SPEED 100

const int MODE_ONE = 1;
const int MODE_TWO = 2;
const int MODE_THREE = 3;

int multiplier = 1;
int mode = MODE_ONE;

const unsigned long eventInterval = 1000;
long previousMillis = 0;

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {

  unsigned long currentMillis = millis();
  
    if (Serial.available() > 0) {

      char input_event = Serial.read();
      Serial.flush();

      if(mode == MODE_ONE){

        switch(input_event){
          case 'q': 
            updateMultiplier(multiplier - 1);
          break;

          case 'e': 
            updateMultiplier(multiplier + 1);
          break;

        }

        switch (input_event) {
          case 'z':
            motors.setSpeeds(0, 0);
          break;

          case 'w':
            motors.setSpeeds((MOTOR_SPEED * multiplier), (MOTOR_SPEED * multiplier));
          break;

          case 'a':
            motors.setSpeeds(-(MOTOR_SPEED * multiplier), (MOTOR_SPEED * multiplier));
          break;

          case 'd':
            motors.setSpeeds((MOTOR_SPEED * multiplier), -(MOTOR_SPEED * multiplier));
          break;

          case 's':
            motors.setSpeeds(-(MOTOR_SPEED * multiplier), -(MOTOR_SPEED * multiplier));
          break;
        }  

      }
  }

  if(currentMillis - previousMillis >= eventInterval) {

    previousMillis = currentMillis;

    ledRed(1);
    ledYellow(1);
    ledGreen(1);

  }else if (currentMillis - previousMillis >= eventInterval/2){
    ledRed(0);
    ledYellow(0);
    ledGreen(0);
  }

}

void updateMultiplier(int new_multiplier) {

  Serial.println( "ultiplier: " + new_multiplier);

  if(new_multiplier >= 0 && new_multiplier <= 4) {
    multiplier = new_multiplier;
  }
}

