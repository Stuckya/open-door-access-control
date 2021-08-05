#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

// TODO: Config/ Header File?
#define RELAY_PIN 5

class DoorAccessCanMessageService {
   private:
	MCP_CAN *canBus = nullptr;

	unsigned char len = 0;
	unsigned char rxBuf[8];
	long unsigned int rxId;

	// TODO: Replace with SerialService
	unsigned char arduinoId = 100;

   public:
	DoorAccessCanMessageService(MCP_CAN *bus) : canBus(bus) {
		if (canBus == nullptr) {
			Serial.println("Service must not be null.");
		}
	}

   public:
	boolean canMessageAvailable() {
		return CAN_MSGAVAIL == canBus->checkReceive();
	}

   public:
	void processCanMessage() {
		// TODO: parameterType?
		// INFO: Message structure { arduinoId, commandId, parameter }
		canBus->readMsgBuf(&rxId, &len, rxBuf);

		if (rxBuf[0] != arduinoId) {
			return;
		}

		// TODO: Switch over available commands? Unlock, LED, hold, program new key?
		if (rxBuf[1] == 1) {
			Serial.println("Received unlock command.");

			digitalWrite(RELAY_PIN, HIGH);

			// TODO: Should this accept 0 for unlimited? A way to hold unlock? What action triggers this?
			unsigned char lockDelaySeconds = rxBuf[2];
			unsigned long lockDelayMilliseconds = lockDelaySeconds * 1000;

			delay(lockDelayMilliseconds);
			digitalWrite(RELAY_PIN, LOW);
		}
	}

   public:
	void sendCardCodeToCanBus(unsigned long cardCode) {
		unsigned char *byteArraycardCode = convertLongToByteArray(cardCode);

		byte sndStat = canBus->sendMsgBuf(arduinoId, 0, 8, byteArraycardCode);

		if (sndStat == CAN_OK) {
			Serial.println("Success sending door code over CAN network");
		} else {
			Serial.println("Failure sending door code over CAN network");
		}
	}

   private:
	unsigned char *convertLongToByteArray(unsigned long num) {
		static unsigned char byteArray[4];

		byteArray[0] = (int)((num >> 24) & 0xFF);
		byteArray[1] = (int)((num >> 16) & 0xFF);
		byteArray[2] = (int)((num >> 8) & 0XFF);
		byteArray[3] = (int)((num & 0XFF));

		return byteArray;
	}
};