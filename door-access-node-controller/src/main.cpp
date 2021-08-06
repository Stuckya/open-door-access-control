#include <Arduino.h>
#include <SPI.h>
#include <WiegandMulti.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

#include <DoorAccessCanMessageService.cpp>

WIEGANDMULTI wg;

// TODO: define CS pin in header/config.
MCP_CAN *CAN0 = new MCP_CAN(10);

DoorAccessCanMessageService canBusService(CAN0);

// TODO: DoorAccessWiegandService?
void Reader1D0Interrupt(void) {
	wg.ReadD0();
}

void Reader1D1Interrupt(void) {
	wg.ReadD1();
}

void setup() {
	Serial.begin(9600);

	canBusService.configureBus();

	// TODO: Move pins to header/ config.
	wg.begin(8, 9, Reader1D0Interrupt, Reader1D1Interrupt);
}

void loop() {
	canBusService.heartbeatProducer();

	// TODO: Relay service?
	canBusService.checkToTurnRelayOff();

	if (canBusService.canMessageAvailable()) {
		canBusService.processCanMessage();
	}

	if (wg.available()) {
		canBusService.sendCardCodeToCanBus(wg.getCode());
	}
}
