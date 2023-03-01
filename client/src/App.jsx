import { useContext, useState } from 'react';
import Gamepad from 'react-gamepad';
import { SocketContext } from './context/socket';

import Grid from '@mui/material/Grid';
import Button from '@mui/material/Button';
import Box from '@mui/material/Box';
import Checkbox from '@mui/material/Checkbox';
import Typography from '@mui/material/Typography';

import ControlButton from './components/ControlButton';
import Canvas from './components/Canvas';

import Filter1Icon from '@mui/icons-material/Filter1';
import Filter2Icon from '@mui/icons-material/Filter2';
import Filter3Icon from '@mui/icons-material/Filter3';
import KeyboardArrowDownIcon from '@mui/icons-material/KeyboardArrowDown';
import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import RedoIcon from '@mui/icons-material/Redo';
import StopIcon from '@mui/icons-material/Stop';
import UndoIcon from '@mui/icons-material/Undo';

// Used to map the gamepad buttons to serial data sent to the Arduino
const actions = [
	{ key: 'left', icon: UndoIcon, label: 'Left' },
	{ key: 'right', icon: RedoIcon, label: 'Right' },
	{ key: 'forward', icon: KeyboardArrowUpIcon, label: 'Forward' },
	{ key: 'backward', icon: KeyboardArrowDownIcon, label: 'Backward' },
	{ key: 'accelerate', icon: KeyboardArrowUpIcon, label: 'Accelerate' },
	{ key: 'decelerate', icon: KeyboardArrowDownIcon, label: 'Decelerate' },
	{ key: 'stop', icon: StopIcon, label: 'Stop' },
	{ key: 'mode1', icon: Filter1Icon, label: 'mode 1' },
	{ key: 'mode2', icon: Filter2Icon, label: 'mode 2' },
	{ key: 'mode3', icon: Filter3Icon, label: 'mode 3' },
	{ key: 'SS', icon: Filter3Icon, label: 'ss' },
];

const App = () => {
	const [hasBoxlayout, setHasBoxlayout] = useState(false);
	const [color, setColor] = useState('white');
	const [value, setValue] = useState(1);
	const [direction, setDirection] = useState('stop');

	const socket = useContext(SocketContext);
	const emitSerialData = (key) => socket.emit('serialdata', key);

	socket.on('data', (message) => {
		console.log(`message received: ${message}`);
		const fix = `${message[0]}`;
		switch (fix) {
			case 'u':
				setDirection('up');
				break;
			case 'd':
				setDirection('down');
				break;
			case 'l':
				setDirection('left');
				break;
			case 'r':
				setDirection('right');
				break;
			case 's':
				setDirection('stop');
				break;
			case 'p':
				setDirection('point');
				break;
			case 'c':
				setDirection('clear');
				break;
			default:
		}
		console.log(direction);
	});

	return (
		<Box>
			<Typography variant="h3">Arduino Zumo Control</Typography>
			<Typography variant="subtitle1">
				Click the buttons to send data to the serial port. This setup uses a
				React client and a Node server to communicate with an Arduino Zumo Bot.
			</Typography>

			<Typography pt={2} variant="h4">
				Controls
			</Typography>

			<Typography variant="subtitle1">
				<Checkbox
					checked={hasBoxlayout}
					onChange={() => setHasBoxlayout(!hasBoxlayout)}
				/>
				Box/Debug layout
			</Typography>

			<Gamepad
				onA={() => emitSerialData('mode1')}
				onB={() => emitSerialData('right')}
				onX={() => emitSerialData('left')}
				onY={() => emitSerialData('forwards')}
				//
				onLB={() => emitSerialData('stop')}
				onLT={() => emitSerialData('decelerate')}
				onRT={() => emitSerialData('accelerate')}
				//
				onUp={() => emitSerialData('forward')}
				onDown={() => emitSerialData('backward')}
				onLeft={() => emitSerialData('left')}
				onRight={() => emitSerialData('right')}
				//
				onRB={() => emitSerialData('accelerate')}
				onLS={() => emitSerialData('decelerate')}
			>
				<Box>
					{hasBoxlayout && (
						<Grid container spacing={1}>
							{actions.map(({ key, icon: Icon, label }, index) => (
								<Grid key={key} item xs={index < 3 ? 4 : 6} md={2}>
									<ControlButton
										icon={Icon}
										text={label}
										onClick={() => emitSerialData(key)}
									/>
								</Grid>
							))}
						</Grid>
					)}

					<Grid
						container
						spacing={2}
						mt={2}
						alignItems="center"
						justifyContent="center"
					>
						<Grid item xs={4} md={2}>
							<ControlButton
								icon={Filter1Icon}
								text="Mode 1"
								onClick={() => emitSerialData('mode1')}
							/>
						</Grid>
						<Grid item xs={4} md={2}>
							<ControlButton
								icon={Filter2Icon}
								text="Mode 2"
								onClick={() => emitSerialData('mode2')}
							/>
						</Grid>
						<Grid item xs={4} md={2}>
							<ControlButton
								icon={Filter3Icon}
								text="Mode 3"
								onClick={() => emitSerialData('mode3')}
							/>
						</Grid>
					</Grid>

					<Grid
						container
						spacing={2}
						mt={2}
						alignItems="center"
						justifyContent="center"
					>
						<Grid item xs={4} md={2}>
							<ControlButton
								icon={KeyboardArrowUpIcon}
								text="Accelerate"
								onClick={() => emitSerialData('accelerate')}
							/>
						</Grid>
						<Grid item xs={4} md={2}>
							<ControlButton
								icon={KeyboardArrowDownIcon}
								text="Decelerate"
								onClick={() => emitSerialData('decelerate')}
							/>
						</Grid>
					</Grid>

					<Grid
						container
						spacing={2}
						mt={2}
						alignItems="center"
						justifyContent="center"
					>
						<Grid item xs={4} md={2}>
							<ControlButton
								icon={UndoIcon}
								text="Left"
								onClick={() => emitSerialData('left')}
							/>
						</Grid>
						<Grid item xs={4} md={2}>
							<Box mb={1}>
								<ControlButton
									icon={KeyboardArrowUpIcon}
									text="Forward"
									onClick={() => emitSerialData('forward')}
								/>
							</Box>
							<Box>
								<ControlButton
									icon={KeyboardArrowDownIcon}
									text="Backward"
									onClick={() => emitSerialData('backward')}
								/>
							</Box>
						</Grid>
						<Grid item xs={4} md={2}>
							<ControlButton
								icon={RedoIcon}
								text="Right"
								onClick={() => emitSerialData('right')}
							/>
						</Grid>
					</Grid>
					<Box mt={2} mx={6}>
						<ControlButton
							icon={StopIcon}
							text="Stop"
							onClick={() => emitSerialData('stop')}
						/>
					</Box>

					{hasBoxlayout && (
						<Box mt={2} justifyItems={'center'}>
							<Box maxWidth={600} mx="auto">
								<Typography pt={2} variant="h4">
									Drawer Debug Controls
								</Typography>
								<Box
									mt={2}
									display="flex"
									flexDirection="column"
									alignItems="stretch"
								>
									<Box mb={1}>
										<Button
											variant="contained"
											color="primary"
											fullWidth
											onClick={() =>
												setDirection(direction === 'left' ? 'right' : 'left')
											}
										>
											{direction === 'left' ? 'Turn Right' : 'Turn Left'}
										</Button>
									</Box>
									<Box mb={1}>
										<Button
											variant="contained"
											color="primary"
											fullWidth
											onClick={() =>
												setDirection(direction === 'up' ? 'down' : 'up')
											}
										>
											{direction === 'up' ? 'Move Down' : 'Move Up'}
										</Button>
									</Box>
									<Box mb={1}>
										<Button
											variant="contained"
											color="primary"
											fullWidth
											onClick={() => setDirection('point')}
										>
											point
										</Button>
									</Box>
									<Box mb={1}>
										<Button
											variant="contained"
											color="primary"
											fullWidth
											onClick={() => setDirection('stop')}
										>
											stop
										</Button>
									</Box>
									<Box mb={1}>
										<Button
											variant="contained"
											color="primary"
											fullWidth
											onClick={() => setDirection('clear')}
										>
											clear
										</Button>
									</Box>
								</Box>
							</Box>
						</Box>
					)}
				</Box>
			</Gamepad>

			<Box mt={2} mx={6}>
				<Canvas color={color} speed={value} direction={direction} />
			</Box>
		</Box>
	);
};

export default App;
