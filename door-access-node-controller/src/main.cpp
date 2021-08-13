#include <Arduino.h>
#include <ArduinoLog.h>
#include <CanMessageService.h>
#include <CardReaderService.h>
#include <SPI.h>
#include <Settings.h>
#include <WiegandMulti.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

WIEGANDMULTI *wg = new WIEGANDMULTI;
MCP_CAN *CAN0 = new MCP_CAN(CAN0_CS_PIN);

RelayControlService *relayService = new RelayControlService;
CanMessageService *busService = new CanMessageService(CAN0, relayService);
CardReaderService *cardReaderService = new CardReaderService(wg);

void Reader1D0Interrupt(void) {
	wg->ReadD0();
}

void Reader1D1Interrupt(void) {
	wg->ReadD1();
}

void setup() {
	Serial.begin(9600);
	Log.begin(LOG_LEVEL_VERBOSE, &Serial);

	busService->setup();
	cardReaderService->setup(Reader1D0Interrupt, Reader1D1Interrupt);
	relayService->setup();
}

void loop() {
	busService->heartbeatProducer();
	relayService->checkToTurnRelayOff();

	if (busService->canMessageAvailable()) {
		busService->processCanMessage();
	}

	if (cardReaderService->cardAvailable()) {
		busService->sendCardCodeToCanBus(cardReaderService->getCode());
	}
}
