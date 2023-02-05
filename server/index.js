import express from 'express';
import { SerialPort } from 'serialport';
import { Server as SocketIO } from 'socket.io';

const app = express();
const isWin = () => process.platform === 'win32';

const isWired = true;

const serverPort = 8888;

const paths = {
	windows: [
		{ type: 'cable', path: 'COM5' },
		{ type: 'xbee', path: 'COM10' },
	],
	linux: [
		{ type: 'cable', path: '/dev/ttyACM0' },
		{ type: 'xbee', path: '/dev/ttyUSB0' },
	],
};
const selectedPath = paths[isWin() ? 'windows' : 'linux'].find(
	(p) => p.type === (isWired ? 'cable' : 'xbee')
).path;

const serialPort = new SerialPort({
	path: selectedPath,
	baudRate: 9600,
});

const server = app.listen(serverPort, () =>
	console.log(`Listening for requests on port ${serverPort}.`)
);
const io = new SocketIO(server, { cors: { origin: '*' } });

io.on('connection', (socket) => {
	console.log(`${socket.id} is connected`);

	socket.on('serialdata', (data) => {
		console.log(data);
		serialPort.write(`${data}\n`);
	});
});

serialPort.on('open', function () {
	console.log('-- Connection opened --');
	serialPort.on('data', function (data) {
		console.log('Data received: ' + data);
	});
});
