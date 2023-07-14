#include "ScanBLE.h"

int ScanBLE::rssiTres = -100;


ScanBLE::ScanBLE(){
    //BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);  // less or equal setInterval value
}

// Time to scan in seconds
void ScanBLE::scan(uint16_t timeToScan){
    uint16_t iterations = timeToScan / TIME_SCAN;
    Serial.println("Scanning...");

    for(uint16_t i = 0; i < iterations; i++){
        BLEScanResults foundDevices = pBLEScan->start(TIME_SCAN, false);
        uint8_t devCount = getScanDevCount(foundDevices);

        if(devCount != 0){
            Serial.println("------------------------------------------------------------------------------------------");
            Serial.print("Devices found: ");
            Serial.println(getScanDevCount(foundDevices));
            Serial.println("Latest scan data");
            Serial.println("------------------------------------------------------------------------------------------");
        }
        
        pBLEScan->clearResults();   // delete results from BLEScan buffer to release memory
        delay(100);
    }

    Serial.println("Scanning finished!");
}

uint16_t ScanBLE::getScanDevCount(BLEScanResults totalScanDevices){
  uint16_t devCount = 0;

  for (int i = 0; i < totalScanDevices.getCount(); i++){
    BLEAdvertisedDevice dev = totalScanDevices.getDevice(i);

    if(dev.getRSSI() > rssiTres)
      devCount++;
  }

  return devCount;
}
