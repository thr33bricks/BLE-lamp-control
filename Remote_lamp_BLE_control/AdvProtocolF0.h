#ifndef ADV_PROTOCOL2_H
#define ADV_PROTOCOL2_H

#include <Arduino.h>
#include "Cmd.h"

#define XBOXES_SIZE 128

class AdvProtocolF0{
    public:
        void getAdvData(uint8_t cmd, uint8_t valCh1, uint8_t valCh2, char* output);
    private:
        void blevbBinding(uint8_t* result);
        void blevbLampOnOff(bool state, uint8_t* result);
        void blevbDimming(uint8_t valCh1, uint8_t valCh2, uint8_t* result);
        uint16_t v2_crc16_ccitt(long param_1, uint8_t param_2, uint16_t param_3);
        bool ble_v2_encode(uint8_t *param_1, uint8_t *param_2);
        void reverse(uint8_t* data);
        void prepareFunctionData(uint8_t* data);
        void getId(uint8_t* id);

        const uint8_t XBOXES[XBOXES_SIZE] = { 
            0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 
            0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 
            0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 
            0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 
            0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 
            0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 
            0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 
            0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 
            0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 
            0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 
            0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 
            0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 
            0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 
            0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 
            0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 
            0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };
};

#endif