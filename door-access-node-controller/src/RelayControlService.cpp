#include <Arduino.h>
#include <RelayControlService.h>
#include <Settings.h>
#include <millisDelay.h>

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
