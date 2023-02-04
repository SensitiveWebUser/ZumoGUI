import { useContext } from 'react';
import { SocketContext } from './context/socket';

import Button from '@mui/material/Button';
import Grid from '@mui/material/Grid';

import KeyboardArrowUpIcon from '@mui/icons-material/KeyboardArrowUp';
import KeyboardArrowDownIcon from '@mui/icons-material/KeyboardArrowDown';
import UndoIcon from '@mui/icons-material/Undo';
import RedoIcon from '@mui/icons-material/Redo';
import StopIcon from '@mui/icons-material/Stop';

export const Controller = (props) => {
	const socket = useContext(SocketContext);

	return (
		<>
			<Grid container spacing={1}>
				<Grid item xs={6} md={2}>
					<Button onClick={() => socket.emit('serialdata', 'a')}>
						<UndoIcon fontSize="large" />
					</Button>
				</Grid>
				<Grid item xs={4} md={2}>
					<Grid container spacing={2} justifyContent="center">
						<Button onClick={() => socket.emit('serialdata', 'w')}>
							<KeyboardArrowUpIcon fontSize="large" />
						</Button>
						<Button onClick={() => socket.emit('serialdata', 's')}>
							<KeyboardArrowDownIcon fontSize="large" />
						</Button>
					</Grid>
				</Grid>
				<Grid item xs={4} md={2}>
					<Button onClick={() => socket.emit('serialdata', 'd')}>
						<RedoIcon fontSize="large" />
					</Button>
				</Grid>
				<Grid item xs md={6}>
					<Button onClick={() => socket.emit('serialdata', 'z')}>
						<StopIcon fontSize="large" />
					</Button>
				</Grid>
			</Grid>
		</>
	);
};
