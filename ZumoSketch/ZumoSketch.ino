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
#define SERIAL_COM Serial1

// Motor speed variables
#define MOTOR_SPEED 100
static uint16_t multiplier = 1;
static uint16_t motorSpeed = MOTOR_SPEED * multiplier;
static uint16_t rotationSpeed = MOTOR_SPEED * 0.5f;

// Mode variables
static const uint8_t MODE_ONE = 1, MODE_TWO = 2, MODE_THREE = 3;
static uint8_t mode = MODE_ONE;

// Event LED variables
static uint64_t prevLEDMillis = 0;
static const uint16_t EVENT_LED_INTERVAL = 1000;

// Event Proximity check variables
static uint64_t prevProximityMillis = 0;
static const uint16_t EVENT_PROXIMITY_INTERVAL = 1000;
static const uint16_t EVENT_COOLDOWN_PROXIMITY_INTERVAL = 1000;
static uint8_t proximitySensorValues[6] = { 0, 0, 0, 0, 0, 0 };
static uint8_t proximityThreshold = 5;
static bool cooldown = false;

// Line sensor values
static uint16_t lineSensorValues[3] = { 0, 0, 0 };
static uint8_t lastPos = 0;
static const uint16_t threshold = 250;


// Include additional code files
#include "TurnSensor.h"
#include "Util.h"

// Setup function that runs once at the start
void setup() {
  // Initialize Serial communication
  SERIAL_COM.begin(9600);

  // Initialize line sensors and proximity sensors
  lineSensors.initThreeSensors();
  proxSensors.initThreeSensors();

  // Setup turn sensor
  turnSensorSetup();
}

// Loop function that runs continuously
void loop() {

  // Check for incoming Serial commands
  if (SERIAL_COM.available() > 0) {

    // Read the command from the Serial buffer
    String cmd = SERIAL_COM.readStringUntil('\n');

    //Sets the mode of the bot
    if (cmd == "mode1") {
      SERIAL_COM.println("c");
      mode = MODE_ONE;
    } else if (cmd == "mode2") {
      SERIAL_COM.println("c");
      mode = MODE_TWO;
    } else if (cmd == "mode3") {
      SERIAL_COM.println("c");
      mode = MODE_THREE;
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
  // "if" statments used to optimise code for compiler
  if (cmd == "forward") {
    // Set both motors to run forward at the same speed
    drawLine(0);
    motors.setSpeeds(motorSpeed, motorSpeed);
  } else if (cmd == "backward") {
    // Set both motors to run backward at the same speed
    drawLine(2);
    motors.setSpeeds(-motorSpeed, -motorSpeed);
  } else if (cmd == "left") {
    // Set the left motor to run backward and the right motor to run forward
    drawLine(3);
    motors.setSpeeds(-rotationSpeed, rotationSpeed);
  } else if (cmd == "right") {
    // Set the left motor to run forward and the right motor to run backward
    drawLine(1);
    motors.setSpeeds(rotationSpeed, -rotationSpeed);
  } else if (cmd == "stop") {
    // Stop both motors
    drawStop(false);
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

  // Sets multiplier to 1 for the movement
  updateMultiplier(1);

  // Check the command and rotate the robot accordingly
  // "if" statments used to optimise code for compiler
  if (cmd == "left") {
    // Set the left motor to run backward and the right motor to run forward (90 degree left turn)
    turnZumo(turnAngle90, -motorSpeed, motorSpeed, leftTurnCheck);
    drawLine(3);
    autoNavigator();
  } else if (cmd == "right") {
    // Set the left motor to run forward and the right motor to run backward (90 degree right turn)
    turnZumo(-turnAngle90, motorSpeed, -motorSpeed, rightTurnCheck);
    drawLine(1);
    autoNavigator();
  } else if (cmd == "backward") {
    // Set the left motor to run forward and the right motor to run backward (180 degree turn)
    turnZumo(turnAngle180, -motorSpeed, motorSpeed, leftTurnCheck);
    drawLine(2);
    autoNavigator();
  } else if (cmd == "forward") {
    drawLine(0);
    autoNavigator();
  }
}

// Function to handle auto control commands
void autoControl(String cmd) {

  bool active = true;

  // Used to control for cycle
  uint16_t cycle = 20;

  // Will work thought the maze until 20 corridors have been explored
  while (active && cycle > 0) {

    drawStop(false);

    if (SERIAL_COM.available() > 0) {
      active = false;
      break;
    }

    // Array used to check sensor values for check,
    bool pathValues[4] = { false, false, false, false };
    // Used to set value postion in array
    uint8_t postion = 0;
    
    // moves half of 45 degrees to the left then right to check for a wall
    for (uint8_t x = 0; x < 2; x++) {

      switch (x) {
        case 0:
          turnZumo(turnAngleHalf45, -motorSpeed, motorSpeed, leftTurnCheck);
          break;
        case 1:
          turnZumo(-turnAngle45 + -turnAngleHalf45, motorSpeed, -motorSpeed, rightTurnCheck);
          postion = 2;
          break;
      }

      delay(100);

      // Read the line sensor values left and right and check if they are above the threshold
      lineSensors.read(lineSensorValues);
      const uint16_t leftLineValue = lineSensorValues[0];
      const uint16_t rightLineValue = lineSensorValues[2];


      if (!(leftLineValue > threshold)) {
        pathValues[postion] = true;
      }

      if (!(rightLineValue > threshold)) {
        pathValues[postion + 1] = true;
      }
    }

    // Reset rotation
    turnZumo(turnAngle45, -motorSpeed, motorSpeed, leftTurnCheck);

    // Will in order of importance and ability turn to face the next corridor
    if (pathValues[1] == true && pathValues[2] == true) {
      drawLine(0);
      autoNavigator();
    } else if (pathValues[0] == true) {
      turnZumo(turnAngle90, -motorSpeed, motorSpeed, leftTurnCheck);
      drawLine(3);
      autoNavigator();
    } else if (pathValues[3] == true) {
      turnZumo(-turnAngle90, motorSpeed, -motorSpeed, rightTurnCheck);
      drawLine(1);
      autoNavigator();
    } else {
      turnZumo(turnAngle180, -motorSpeed, motorSpeed, leftTurnCheck);
      drawLine(2);
      autoNavigator();
    }

    // Decrease the cycle count by 1 to prevent infinite loop
    cycle--;
  }

  // Stop the drawing on the client map
  drawStop(false);
}

// Function to handle auto navigation
void autoNavigator() {

  bool active = true;

  // Used to control the speed of the robot turning
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
    //printReadingsToSerial();

    // Does a promximity check for objects
    proximityCheck();

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

  drawStop(false);
}

// Function to handle the proximity check for objects
void proximityCheck() {

  // Set current time and time since last proximity check
  const uint64_t currMillis = millis();
  const uint64_t proximityTimeUpdate = (uint64_t)(currMillis - prevLEDMillis);
  const uint64_t interval = cooldown ? EVENT_COOLDOWN_PROXIMITY_INTERVAL : EVENT_PROXIMITY_INTERVAL;

  // Check if the time since last proximity check is greater than the interval
  if (proximityTimeUpdate >= interval) {

    cooldown = false;
    bool hasObject = false;

    // Send IR pulses and read the proximity sensors.
    proxSensors.read();

    // Read the proximity sensor values
    proximitySensorValues[0] = proxSensors.countsFrontWithLeftLeds();
    proximitySensorValues[1] = proxSensors.countsFrontWithRightLeds();
    proximitySensorValues[2] = proxSensors.countsRightWithLeftLeds();
    proximitySensorValues[3] = proxSensors.countsRightWithRightLeds();
    proximitySensorValues[4] = proxSensors.countsLeftWithRightLeds();
    proximitySensorValues[5] = proxSensors.countsLeftWithLeftLeds();

    // Loop through the proximity sensor values and check if any are above the threshold
    for (uint8_t sensor = 0; sensor < 6; sensor++) {
      if (proximitySensorValues[sensor] >= proximityThreshold) {
        hasObject = true;
        break;
      }
    }

    // Play a sound and draw a point on the client map if an object is detected
    if (hasObject) {
      cooldown = true;
      drawStop(true);
      buzzer.play("a");
    }
  }
}