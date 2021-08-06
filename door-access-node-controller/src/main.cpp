#include <Arduino.h>
#include <SPI.h>
#include <WiegandMulti.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

#include <CanMessageService.cpp>

WIEGANDMULTI wg;

// TODO: define CS pin in header/config.
MCP_CAN *CAN0 = new MCP_CAN(10);
RelayControlService *relayService = new RelayControlService();

CanMessageService canBusService(CAN0, relayService);

// TODO: CardReaderService?
void Reader1D0Interrupt(void) {
	wg.ReadD0();
}

void Reader1D1Interrupt(void) {
	wg.ReadD1();
}

void setup() {
	Serial.begin(9600);

	canBusService.configureBus();
	relayService->configureRelay();

	// TODO: Move pins to header/ config.
	wg.begin(8, 9, Reader1D0Interrupt, Reader1D1Interrupt);
}

void loop() {
	canBusService.heartbeatProducer();
	relayService->checkToTurnRelayOff();

	if (canBusService.canMessageAvailable()) {
		canBusService.processCanMessage();
	}

	if (wg.available()) {
		canBusService.sendCardCodeToCanBus(wg.getCode());
	}
}
