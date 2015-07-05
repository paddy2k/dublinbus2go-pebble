var appMessageQueue = {
	queue: [],
	numTries: 0,
	maxTries: 5,
	working: false,
	clear: function() {
		this.queue = [];
		this.working = false;
	},
	isEmpty: function() {
		return this.queue.length === 0;
	},
	nextMessage: function() {
		return this.isEmpty() ? {} : this.queue[0];
	},
	send: function(message) {
		if (message) this.queue.push(message);
		if (this.working) return;
		if (this.queue.length > 0) {
			this.working = true;
			var ack = function() {
				appMessageQueue.numTries = 0;
				appMessageQueue.queue.shift();
				appMessageQueue.working = false;
				appMessageQueue.send();
        console.log('AppMessage SENT: ' + JSON.stringify(appMessageQueue.nextMessage()));
			};
			var nack = function() {
				appMessageQueue.numTries++;
				appMessageQueue.working = false;
				appMessageQueue.send();
        console.log('AppMessage NACK: ' + JSON.stringify(appMessageQueue.nextMessage()));
			};
			if (appMessageQueue.numTries >= appMessageQueue.maxTries) {
				console.log('Failed sending AppMessage: ' + JSON.stringify(appMessageQueue.nextMessage()));
				ack();
			}
			console.log('Sending AppMessage: ' + JSON.stringify(appMessageQueue.nextMessage()));
			Pebble.sendAppMessage(appMessageQueue.nextMessage(), ack, nack);
		}
	}
};