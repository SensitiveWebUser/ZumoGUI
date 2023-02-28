import React, { useState, useEffect } from 'react';

const Canvas = (props) => {
	const { color, speed, direction } = props;

	const value = speed / 100;

	const size = 500;

	// Create a ref for the stop flag
	const stopRef = React.useRef(false);

	// Get a reference to the canvas element
	const canvasRef = React.useRef(null);

	// Create a state for storing the previous position
	const [prevPos, setPrevPos] = useState({
		x: size / 2,
		y: size / 2,
	});

	// Create a state for storing the previous direction
	const [prevDir, setPrevDir] = useState(direction);

	// Update the canvas context on each render
	useEffect(() => {
		// Get the canvas context
		const ctx = canvasRef.current.getContext('2d');

		// Clear the previous drawing
		ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);

		// Set an initial position for drawing lines
		let x = prevPos.x;
		let y = prevPos.y;

		// Define a function that draws a line on the canvas context
		const drawLine = () => {
			// Check if stop flag is true and return if so
			if (stopRef.current) return;

			// Draw a line with the current color and value only if direction has not changed
			if (direction === prevDir) {
				ctx.strokeStyle = color;
				ctx.lineWidth = 6;
				ctx.beginPath();
				ctx.moveTo(x - value / 2, y - value / 2);
				ctx.lineTo(x + value / 2, y + value / 2);
				ctx.stroke();
			}

			// Increment x or y depending on direction state
			switch (direction) {
				case 'up':
					y -= value;
					break;
				case 'down':
					y += value;
					break;
				case 'left':
					x -= value;
					break;
				case 'right':
					x += value;
					break;
				case 'point':
					ctx.arc(x, y, 3, 0, 2 * Math.PI);
					ctx.stroke();
					break;
				case 'clear':
					ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
					break;
				default:
					break;
			}

			// If direction is 'point', update the previous direction state to the last non-'point' direction
			if (direction === 'point') {
				setPrevDir(prevDir !== 'point' ? prevDir : 'right');
			}

			// Wrap around if x or y goes out of bounds
			if (x < value || x > ctx.canvas.width - value) {
				x = Math.abs(ctx.canvas.width - x);
			}
			if (y < value || y > ctx.canvas.height - value) {
				y = Math.abs(ctx.canvas.height - y);
			}

			// Update the previous position and direction states
			if (direction !== 'clear') {
				setPrevPos({ x, y });
			} else {
				setPrevPos({
					x: size / 2,
					y: size / 2,
				});
			}
			setPrevDir(direction);

			// Request an animation frame
			requestAnimationFrame(drawLine);
		};

		// Call the drawLine function once
		drawLine();
	}, [color, value, direction, prevPos.x, prevPos.y, prevDir]); // Run only when color,value or direction changes

	// Return a canvas element with a ref

	return <canvas ref={canvasRef} width={size} height={size} />;
};

export default Canvas;
