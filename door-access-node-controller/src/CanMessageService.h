#include <ArduinoLog.h>
#include <RelayControlService.h>
#include <SPI.h>
#include <Settings.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <millisDelay.h>

class CanMessageService {
   private:
	MCP_CAN *canBus = nullptr;
	RelayControlService *relayService = nullptr;

	unsigned char len = 0;
	unsigned char rxBuf[8];
	long unsigned int rxId;

	millisDelay heartbeatDelay;

   public:
	CanMessageService(MCP_CAN *bus, RelayControlService *relayService) : canBus(bus), relayService(relayService) {
		if (canBus == nullptr || relayService == nullptr) {
			Log.error("Injected services must not be null!");
		}
	}

   public:
	void setup();

   private:
	void configureBus();

   private:
	void configureMaskAndFilters();

   public:
	void heartbeatProducer();

   public:
	boolean canMessageAvailable();

   public:
	void processCanMessage();

   public:
	void sendCardCodeToCanBus(unsigned long cardCode);

   private:
	unsigned char *convertLongToByteArray(unsigned long num);
};