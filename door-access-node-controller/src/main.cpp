#include <Arduino.h>
#include <ArduinoLog.h>
#include <SPI.h>
#include <Settings.h>
#include <WiegandMulti.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

#include <CanMessageService.cpp>

WIEGANDMULTI wg;
MCP_CAN *CAN0 = new MCP_CAN(CAN0_CS_PIN);
RelayControlService relayService;
CanMessageService canBusService(CAN0, relayService);

// TODO: CardReaderService? Interrupt not compatible with C++ classes. Workaround?
void Reader1D0Interrupt(void) {
	wg.ReadD0();
}

void Reader1D1Interrupt(void) {
	wg.ReadD1();
}

void setup() {
	Serial.begin(9600);
	Log.begin(LOG_LEVEL_VERBOSE, &Serial);

	canBusService.setup();
	relayService.setup();

	wg.begin(WIEGAND_PIN_D0, WIEGAND_PIN_D1, Reader1D0Interrupt, Reader1D1Interrupt);
}

void loop() {
	canBusService.heartbeatProducer();
	relayService.checkToTurnRelayOff();

	if (canBusService.canMessageAvailable()) {
		canBusService.processCanMessage();
	}

	if (wg.available()) {
		canBusService.sendCardCodeToCanBus(wg.getCode());
	}
}
