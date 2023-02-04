import express from "express";
import { SerialPort, ReadlineParser } from "serialport";
import { Server as socket } from "socket.io";

// App setup
var app = express();

const port = new SerialPort({
  path: "/dev/ttyACM0",
  baudRate: 9600,
});

var server = app.listen(8888, function () {
  console.log("listening for requests on port 8888,");
});

let io = new socket(server, { cors: { origin: "*" } });
io.on("connection", function (socket) {
  console.log(`${socket.id} is connected`);

  socket.on("serialdata", function (data) {
    console.log(data);
    port.write(data);
  });

  const parser = port.pipe(new ReadlineParser());
  parser.read;
});
