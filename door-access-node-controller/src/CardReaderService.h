#include <ArduinoLog.h>
#include <Settings.h>
#include <WiegandMulti.h>

class CardReaderService {
   private:
	WIEGANDMULTI *wg = nullptr;

   public:
	CardReaderService(WIEGANDMULTI *wg) : wg(wg) {
		if (wg == nullptr) {
			Log.error("Injected services must not be null!");
		}
	}

   public:
	void setup(void (*ISR_D0)(void), void (*ISR_D1)(void));

   public:
	boolean cardAvailable();

   public:
	unsigned long getCode();
};
