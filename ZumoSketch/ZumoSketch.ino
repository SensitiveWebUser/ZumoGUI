// Include libraries
#include <Zumo32U4.h>
#include <Zumo32U4Motors.h>
#include <Zumo32U4LineSensors.h>
#include <Zumo32U4ProximitySensors.h>
#include <Zumo32U4Buzzer.h>
#include <Zumo32U4IMU.h>

// Create objects
Zumo32U4ProximitySensors proxSensors;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4Buzzer buzzer;
Zumo32U4IMU imu;

// Sets Serial Communication type
#define SERIAL_COM Serial

// Motor speed variables
#define MOTOR_SPEED 100
static uint16_t multiplier = 1;
static uint16_t motorSpeed = MOTOR_SPEED * multiplier;
static uint16_t rotationSpeed = MOTOR_SPEED * 0.5f;

// Mode variables
static const uint8_t MODE_ONE = 1, MODE_TWO = 2, MODE_THREE = 3;
static uint8_t mode = MODE_TWO;

// Event LED variables
static uint64_t prevLEDMillis = 0;
static const uint16_t EVENT_LED_INTERVAL = 1000;

// Line sensor values
static uint16_t lineSensorValues[3] = { 0, 0, 0 };
static const uint16_t threshold = 800;

// Include additional code files
#include "TurnSensor.h"
#include "Util.h"

// Setup function that runs once at the start
void setup() {
  // Initialize Serial communication
  SERIAL_COM.begin(9600);

  // Initialize line sensors and proximity sensors
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();

  // Setup turn sensor
  turnSensorSetup();
}

// Loop function that runs continuously
void loop() {

  // Check for incoming Serial commands
  if (SERIAL_COM.available() > 0) {

    // Read the command from the Serial buffer
    String cmd = SERIAL_COM.readStringUntil('\n');

    // TODO: Add a command to switch between modes
    if (cmd == "switchMode") {
      mode = mode != MODE_ONE ? MODE_ONE : MODE_TWO;
      motors.setSpeeds(0, 0);
    }

    // Check the current mode and handle the command accordingly
    switch (mode) {
      case MODE_ONE:
        manualControl(cmd);
        break;
      case MODE_TWO:
        semiAutoControl(cmd);
        break;
      case MODE_THREE:
        autoControl(cmd);
        break;
    }

    SerialFlush();
  }

  // Set current time and time since last LED update
  const uint64_t currMillis = millis();
  const uint64_t ledTimeUpdate = (uint64_t)(currMillis - prevLEDMillis);

  // Update the LED every second
  if (ledTimeUpdate >= EVENT_LED_INTERVAL) {
    prevLEDMillis = currMillis;
    ledRed(1), ledYellow(1), ledGreen(1);
  } else if (ledTimeUpdate >= EVENT_LED_INTERVAL / 2) {
    ledRed(0), ledYellow(0), ledGreen(0);
  }
}

// Function to handle manual control commands
void manualControl(String cmd) {

  if (cmd == "") return;

  motorSpeed = MOTOR_SPEED * multiplier;
  rotationSpeed = motorSpeed * 0.5f;

  // Check the command and set the motor speeds accordingly
  if (cmd == "forward") {
    // Set both motors to run forward at the same speed
    motors.setSpeeds(motorSpeed, motorSpeed);
  } else if (cmd == "backward") {
    // Set both motors to run backward at the same speed
    motors.setSpeeds(-motorSpeed, -motorSpeed);
  } else if (cmd == "left") {
    // Set the left motor to run backward and the right motor to run forward
    motors.setSpeeds(-rotationSpeed, rotationSpeed);
  } else if (cmd == "right") {
    // Set the left motor to run forward and the right motor to run backward
    motors.setSpeeds(rotationSpeed, -rotationSpeed);
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

// Function to handle semi-auto control commands
void semiAutoControl(String cmd) {

  //
  SERIAL_COM.print("Make a turn using 'left' or 'right' \n");

  // Sets multiplier to 3 for the turning
  updateMultiplier(3);

  // Check the command and rotate the robot accordingly
  if (cmd == "left") {
    // Set the left motor to run backward and the right motor to run forward (90 degree left turn)
    SERIAL_COM.println("Turning left");
    turnZumo(turnAngle90, -motorSpeed, motorSpeed, leftTurnCheck);
    autoNavigator();
  } else if (cmd == "right") {
    SERIAL_COM.println("Turning Right");
    // Set the left motor to run forward and the right motor to run backward (90 degree right turn)
    turnZumo(-turnAngle90, motorSpeed, -motorSpeed, rightTurnCheck);
    autoNavigator();
  } else if (cmd == "backward") {
    // Set the left motor to run forward and the right motor to run backward (180 degree turn)
    turnZumo(turnAngle180, -motorSpeed, motorSpeed, leftTurnCheck);
    autoNavigator();
  } else if (cmd == "mode1") {
    // TODO: Remove this option
    autoNavigator();
  }
}

// Function to handle auto control commands
void autoControl(String cmd) {

  bool active = true;

  // Sets multiplier to 1 for the turning
  updateMultiplier(1);

  while (active) {

    //TODO: check front, left, right and lastly behind for open area
    bool pathValues[4] = { false, false, false, false };

    for (uint8_t x = 0; x <= 3; x++) {

      switch (x) {
        case 1:
          turnZumo(turnAngle90, -motorSpeed, motorSpeed, leftTurnCheck);
          break;
        case 2:
          turnZumo(-turnAngle180, motorSpeed, -motorSpeed, rightTurnCheck);
          break;
        case 3:
          turnZumo(turnAngle180, -motorSpeed, motorSpeed, leftTurnCheck);
          break;
      }

      lineSensors.read(lineSensorValues);
      const uint16_t lineValue = lineSensorValues[1];

      SERIAL_COM.println(lineValue);

      if (!(lineValue > threshold)) {
        pathValues[x] = true;
      }
    }

    // Reset rotation
    turnZumo(turnAngle180, -motorSpeed, motorSpeed, leftTurnCheck);

    if (pathValues[0] == true) {
      turnZumo(turnAngle90, -motorSpeed, motorSpeed, leftTurnCheck);
      autoNavigator();
    } else if (pathValues[1] == true) {
      turnZumo(-turnAngle90, motorSpeed, -motorSpeed, rightTurnCheck);
      autoNavigator();
    } else if (pathValues[2] == true) {
      turnZumo(turnAngle180, -motorSpeed, motorSpeed, leftTurnCheck);
      autoNavigator();
    }

    active = false;
  }
}

// Function to handle auto navigation
void autoNavigator() {

  bool active = false;

  float modifyer = 1.35f;

  updateMultiplier(1);

  // Loop until the robot reaches the end of a straight line
  while (active) {

    // Check for incoming Serial commands to exit the loop
    if (SERIAL_COM.available() > 0) {
      active = false;
      motors.setSpeeds(0, 0);
      break;
    }

    // Set the default motor speeds
    uint16_t leftMotor = 75;
    uint16_t rightMotor = 75;

    // Read the line sensor values
    lineSensors.read(lineSensorValues);
    printReadingsToSerial();

    // Loop through the line sensor values and check for a line
    for (uint8_t postion = 0; postion <= 2; postion++) {

      const uint16_t lineValue = lineSensorValues[postion];

      if (postion == 1 && lineValue > threshold) {
        active = false;
        leftMotor = 0;
        rightMotor = 0;
        break;
      };

      const uint64_t currMillis = millis();

      // Check if the line sensor is over a line and set the motor speeds accordingly
      if (lineValue > threshold && currMillis - prevLEDMillis > 20) {
        motors.setSpeeds(0, 0);
        switch (postion) {
          case 0:
            leftMotor = (uint16_t)(motorSpeed * modifyer);
            rightMotor = -(uint16_t)(motorSpeed * modifyer);
            prevLEDMillis = currMillis;
            break;
          case 2:
            rightMotor = (uint16_t)(motorSpeed * modifyer);
            leftMotor = -(uint16_t)(motorSpeed * modifyer);
            prevLEDMillis = currMillis;
            break;
        }
      }
    }

    // Set the motor speeds
    motors.setSpeeds(leftMotor, rightMotor);
  }
}