#include <Arduino.h>
#include <SPI.h>
#include <WiegandMulti.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

#include <DoorAccessCanMessageService.cpp>

WIEGANDMULTI wg;
MCP_CAN *CAN0 = new MCP_CAN(10);

DoorAccessCanMessageService canBusService(CAN0);

// TODO: Move this to config header file?
#define CAN0_INTERRUPT_PIN 2
#define RELAY_PIN 5

void Reader1D0Interrupt(void) {
	wg.ReadD0();
}

void Reader1D1Interrupt(void) {
	wg.ReadD1();
}

void setup() {
	Serial.begin(9600);

	if (CAN0->begin(MCP_STDEXT, CAN_250KBPS, MCP_16MHZ) == CAN_OK)
		Serial.println("MCP2515 Success!");
	else
		Serial.println("MCP2515 Failure!");

	pinMode(RELAY_PIN, OUTPUT);
	pinMode(CAN0_INTERRUPT_PIN, INPUT);

	// Create mask and filter to allow only receipt of 0x7xx CAN IDs
	CAN0->init_Mask(0, 0, 0x07000000);
	CAN0->init_Mask(1, 0, 0x07000000);

	for (uint8_t i = 0; i < 6; ++i) {
		CAN0->init_Filt(i, 0, 0x07000000);
	}

	CAN0->setMode(MCP_NORMAL);

	wg.begin(8, 9, Reader1D0Interrupt, Reader1D1Interrupt);
}

void loop() {
	if (canBusService.canMessageAvailable()) {
		canBusService.processCanMessage();
	}

	if (wg.available()) {
		canBusService.sendCardCodeToCanBus(wg.getCode());
	}
}
