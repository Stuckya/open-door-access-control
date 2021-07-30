#include <WiegandMulti.h>
#include <mcp_can_dfs.h>
#include <mcp_can.h>
#include <SPI.h>

WIEGANDMULTI wg;
MCP_CAN CAN0(10);

// TODO: Move this to config header file?
#define CAN0_INTERRUPT_PIN 2
#define RELAY_PIN 5

void Reader1D0Interrupt(void)
{
    wg.ReadD0();
}

void Reader1D1Interrupt(void)
{
    wg.ReadD1();
}

// TODO: Move this to config header file?
unsigned char arduinoId = 100;
unsigned char len = 0;
unsigned char rxBuf[8];
long unsigned int rxId;

void setup()
{
    Serial.begin(9600);

    if (CAN0.begin(MCP_STDEXT, CAN_250KBPS, MCP_16MHZ) == CAN_OK)
        Serial.println("MCP2515 Success!");
    else
        Serial.println("MCP2515 Failure!");

    pinMode(RELAY_PIN, OUTPUT);
    pinMode(CAN0_INTERRUPT_PIN, INPUT);

    // Create mask and filter to allow only receipt of 0x7xx CAN IDs
    CAN0.init_Mask(0, 0, 0x07000000);
    CAN0.init_Mask(1, 0, 0x07000000);

    for (uint8_t i = 0; i < 6; ++i)
    {
        CAN0.init_Filt(i, 0, 0x07000000);
    }

    CAN0.setMode(MCP_NORMAL);

    wg.begin(8, 9, Reader1D0Interrupt, Reader1D1Interrupt);
}

void loop()
{

    if (canMessageAvailable())
    {
        processCanMessage();
    }

    if (wg.available())
    {
        sendCardCodeToCanBus(wg.getCode());
    }
}

boolean canMessageAvailable()
{
    return CAN_MSGAVAIL == CAN0.checkReceive();
}

void processCanMessage()
{
    // TODO: parameterType?
    // INFO: Message structure { arduinoId, commandId, parameter }
    CAN0.readMsgBuf(&rxId, &len, rxBuf);

    if (rxBuf[0] != arduinoId)
    {
        return;
    }

    // TODO: Switch over available commands? Unlock, LED, hold, program new key?
    if (rxBuf[1] == 1)
    {
        Serial.println("Received unlock command.");

        digitalWrite(RELAY_PIN, HIGH);

        // TODO: Should this accept 0 for unlimited? A way to hold unlock? What action triggers this?
        unsigned char lockDelaySeconds = rxBuf[2];
        unsigned long lockDelayMilliseconds = lockDelaySeconds * 1000;

        delay(lockDelayMilliseconds);
        digitalWrite(RELAY_PIN, LOW);
    }
}

void sendCardCodeToCanBus(unsigned long cardCode)
{
    unsigned char *byteArraycardCode = convertLongToByteArray(cardCode);

    byte sndStat = CAN0.sendMsgBuf(arduinoId, 0, 8, byteArraycardCode);

    if (sndStat == CAN_OK)
    {
        Serial.println("Success sending door code over CAN network");
    }
    else
    {
        Serial.println("Failure sending door code over CAN network");
    }
}

unsigned char *convertLongToByteArray(unsigned long num)
{
    static unsigned char byteArray[4];

    byteArray[0] = (int)((num >> 24) & 0xFF);
    byteArray[1] = (int)((num >> 16) & 0xFF);
    byteArray[2] = (int)((num >> 8) & 0XFF);
    byteArray[3] = (int)((num & 0XFF));

    return byteArray;
}
