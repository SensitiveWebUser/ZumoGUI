#include <Zumo32U4.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4ProximitySensors.h>
#include <Zumo32U4Buzzer.h>

// Create instances for each of the Zumo 32U4 libraries.
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4Buzzer buzzer;

// Define the motor speed constant
const int MOTOR_SPEED = 100;

// Multiplier to change motor speed
int multiplier = 1;

// Define constants for each mode
const int MODE_ONE = 1, MODE_TWO = 2, MODE_THREE = 3;

// Set the default mode to MODE_TWO
int mode = MODE_TWO;

// Timer variables to control the LED
unsigned long prevMillis = 0;
unsigned long eventInterval = 1000;

// Flag to track if semi-autonomous mode is active
static bool isAutoActive = false;

void setup() {
  // Start the serial communication
  Serial.begin(9600);

  // Initialize the line sensors, front proximity sensor
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();

  //Play sound on start of Bot
  buzzer.play("a");
}

void loop() {
  // If there is data available from the serial, read the command and call the relevant function
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');

    switch (mode) {
      case MODE_ONE:
        manualControl(cmd);
        break;
      case MODE_TWO:
        semiAutoControl(cmd);
        break;

      case MODE_THREE:
        break;
    }

    // Flush the serial buffer
    serialFlush();
  }

  // Get the current time
  unsigned long currMillis = millis();

  // If the current time minus the previous time is greater than the event interval
  if (currMillis - prevMillis >= eventInterval) {
    // Update the previous time to the current time
    prevMillis = currMillis;

    // Turn on all the LEDs
    ledRed(1), ledYellow(1), ledGreen(1);
  }

  // If the current time minus the previous time is greater than half the event interval
  else if (currMillis - prevMillis >= eventInterval / 2) {
    // Turn off all the LEDs
    ledRed(0), ledYellow(0), ledGreen(0);
  }
}

// Function to handle manual control commands
void manualControl(String cmd) {

  // If no command is provided, return
  if (cmd == "") return;

  // Check the command and set the motor speeds accordingly
  if (cmd == "forward") {
    // Set both motors to run forward at the same speed
    motors.setSpeeds(MOTOR_SPEED * multiplier, MOTOR_SPEED * multiplier);
  } else if (cmd == "backward") {
    // Set both motors to run backward at the same speed
    motors.setSpeeds(-(MOTOR_SPEED * multiplier), -(MOTOR_SPEED * multiplier));
  } else if (cmd == "left") {
    // Set the left motor to run backward and the right motor to run forward
    motors.setSpeeds(MOTOR_SPEED * multiplier, -(MOTOR_SPEED * multiplier));
  } else if (cmd == "right") {
    // Set the left motor to run forward and the right motor to run backward
    motors.setSpeeds(-(MOTOR_SPEED * multiplier), MOTOR_SPEED * multiplier);
  } else if (cmd == "stop") {
    // Stop both motors
    motors.setSpeeds(0, 0);
  } else if (cmd == "accelerate") {
    // Decrease the speed multiplier
    updateMultiplier(multiplier + 1);
  } else if (cmd == "decelerate") {
    // Increase the speed multiplier
    updateMultiplier(multiplier - 1);
  }
}

// Function to handle semi-autonomous control commands
void semiAutoControl(String cmd) {

  // If the command is to switch between manual and semi-autonomous mode
  if (cmd == "switch") {
    // Toggle the flag that indicates if semi-autonomous mode is active
    isAutoActive = !isAutoActive;
    
    if (isAutoActive) {
      // If semi-autonomous mode is active, print a message and set the motors to run
      Serial.println("Semi-Autonomous Mode Active");
      motors.setSpeeds(MOTOR_SPEED * multiplier, MOTOR_SPEED * multiplier);
    } else {
      // If semi-autonomous mode is inactive, print a message and stop the motors
      Serial.println("Semi-Autonomous Mode Inactive");
      motors.setSpeeds(0, 0);
    }

    return;
  }

  String name = "";

  Serial1.print("Make a turn using 'L' or 'R', then press 'C' to resume. \n"); /*prints out to the user*/

  switch (cmd) {
    case 'l':
    case 'L':
      { /*If A is entered turn left*/
        name = "Left";
        Serial1.println("Left turn 90 degrees \n"); /*prints out the command to the user*/
        motors.setSpeeds(-MOTOR_TURN_SPEED, MOTOR_TURN_SPEED);
        delay(330);
        motors.setSpeeds(0, 0);
        status = false;
        autoNavigator();
        break;
      }

    case 'r':
    case 'R':
      { /*If A is entered turn left*/
        name = "Right";
        Serial1.println("Left turn 90 degrees \n"); /*prints out the command to the user*/
        motors.setSpeeds(MOTOR_TURN_SPEED, -MOTOR_TURN_SPEED);
        delay(330);
        motors.setSpeeds(0, 0);
        status = false;
        autoNavigator();
        break;
      }
    case 'b':
    case 'B':
      { /*If A is entered turn left*/
        name = "180";
        Serial1.println("Turn 180 degrees \n"); /*prints out the command to the user*/
        motors.setSpeeds(-MOTOR_TURN_SPEED, MOTOR_TURN_SPEED);
        delay(660);
        motors.setSpeeds(0, 0);
        status = false;
        autoNavigator();
        break;
      }
      return;
  }
}

// Function to update the speed multiplier
void updateMultiplier(int m) {
  // Check if the provided multiplier is within the valid range (0 to 4)
  if (m >= 0 && m <= 4) {
    // If it is, update the multiplier
    multiplier = m;
  }
}

// Function to flush the serial buffer
void serialFlush() {
  // Read and discard any data in the serial buffer
  while (Serial.available() > 0) Serial.read();
}