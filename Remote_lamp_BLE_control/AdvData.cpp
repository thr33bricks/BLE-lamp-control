#include "AdvData.h"

char AdvData::advData[CMD_BYTES] = {0,};
bool AdvData::advStopped = true;
uint8_t AdvData::advLen = 26;
uint8_t AdvData::colour = 50;
uint8_t AdvData::brightness = 100;
uint8_t AdvData::currProt = F0;
uint32_t AdvData::startAdvTime;
uint32_t AdvData::startAdvTimeForProt;
AdvProtocolF9 AdvData::p1;
AdvProtocolF0 AdvData::p2;
uint16_t AdvData::sentCmdsCount = REPEAT_CMDS_COUNT;
uint8_t AdvData::currCmd = 0xFF;

uint8_t AdvData::advProt = ADV_PROT;
uint16_t AdvData::advDelay = ADV_DELAY;
uint16_t AdvData::repeatCmdsCount = REPEAT_CMDS_COUNT;
bool AdvData::advertiseByCount = true;
bool AdvData::invertCh = false;


void AdvData::setup(){
    BLEDevice::init(DEVICE_NAME);
    BLEServer *pServer = BLEDevice::createServer();
    pAdvertising = pServer->getAdvertising();
}

void AdvData::start(uint8_t cmd){
    currProt = F9;
    currCmd = cmd;
    sentCmdsCount = 0;
    advStopped = true;
    startAdvTime = millis();
}

void AdvData::advertise(){
    uint32_t currTime = millis();
    bool prepareNextAdv = (currTime - startAdvTimeForProt > advDelay);
    bool conditionRes = advertiseByCount ?
            (sentCmdsCount < repeatCmdsCount) :
            (currTime - startAdvTime < ADV_TIME && currTime > ADV_TIME);

    if (conditionRes){
        if(prepareNextAdv){
            startAdvTimeForProt = millis();
            
            prepareAdvertising();
            sentCmdsCount++; // used for advertising by count only
        }
    }
    else if(!advStopped && prepareNextAdv){
        advStopped = true;
        pAdvertising->stop();
    }
}

void AdvData::advRaw(char* rawData, uint8_t dataLen){
    hex2bin(rawData, advData);
    advLen = dataLen/2;
    
    start(0xFF); // select raw data mode
}

void AdvData::setRepeatCmdsCount(uint16_t repeatCmdsCount){
    this->repeatCmdsCount = repeatCmdsCount;
    sentCmdsCount = repeatCmdsCount;
}

void AdvData::printSettings(){
    Serial.println("\nCurrent settings:");

    Serial.print("\tCCT Level: ");
    Serial.println(colour);

    Serial.print("\tBrightness: ");
    Serial.println(brightness);
    
    Serial.print("\tPackets to send: ");
    Serial.println(repeatCmdsCount);

    Serial.print("\tAdvertisement protocol: ");
    if(advProt == F9)
        Serial.println("F9");
    else if(advProt == F0)
        Serial.println("F0");
    else if(advProt == ALL)
        Serial.println("ALL");
    else
        Serial.println("ERROR");

    Serial.print("\tAdvertisement delay: ");
    Serial.println(advDelay);
    
    Serial.print("\tAdvertisement mode: ");
    if(advertiseByCount)
        Serial.println("by count");
    else
        Serial.println("by time");

    Serial.print("\tInvert channels: ");
    if(invertCh)
        Serial.println("true");
    else
        Serial.println("false");
    
    Serial.println("");
}

void AdvData::setAdvData(BLEAdvertisementData &adv) {
    char arr[5];
    arr[0] = 0x02; // flags length
    arr[1] = 0x01; // flag1
    arr[2] = 0x02; // flag2
    arr[3] = advLen + 1; // data length
    
    if(currProt == F9)
        arr[4] = 0xff; // data type
    else if(currProt == F0)
        arr[4] = 0x03; // different data type for the F0 protocol

    std::string res = std::string(arr, 5) + std::string(advData, advLen);
    adv.addData(res);
}

void AdvData::setRawAdvData(BLEAdvertisementData &adv) {
    //uint8_t adr[6] = {0x42, 0x92, 0x53, 0x96, 0x01, 0x65}; 
    //pAdvertising->setAdvertisementType(ADV_TYPE_NONCONN_IND);
    //pAdvertising->setDeviceAddress(adr, BLE_ADDR_TYPE_RANDOM);

    std::string res = std::string(advData, advLen);
    adv.addData(res);
}

void AdvData::prepareAdvertising(){
    BLEAdvertisementData advData;

    if(!advStopped)
        pAdvertising->stop();

    // If raw data is advertised
    if(currCmd == 0xFF)
        setRawAdvData(advData);
    else{
        // If F0 or F9 data has to be advertised
        getAdvData(currCmd);
        setAdvData(advData);
    }

    pAdvertising->setAdvertisementData(advData);
    pAdvertising->start();

    advStopped = false;
}

uint8_t AdvData::getNextProt(){
    if(advProt == ALL){
        currProt++;
        if(currProt == PROT_COUNT)
            currProt = F9;
    }
    else
        currProt = advProt;

    return currProt;
}

void AdvData::brAndCCTtoPWM(uint8_t &valCh1, uint8_t &valCh2){
    uint8_t maxCh1 = (colour <= 50) ? 255 : (uint8_t)(2.55 * (100 - colour) + 0.5);
    uint8_t maxCh2 = (colour >= 50) ? 255 : (uint8_t)(2.55 * colour + 0.5);

    valCh1 = (uint8_t)(maxCh1 * (brightness / 100.0) + 0.5);
    valCh2 = (uint8_t)(maxCh2 * (brightness / 100.0) + 0.5);

    if(invertCh){
        uint8_t temp = valCh1;
        valCh1 = valCh2;
        valCh2 = temp;
    }
}

void AdvData::getAdvData(uint8_t cmd){
    getNextProt();

    if(currProt == F9){
        uint8_t valCh1, valCh2;

        brAndCCTtoPWM(valCh1, valCh2);
        p1.getAdvData(cmd, valCh1, valCh2, (uint8_t*)advData);
        advLen = 25;
    }
    else if(currProt == F0){
        uint8_t valCh1, valCh2;

        brAndCCTtoPWM(valCh1, valCh2);
        p2.getAdvData(cmd, valCh1, valCh2, advData);
        advLen = 26;
    }
}

uint8_t AdvData::char2int(char input){
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  
  return -1;
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void AdvData::hex2bin(const char* src, char* target){
  while(*src && src[1]){
    *(target++) = char2int(*src)*16 + char2int(src[1]);
    src += 2;
  }
}