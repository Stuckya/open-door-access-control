#include <SPI.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <millisDelay.h>

// TODO: Config/ Header File?
#define RELAY_PIN 5
#define CAN0_INTERRUPT_PIN 2
#define ARDUINO_ID 100

class DoorAccessCanMessageService {
   private:
	MCP_CAN *canBus = nullptr;

	unsigned char len = 0;
	unsigned char rxBuf[8];
	long unsigned int rxId;
	millisDelay heartbeatDelay;
	millisDelay relayDelay;

   public:
	DoorAccessCanMessageService(MCP_CAN *bus) : canBus(bus) {
		if (canBus == nullptr) {
			Serial.println("Injected service must not be null.");
		}
	}

   public:
	void configureBus() {
		if (canBus->begin(MCP_STDEXT, CAN_250KBPS, MCP_16MHZ) != CAN_OK) {
			Serial.println("MCP2515 setup failure!");
			return;
		}

		busSetup();
		pinMode(RELAY_PIN, OUTPUT);
	}

   private:
	void busSetup() {
		pinMode(CAN0_INTERRUPT_PIN, INPUT);

		configureMaskAndFilters();

		canBus->setMode(MCP_NORMAL);

		heartbeatDelay.start(1000);

		Serial.println("MCP2515 setup success!");
	}

   private:
	void configureMaskAndFilters() {
		// Create mask and filter to allow only receipt of 0x7xx CAN IDs
		canBus->init_Mask(0, 0, 0x07000000);
		canBus->init_Mask(1, 0, 0x07000000);

		for (uint8_t i = 0; i < 6; ++i) {
			canBus->init_Filt(i, 0, 0x07000000);
		}
	}

   public:
	void heartbeatProducer() {
		if (heartbeatDelay.justFinished()) {
			heartbeatDelay.repeat();

			// TODO: Send heartbeat message here.
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

		// This message isn't for me!
		if (rxBuf[0] != ARDUINO_ID) {
			return;
		}

		// TODO: Switch over available commands? Unlock, LED, hold, program new key?
		if (rxBuf[1] == 1) {
			Serial.println("Received unlock command.");

			digitalWrite(RELAY_PIN, HIGH);

			// TODO: Should this accept 0 for unlimited w/ timeout? A way to hold unlock.
			// TODO: Trigger hold: scan card, door detected open, scan again?
			unsigned char lockDelaySeconds = rxBuf[2];
			unsigned long lockDelayMilliseconds = lockDelaySeconds * 1000;

			relayDelay.start(lockDelayMilliseconds);
		}
	}

   public:
	void checkToTurnRelayOff() {
		if (relayDelay.justFinished()) {
			digitalWrite(RELAY_PIN, LOW);
		}
	}

   public:
	void sendCardCodeToCanBus(unsigned long cardCode) {
		unsigned char *byteArraycardCode = convertLongToByteArray(cardCode);

		byte sndStat = canBus->sendMsgBuf(ARDUINO_ID, 0, 8, byteArraycardCode);

		if (sndStat == CAN_OK) {
			Serial.println("Success sending card code over CAN network.");
		} else {
			Serial.println("Failure sending card code over CAN network.");
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