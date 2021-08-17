#include <Arduino.h>
#include <Settings.h>
#include <millisDelay.h>
#include <relay/RelayControlService.h>

void RelayControlService::setup() {
	pinMode(RELAY_PIN, OUTPUT);
}

void RelayControlService::checkToTurnRelayOff() {
	if (relayDelay.justFinished()) {
		digitalWrite(RELAY_PIN, LOW);
	}
}

void RelayControlService::triggerRelay(unsigned long lockDelayMilliseconds) {
	digitalWrite(RELAY_PIN, HIGH);

	relayDelay.start(lockDelayMilliseconds);
}
