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
static const uint8_t MOTOR_SPEED = 50;

static const float ROTATION_SPEED = 1.2f;

// Multiplier to change motor speed
static uint16_t multiplier = 1;

// Define constants for each mode
static const int8_t MODE_ONE = 1, MODE_TWO = 2, MODE_THREE = 3;

// Set the default mode to MODE_TWO
static int8_t mode = MODE_TWO;

// Timer variables to control the LED
static uint64_t prevMillis = 0;
static const uint16_t EVENT_LED_INTERVAL = 1000;

// Flag to track if semi-autonomous mode is active
static bool isAutoActive = false;

static uint16_t lineSensorValues[3] = { 0, 0, 0 };
static uint16_t lastLineSensorValues[3] = { 0, 0, 0 };
static const uint16_t threshold = 700;

char stringBuffer[200];

void setup() {
  // Start the serial communication
  Serial.begin(9600);

  // Initialize the line sensors, front proximity sensor
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();

  //Play sound on start of Bot
  //buzzer.play("a"); /* TODO: Uncomment
}

void loop() {
  // If there is data available from the serial, read the command and call the relevant function
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');

    if (cmd == "switchMode") {
      mode = mode != MODE_ONE ? MODE_ONE : MODE_TWO;
      motors.setSpeeds(0, 0);
    }

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
  const uint64_t currMillis = millis();

  // If the current time minus the previous time is greater than the event interval
  if (currMillis - prevMillis >= EVENT_LED_INTERVAL) {
    // Update the previous time to the current time
    prevMillis = currMillis;

    // Turn on all the LEDs
    ledRed(1), ledYellow(1), ledGreen(1);
  }

  // If the current time minus the previous time is greater than half the event interval
  else if (currMillis - prevMillis >= EVENT_LED_INTERVAL / 2) {
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
    motors.setSpeeds((uint16_t)(MOTOR_SPEED * multiplier), (uint16_t)(MOTOR_SPEED * multiplier));
  } else if (cmd == "backward") {
    // Set both motors to run backward at the same speed
    motors.setSpeeds(-(uint16_t)(MOTOR_SPEED * multiplier), -(uint16_t)(MOTOR_SPEED * multiplier));
  } else if (cmd == "left") {
    // Set the left motor to run backward and the right motor to run forward
    motors.setSpeeds((uint16_t)((MOTOR_SPEED * multiplier) * ROTATION_SPEED), -(uint16_t)((MOTOR_SPEED * multiplier) * ROTATION_SPEED));
  } else if (cmd == "right") {
    // Set the left motor to run forward and the right motor to run backward
    motors.setSpeeds(-(uint16_t)((MOTOR_SPEED * multiplier) * ROTATION_SPEED), (uint16_t)((MOTOR_SPEED * multiplier) * ROTATION_SPEED));
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
      autoNavigator();
    } else {
      // If semi-autonomous mode is inactive, print a message and stop the motors
      Serial.println("Semi-Autonomous Mode Inactive");
      motors.setSpeeds(0, 0);
    }

    return;
  }

  Serial.print("Make a turn using 'left' or 'right' \n"); /*prints out to the user*/
  //updateMultiplier(4);

  if (cmd == "left") {
    Serial.println("Left turn 90 degrees \n");
    motors.setSpeeds((MOTOR_SPEED * multiplier), -(MOTOR_SPEED * multiplier));
    delay(330);
    motors.setSpeeds(0, 0);
    autoNavigator();
  } else if (cmd == "right") {
    Serial.println("Left turn 90 degrees \n");
    motors.setSpeeds(-(MOTOR_SPEED * multiplier), (MOTOR_SPEED * multiplier));
    delay(330);
    motors.setSpeeds(0, 0);
    autoNavigator();
  } else if (cmd == "backward") {
    Serial.println("Turn 180 degrees \n");
    motors.setSpeeds(-MOTOR_SPEED, MOTOR_SPEED);
    delay(660);
    motors.setSpeeds(0, 0);
    autoNavigator();
  }
}

void autoNavigator() {

  bool active = true;

  float modifyer = 2.5f;

  while (active) {

    if (Serial.available() > 0) {
      active = false;
      motors.setSpeeds(0, 0);
      break;
    }

    uint16_t leftMotor = 50;
    uint16_t rightMotor = 50;

    lineSensors.read(lineSensorValues);
    printReadingsToSerial();

    for (uint8_t x = 0; x <= 2; x++) {

      const uint16_t lineValue = lineSensorValues[x];
      const uint16_t lastLineValue = lastLineSensorValues[x];

      if (lineValue > threshold) {
        switch (x) {
          case 0:
            leftMotor = (uint16_t)(MOTOR_SPEED * modifyer);
            rightMotor = -(uint16_t)(MOTOR_SPEED * modifyer);
            Serial.println("Left Sensor Hit");
            break;
          case 1:
            leftMotor = 0;
            rightMotor = 0;
            active = false;
            Serial.println("Front Sensor Hit");
            break;
          case 2:
            rightMotor = (uint16_t)(MOTOR_SPEED * modifyer);
            leftMotor = -(uint16_t)(MOTOR_SPEED * modifyer);
            Serial.println("Right Sensor Hit");
            break;
        }

        if (!active) break;
      } 
      /*
      else if (x == 1 && (uint16_t)(lineValue * threshold) > lastLineValue) {
        leftMotor = 0;
        rightMotor = 0;
        active = false;
        Serial.println("Front Sensor Hit");
        break;
      }
      */

      sprintf(stringBuffer, "Line value: %d Last Line value: %d x value: %d lspeed: %d rspeed: %d\n",
              lineValue,
              lastLineValue,
              x,
              leftMotor,
              rightMotor);
      Serial.print(stringBuffer);
    }

    memcpy(lastLineSensorValues, lineSensorValues, 3 * sizeof(uint16_t));

    //if (!active) break;

    motors.setSpeeds(leftMotor, rightMotor);
  }
}

void printReadingsToSerial() {
  sprintf(stringBuffer, "Values: %d %d %d Last: %d %d %d\n",
          lineSensorValues[0],
          lineSensorValues[1],
          lineSensorValues[2],
          lastLineSensorValues[0],
          lastLineSensorValues[1],
          lastLineSensorValues[2]);
  Serial.print(stringBuffer);
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