# Introduction

This document provides an overview of the software components involved in the development of a maze-solving Zumo Bot. The application includes a client, a server, and a zumo bot, all of which have been designed and implemented using various software tools and libraries. This document focuses on providing a more detailed description of each component.

# Client

The client is a React-based web application that has been created using the Create React App boilerplate. It is a simple, and user-friendly application that allows users to control the zumo bot and interact with the server. The client has a user interface that enables users to press buttons and use a controller to send messages to the server. The client also receives messages from the server, which it uses to update a map of the maze that the zumo bot is exploring.

## Implementation

The client has been implemented using modern front-end web development technologies, including React, material-ui, and SocketIO. It communicates with the server via the SocketIO library, which provides real-time communication capabilities. The client codebase has been organized using best practices to ensure code maintainability and readability.

## How to run

To run the client, the user needs to have node installed on their machine. They can then navigate to the client folder and run the following commands:

```bash
cd client
npm install
npm start
```

# Server

The server is a node.js application that has been developed using the express framework and the SocketIO library. The server's primary responsibility is to facilitate communication between the client and the zumo bot. The server receives messages from the client and sends them to the zumo bot, and vice versa.

## Implementation

The server has been developed using the express framework, which provides a lightweight and flexible web application framework for node.js. The SocketIO library has been used to enable real-time communication between the client and the server. The server also uses serialport to communicate with the zumo bot.

## How to run

To run the server, the user needs to have node installed on their machine. They can navigate to the server folder and run the following commands:

```bash
cd server
npm install
npm start
```

# Zumo Bot

The zumo bot is a simple robot designed to move around a maze autonomously. The zumo bot is equipped with sensors that allow it to detect walls and navigate the maze. The zumo bot is controlled by the server using the Serial Socket library. The zumo bot can send messages to the server, which are used to update the maze's status and communicate the zumo bot's location.

## Implementation

The zumo bot has been designed using the Zumo32U4 library, which provides an easy-to-use interface to control the robot. The robot's movement is based on simple commands, such as moving forward, turning left or right, and detecting walls. The robot can also communicate with the server using the Serial Socket library. The code used to interpret the Z-axis of the accelerometer is based on the code from the Pololu Zumo 32U4 library.

## How to run

To run the zumo bot, the user needs to have the Arduino IDE installed on their machine. They can then upload the code from the zumo bot folder to the zumo bot.

# Resources

The following resources were used in the development of the application:

- [Zumo Robot](https://www.pololu.com/category/148/zumo-robot-kits)
- [Arduino IDE](https://www.arduino.cc/en/software)
- [Create React App](https://reactjs.org/) - The boilerplate used to create the client.
- [Express](https://expressjs.com/) - The framework used to create the server.
- [SocketIO](https://socket.io/) - The library used to create the server.
- [Serialport](https://serialport.io/) - The library used to communicate with the zumo bot.
- [Zumo32U4](https://pololu.github.io/zumo-32u4-arduino-library/) - The library used to control the zumo bot.
- [Pololu Zumo 32U4 library](https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/MazeSolver/TurnSensor.h) - The library used to interpret the Z axis of the accelerometer.
