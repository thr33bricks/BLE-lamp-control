#ifndef SCANBLE_H
#define SCANBLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define TIME_SCAN 2 // in seconds


class ScanBLE{
    public:
        ScanBLE();
        void scan(uint16_t timeToScan);

        static int rssiTres;
    private:
        uint16_t getScanDevCount(BLEScanResults totalScanDevices);

        BLEScan* pBLEScan;

        class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
            void onResult(BLEAdvertisedDevice dev) {
                int rssi = dev.getRSSI();
                int payloadSize = dev.getPayloadLength();
                uint8_t* payloadPointer = dev.getPayload();
                uint8_t adrType = dev.getAddressType();
                BLEAddress adr = dev.getAddress();

                if(rssi < rssiTres)
                    return;

                Serial.println("\n---------------------------------------------");
                Serial.print("RSSI: ");
                Serial.println(rssi);

                Serial.print("MAC: ");
                Serial.println(adr.toString().c_str());

                Serial.print("Address type: ");
                switch (adrType){
                    case 0:
                        Serial.println("Public");
                        break;
                    case 1:
                        Serial.println("Random");
                        break;
                    case 2:
                        Serial.println("RPA Public");
                        break;
                    case 3:
                        Serial.println("RPA Random");
                        break;
                    default:
                        Serial.println("Unknown");
                        break;
                }
                
                Serial.print("Payload: ");
                for (int i = 0; i < payloadSize; i++){
                    Serial.printf("%02X",payloadPointer[i]);
                }

                Serial.print("\nPayload len: ");
                Serial.println(payloadSize);
                Serial.println("---------------------------------------------");
            }
        };
};

#endif