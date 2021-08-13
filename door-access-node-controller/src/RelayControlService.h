#include <Arduino.h>
#include <Settings.h>
#include <millisDelay.h>

class RelayControlService {
   private:
	millisDelay relayDelay;

   public:
	RelayControlService() {}

   public:
	void setup();

   public:
	void checkToTurnRelayOff();

   public:
	void triggerRelay(unsigned long lockDelayMilliseconds);
};
