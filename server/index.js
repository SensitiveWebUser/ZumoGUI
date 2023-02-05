import express from 'express';
import { SerialPort } from 'serialport';
import { Server as socket } from 'socket.io';

// App setup
var app = express();

var isWin = () => {
	console.log(`System that is running is ${process.platform}`);
	return process.platform == 'win32';
};

const isWired = true;

const serverPort = 8888;

const WINDOWS_XBEE_PATH = 'COM10';
const LINUX_XBEE_PATH = '/dev/ttyUSB0';

const WINDOWS_CABLE_PATH = 'COM5';
const LINUX_CABLE_PATH = '/dev/ttyUSB0';

const path = () => {
	var path = 'invalide system';
	var option = [];

	if (isWin()) {
		option = [WINDOWS_CABLE_PATH, WINDOWS_XBEE_PATH];
	} else {
		option = [LINUX_CABLE_PATH, LINUX_XBEE_PATH];
	}

	console.log(option);

	if (option.length > 0 && isWired) {
		path = option[0];
	} else if (option.length > 0) {
		path = option[1];
	}

	console.log(`choosen serial path is: ${path}`);

	return path;
};

const port = new SerialPort({
	path: path(),
	baudRate: 9600,
});

var server = app.listen(serverPort, function () {
	console.log(`listening for requests on port ${serverPort},`);
});

let io = new socket(server, { cors: { origin: '*' } });
io.on('connection', function (socket) {
	console.log(`${socket.id} is connected`);

	socket.on('serialdata', function (data) {
		console.log(data);
		port.write(data + '\n');
	});
});
