#include "esp_bt_device.h"
#include "AdvProtocolF9.h"

void AdvProtocolF9::getAdvData(uint8_t cmd, uint8_t valCh1, uint8_t valCh2, uint8_t *output){
    if (cmd == SETUP){
        uint8_t id[2] = {0,};
        getId(id);
        buildPacket(0x28, id[0], id[1], output);
    }
    else if (cmd == ON){
        buildPacket(0x10, 0x00, 0x00, output);
    }
    else if (cmd == OFF){
        buildPacket(0x11, 0x00, 0x00, output);
    }
    else if (cmd == LIGHT){
        buildPacket(0x21, valCh1, valCh2, output);
    }
}

void AdvProtocolF9::bitReverse(uint8_t *bArr){
    for (int i = 0; i < 25; i++){
        uint8_t rev = 0;
        for (int j = 0; j < 8; j++){
            rev += (((bArr[i] & 255) >> (7 - j)) & 1) << j;
        }
        bArr[i] = rev;
    }
}

void AdvProtocolF9::bleWhitening(uint8_t *bArr){
    int key = 83;

    for (uint8_t i = 0; i < 38; ++i){
        uint8_t b = 0;

        for (uint8_t bit = 0; bit < 8; ++bit){
            b |= ((((key & 64) >> 6) << bit) ^ (bArr[i])) & (1 << bit);
            uint8_t k6 = (key >> 6) & 1;
            int i9 = ((key << 1) & -2) | k6;
            key = ((i9 ^ (k6 << 4)) & 16) | (i9 & -17);
        }
        bArr[i] = b;
    }
}

void AdvProtocolF9::bleWhiteningForPacket(uint8_t *bArr){
    uint8_t whArr[38];
    for (uint8_t i = 0; i < 25; ++i){
        whArr[i + 13] = bArr[i];
    }

    bleWhitening(whArr);
    for (uint8_t i = 0; i < 25; ++i){
        bArr[i] = whArr[i + 13];
    }
}

uint16_t AdvProtocolF9::CRC16(uint8_t *bArr, uint8_t offset){
    uint16_t crc = 65535;
    for (uint8_t i = 0; i < 12; ++i){
        crc = CRC_TABLE[((crc >> 8) ^ bArr[offset + i]) & 255] ^ (crc << 8);
    }
    return crc;
}

void AdvProtocolF9::getId(uint8_t *id){
    const uint8_t *btMac = esp_bt_dev_get_address();
    id[0] = btMac[4];
    id[1] = btMac[5];
}

void AdvProtocolF9::buildPacket(uint8_t command, uint8_t arg1, uint8_t arg2, uint8_t *packet){
    uint8_t id[2] = {0,};
    getId(id);

    for (uint8_t i = 0; i < 11; ++i){
        packet[i] = PAYLOAD_PREFIX[i];
    }

    packet[11] = command;
    packet[12] = id[0];
    packet[13] = id[1];
    packet[14] = arg1;
    packet[15] = arg2;
    packet[16] = 0;
    packet[17] = rand() & 0xFF;

    for (uint8_t i = 18; i < 23; ++i){
        packet[i] = 0;
    }

    uint16_t crc = CRC16(packet, 11);
    packet[23] = (crc >> 8) & 0xFF;
    packet[24] = crc & 0xFF;

    bitReverse(packet);
    bleWhiteningForPacket(packet);
}
