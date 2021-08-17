#include <ArduinoLog.h>
#include <Settings.h>
#include <WiegandMulti.h>
#include <wiegand/CardReaderService.h>

void CardReaderService::setup(void (*ISR_D0)(void), void (*ISR_D1)(void)) {
	wg->begin(WIEGAND_PIN_D0, WIEGAND_PIN_D1, ISR_D0, ISR_D1);
}

boolean CardReaderService::cardAvailable() {
	return wg->available();
}

unsigned long CardReaderService::getCode() {
	return wg->getCode();
}
