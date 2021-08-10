#include <Arduino.h>
#include <Settings.h>
#include <millisDelay.h>
class RelayControlService {
   private:
	millisDelay relayDelay;

   public:
	void setup() {
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