#ifndef ADV_DATA_H
#define ADV_DATA_H

#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include "AdvProtocolF9.h"
#include "AdvProtocolF0.h"


#define DEVICE_NAME "Ninja lamp remote (ESP32)"

#define ADV_DELAY 150 // in ms
#define ADV_PROT ALL // F0, F9, ALL
#define REPEAT_CMDS_COUNT 4
#define CMD_BYTES 32
#define PROT_COUNT 2
#define MAX_REPEAT_CMDS 20000
#define MAX_ADV_DELAY 5000

// Used when advertising by time
#define ADV_TIME 5000 // in ms


class AdvData{
    public:
        void setup();
        void start(uint8_t cmd);
        void advertise();
        void advRaw(char* rawData, uint8_t dataLen);
        void setRepeatCmdsCount(uint16_t repeatCmdsCount);
        void printSettings();

        enum AdvProt {F9, F0, ALL};

        static uint8_t colour;
        static uint8_t brightness;
        static uint8_t advProt;
        static uint16_t advDelay;
        static bool advertiseByCount;
        static bool invertCh;
    private:
        void prepareAdvertising();
        void getAdvData(uint8_t cmd);
        void setAdvData(BLEAdvertisementData &adv);
        void setRawAdvData(BLEAdvertisementData &adv);
        void brAndCCTtoPWM(uint8_t &valCh1, uint8_t &valCh2);
        uint8_t getNextProt();
        uint8_t char2int(char input);
        void hex2bin(const char* src, char* target);

        static char advData[CMD_BYTES];

        static bool advStopped;

        static uint8_t currCmd;

        static uint16_t repeatCmdsCount;
        static uint16_t sentCmdsCount;
        static uint8_t advLen;
        static uint8_t currProt;
        static uint32_t startAdvTime;
        static uint32_t startAdvTimeForProt;

        BLEAdvertising *pAdvertising;

        static AdvProtocolF9 p1;
        static AdvProtocolF0 p2;
};

#endif