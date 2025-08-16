let webSocket;
let gateway = "ws://" + window.location.hostname + "/ws";

let reconnectAttempts = 0;
let maxReconnectAttempts = 5;

//window.onun;

function initWebSocket() {
	console.log("Trying to open a websocket connection");
	webSocket = new WebSocket(gateway);

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
	console.log("Attempting reconnection");
	// Exponential backoff reconnection attempts (retry every 2, 4, 8, 16, 32... secs)
	if (reconnectAttempts < maxReconnectAttempts) {
		setTimeout(initWebSocket, 2000 * Math.pow(2, reconnectAttempts));
		reconnectAttempts++;
	} else {
		console.log("Max reconnection attempts reached");
	}
}

function onError(error) {
	//console.log("Websocket error: " + error);
	console.error("Websocket error: ", error);
}

function onMessage(event) {
	try {
		var jsonObj = JSON.parse(event.data); // Deserealize json
		//var jsonKeys = Object.keys(jsonObj);
		console.debug(jsonObj);

		Object.entries(jsonObj).forEach(([key, value]) => {
			// Check if element mapping exists
			const elementId = KEY_TO_ELEMENT_MAP[key];
			if (elementId) {
				// Check if element exists for this elementID
				const element = document.getElementById(elementId);
				if (element) {
					element.innerHTML = value; //
				} else {
					console.warn("No DOM element found for ID: ", elementId);
				}
			} else {
				console.warn("No DOM mapping defined for JSON key: ", key);
			}
		});
	} catch (error) {
		console.error(
			"Failed to parse JSON message: ",
			error,
			"Raw data: ",
			event.data
		);
	}
}

// Init websocket when the page loads
window.onload = () => initWebSocket();
