let webSocket;
let gateway = "ws://" + window.location.hostname + "/ws";

let reconnectAttempts = 0;
let maxReconnectAttempts = 5;

//window.onun;

function initWebSocket() {
	console.log("Trying to open a websocket connection");
	webSocket = new WebSocket(gateway);

	// Set listeners
	webSocket.onopen = onOpen;
	webSocket.onclose = onClose;
	webSocket.onmessage = onMessage; // Process received data
	webSocket.onerror = onError;
}

function onOpen() {
	console.log("Websocket connected");
	reconnectAttempts = 0; // Reset on successful connection
}

function onClose() {
	console.log("Websocket disconnected");
	// Exponential backoff reconnection attempts (retry every 2, 4, 8, 16, 32... secs)
	if (reconnectAttempts < maxReconnectAttempts) {
		console.log("Attempting reconnection in ", delay / 1000, " seconds");
		const delay = 2000 * Math.pow(2, reconnectAttempts);
		setTimeout(initWebSocket, delay);
		reconnectAttempts++;
	} else {
		console.log("Max reconnection attempts reached");
	}
}

function onError(error) {
	//console.log("Websocket error: " + error);
	console.error("Websocket error: ", error);
}

function onMessage(message) {
	try {
		//console.log("WebSocket message received:", message.data);
		//console.log("Message length:", message.data.length);
		const data = JSON.parse(message.data); // Deserealize json
		console.debug("Received: ", data);

		if (data.rand1) {
			document.getElementById("rand-1").innerHTML = data.rand1;
		}
		if (data.rand2) {
			document.getElementById("rand-2").innerHTML = data.rand2;
		}

		if (data.as5600) {
			if (data.as5600.status) {
				if (data.as5600.status.connected !== undefined) {
					document.getElementById("as5600-connected").innerHTML =
						data.as5600.status.connected;
				}
				if (data.as5600.status.magnetDetected !== undefined) {
					document.getElementById("as5600-magnetDetected").innerHTML =
						data.as5600.status.magnetDetected;
				}
				if (data.as5600.status.magnetTooStrong !== undefined) {
					document.getElementById(
						"as5600-magnetTooStrong"
					).innerHTML = data.as5600.status.magnetTooStrong;
				}
				if (data.as5600.status.magnetTooWeak !== undefined) {
					document.getElementById("as5600-magnetTooWeak").innerHTML =
						data.as5600.status.magnetTooWeak;
				}
				if (data.as5600.status.cordicMagnitude !== undefined) {
					document.getElementById(
						"as5600-cordicMagnitude"
					).innerHTML = data.as5600.status.cordicMagnitude;
				}
				if (data.as5600.status.agcGain !== undefined) {
					document.getElementById("as5600-agcGain").innerHTML =
						data.as5600.status.agcGain;
				}
			}
			if (data.as5600.telemetry) {
				if (data.as5600.telemetry.angle !== undefined) {
					document.getElementById("as5600-angle").innerHTML =
						data.as5600.telemetry.angle;
				}
				if (data.as5600.telemetry.rawAngle !== undefined) {
					document.getElementById("as5600-rawAngle").innerHTML =
						data.as5600.telemetry.rawAngle;
				}
				if (data.as5600.telemetry.cumulativeAngle !== undefined) {
					document.getElementById(
						"as5600-cumulativeAngle"
					).innerHTML = data.as5600.telemetry.cumulativeAngle;
					updateDial(
						data.as5600.telemetry.cumulativeAngle,
						data.as5600.telemetry.angle
					);
				}
				if (data.as5600.telemetry.revolutions !== undefined) {
					document.getElementById("as5600-revolutions").innerHTML =
						data.as5600.telemetry.revolutions;
				}
				if (data.as5600.telemetry.speedDeg !== undefined) {
					document.getElementById("as5600-speedDeg").innerHTML =
						data.as5600.telemetry.speedDeg;
				}
				if (data.as5600.telemetry.speedRPM !== undefined) {
					document.getElementById("as5600-speedRPM").innerHTML =
						data.as5600.telemetry.speedRPM;
				}
				if (data.as5600.telemetry.speedRad !== undefined) {
					document.getElementById("as5600-speedRad").innerHTML =
						data.as5600.telemetry.speedRad;
				}
			}
			if (data.as5600.configuration) {
				if (data.as5600.configuration.powerMode !== undefined) {
					document.getElementById("as5600-powerMode").innerHTML =
						data.as5600.configuration.powerMode;
				}
				if (data.as5600.configuration.hysteresis !== undefined) {
					document.getElementById("as5600-hysteresis").innerHTML =
						data.as5600.configuration.hysteresis;
				}
				if (data.as5600.configuration.slowFilter !== undefined) {
					document.getElementById("as5600-slowFilter").innerHTML =
						data.as5600.configuration.slowFilter;
				}
				if (data.as5600.configuration.fastFilter !== undefined) {
					document.getElementById("as5600-fastFilter").innerHTML =
						data.as5600.configuration.fastFilter;
				}
			}
		}

		document.getElementById("json-display").innerHTML = JSON.stringify(
			data,
			null,
			2
		);
	} catch (error) {
		console.error(
			"Failed to parse JSON message: ",
			error,
			"Raw data: ",
			message.data
		);
	}
}

// Init websocket when the page loads
window.onload = () => initWebSocket();
