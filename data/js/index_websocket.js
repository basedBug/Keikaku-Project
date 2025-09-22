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
			document.getElementById("rand-1").innerHTML = data.rand2;
		}
		if (data.rand2) {
			document.getElementById("rand-2").innerHTML = data.rand2;
		}
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
