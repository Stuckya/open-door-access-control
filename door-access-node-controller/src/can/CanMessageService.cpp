#include <ArduinoLog.h>
#include <SPI.h>
#include <Settings.h>
#include <can/CanMessageService.h>
#include <can/CanMessageType.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <millisDelay.h>

void CanMessageService::setup() {
	if (canBus->begin(MCP_STDEXT, CAN_250KBPS, MCP_16MHZ) != CAN_OK) {
		Log.error("MCP2515 setup failure!");
		return;
	}

	configureBus();
}

void CanMessageService::configureBus() {
	pinMode(CAN0_INTERRUPT_PIN, INPUT);

	configureMaskAndFilters();

	canBus->setMode(MCP_NORMAL);

	heartbeatDelay.start(1000);

	Log.notice("MCP2515 setup success!");
}

void CanMessageService::configureMaskAndFilters() {
	// INFO: Create mask and filter to allow only receipt of 0x7xx CAN IDs
	canBus->init_Mask(0, 0, 0x07000000);
	canBus->init_Mask(1, 0, 0x07000000);

	for (uint8_t i = 0; i < 6; ++i) {
		canBus->init_Filt(i, 0, 0x07000000);
	}
}

boolean CanMessageService::canMessageAvailable() {
	return CAN_MSGAVAIL == canBus->checkReceive();
}

void CanMessageService::processIncomingCanMessage() {
	// INFO: Message structure { arduinoId, commandId, data }
	canBus->readMsgBuf(&rxId, &len, rxBuf);

	if (rxBuf[0] != ARDUINO_ID) {
		return;
	}

	switch (rxBuf[1]) {
		// INFO: { updatedStatus }
		case (int)(CanMessageType::UPDATE_STATUS): {
			Log.trace("Received status update command.");

			// TODO: nullptr/ empty check?
			unsigned char updatedStatus = rxBuf[2];

			status = static_cast<HealthCheckStatus>(updatedStatus);
			break;
		}
		// INFO: { lockDelaySeconds }
		case (int)(CanMessageType::DOOR_UNLOCK): {
			Log.trace("Received unlock command.");

			// TODO: Should this accept 0 for unlimited w/ timeout? A way to hold unlock.
			// TODO: Trigger hold: door detected open, scan card to hold?
			// TODO: nullptr/ empty check?
			unsigned char lockDelaySeconds = rxBuf[2];
			unsigned long lockDelayMilliseconds = lockDelaySeconds * 1000;

			relayService->triggerRelay(lockDelayMilliseconds);
			break;
		}
	}
}

void CanMessageService::heartbeatProducer() {
	if (heartbeatDelay.justFinished()) {
		heartbeatDelay.repeat();

		unsigned char messageBuffer[2] = {(int)(CanMessageType::HEARTBEAT), (unsigned char)(status)};

		canBus->sendMsgBuf(ARDUINO_ID, 0, 2, messageBuffer);

		Log.trace("Sending heartbeat.");
	}
}

void CanMessageService::sendCardCodeToCanBus(unsigned long cardCode) {
	unsigned char messageType[1] = {(int)(CanMessageType::CARD_READ)};

	unsigned char *byteArrayCardCode = convertLongToByteArray(cardCode);

	unsigned char messageBuffer[5];

	memcpy(messageBuffer, messageType, 1);
	memcpy(messageBuffer + 1, byteArrayCardCode, 4);

	sendMessageToBus(5, messageBuffer);
}

void CanMessageService::sendMessageToBus(INT8U len, INT8U *buf) {
	if (status == HealthCheckStatus::STOPPED) {
		Log.error("Cannot send message in Stopped state.");

		return;
	}

	byte sndStat = canBus->sendMsgBuf(ARDUINO_ID, 0, len, buf);

	if (sndStat == CAN_OK) {
		Log.trace("Success sending message CAN network.");
	} else {
		Log.error("Failure sending message CAN network.");
	}
}

unsigned char *CanMessageService::convertLongToByteArray(unsigned long num) {
	static unsigned char byteArray[4];

	byteArray[0] = (int)((num >> 24) & 0xFF);
	byteArray[1] = (int)((num >> 16) & 0xFF);
	byteArray[2] = (int)((num >> 8) & 0XFF);
	byteArray[3] = (int)((num & 0XFF));

	return byteArray;
}