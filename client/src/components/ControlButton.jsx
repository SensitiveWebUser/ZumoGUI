import Button from '@mui/material/Button';

const ControlButton = ({ icon: IconComponent, text, onClick }) => (
	<Button variant="contained" color="primary" fullWidth onClick={onClick}>
		<IconComponent fontSize="large" />
		{` ${text}`}
	</Button>
);

export default ControlButton;
