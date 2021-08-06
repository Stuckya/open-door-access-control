#include <Arduino.h>
#include <millisDelay.h>

#define RELAY_PIN 5

class RelayControlService {
   private:
	millisDelay relayDelay;

   public:
	void configureRelay() {
		pinMode(RELAY_PIN, OUTPUT);
	}

   public:
	void checkToTurnRelayOff() {
		if (relayDelay.justFinished()) {
			digitalWrite(RELAY_PIN, LOW);
		}
	}

   public:
	void triggerRelay(unsigned long lockDelayMilliseconds) {
		digitalWrite(RELAY_PIN, HIGH);

		relayDelay.start(lockDelayMilliseconds);
	}
};