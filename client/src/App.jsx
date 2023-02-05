import { useContext } from 'react';
import Gamepad from 'react-gamepad';
import { SocketContext } from './context/socket';

import Typography from '@mui/material/Typography';
import Button from '@mui/material/Button';
import Grid from '@mui/material/Grid';

import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import KeyboardArrowDownIcon from '@mui/icons-material/KeyboardArrowDown';
import UndoIcon from '@mui/icons-material/Undo';
import RedoIcon from '@mui/icons-material/Redo';
import StopIcon from '@mui/icons-material/Stop';

const actions = [
	{ key: 'left', icon: UndoIcon, label: 'Left' },
	{ key: 'right', icon: RedoIcon, label: 'Right' },
	{ key: 'forward', icon: KeyboardArrowUpIcon, label: 'Forward' },
	{ key: 'backward', icon: KeyboardArrowDownIcon, label: 'Backward' },
	{ key: 'accelerate', icon: KeyboardArrowUpIcon, label: 'Accelerate' },
	{ key: 'decelerate', icon: KeyboardArrowDownIcon, label: 'Decelerate' },
	{ key: 'stop', icon: StopIcon, label: 'Stop' },
];

const App = () => {
	const socket = useContext(SocketContext);
	const emitSerialData = (key) => socket.emit('serialdata', key);

	return (
		<div>
			<Typography variant="h3">Arduino Zumo Control</Typography>
			<Typography variant="subtitle1">
				Click the buttons to send data to the serial port. This setup uses a
				React client and a Node server to communicate with an Arduino Zumo Bot.
			</Typography>
			<Gamepad
				onA={() => emitSerialData('backwards')}
				onB={() => emitSerialData('right')}
				onX={() => emitSerialData('left')}
				onY={() => emitSerialData('forwards')}
				onLB={() => emitSerialData('stop')}
				onLT={() => emitSerialData('decelerate')}
				onRT={() => emitSerialData('accelerate')}
				onUp={() => emitSerialData('forward')}
				onDown={() => emitSerialData('backward')}
				onLeft={() => emitSerialData('left')}
				onRight={() => emitSerialData('right')}
				onRB={() => emitSerialData('accelerate')}
				onLS={() => emitSerialData('decelerate')}
			>
				<Grid container spacing={1}>
					{actions.map(({ key, icon: Icon, label }, index) => (
						<Grid key={key} item xs={index < 3 ? 4 : 6} md={2}>
							<Button
								variant="contained"
								color="primary"
								fullWidth
								onClick={() => emitSerialData(key)}
							>
								<Icon fontSize="large" />
								{label}
							</Button>
						</Grid>
					))}
				</Grid>
			</Gamepad>
		</div>
	);
};

export default App;
