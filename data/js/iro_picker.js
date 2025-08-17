function getPickerWidth() {
	const root = document.querySelector(":root");
	const rootStyle = getComputedStyle(root);
	return Number(
		rootStyle.getPropertyValue("--picker-width").replace("px", "")
	);
}
const pickerWidth = getPickerWidth();

const colorPicker = new iro.ColorPicker("#color", {
	// Set the size of the color picker
	width: pickerWidth,
	// Initial color
	color: "rgb(255, 0, 0)",
	// Width of the border around the controls
	borderWidth: 2,
	// Color of the border
	borderColor: "rgb(255, 255, 255)",
	// If set to false, the color wheel will not fade to black when the lightness decreases
	wheelLigthness: false,
	// Slider control size. By default this will be calculated automatically.
	sliderSize: pickerWidth / 11,
});
// Initialize to off color, so that the neopixel is off by default
colorPicker.color.value = 0;

function sendCommandToWs(command) {
	const jsonCommand = JSON.stringify(command);
	webSocket.send(jsonCommand);
	console.log("Sent JSON", jsonCommand);
}

function sendNeopixelCommand(rgb) {
	const command = {
		neopixel: {
			color: {
				r: rgb.r,
				g: rgb.g,
				b: rgb.b,
			},
		},
	};
	sendCommandToWs(command);
}

let debounce = null;
colorPicker.on("input:change", function (value) {
	if (!debounce) {
		console.debug("Internal color change: ", value.rgb);
		changeBackgroundColor(value.rgb);
		sendNeopixelCommand(value.rgb);
	}
	debounce = setTimeout(() => {
		debounce = undefined;
	}, 300); // 300ms debounce
});

function changeBackgroundColor(rgb) {
	document.body.style.backgroundColor =
		"rgb(" + rgb.r + ", " + rgb.g + ", " + rgb.b + ")";
}
