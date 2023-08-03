#include "esp_bt_device.h"
#include "AdvProtocolF0.h"

void AdvProtocolF0::getAdvData(uint8_t cmd, uint8_t valCh1, uint8_t valCh2, char* output){
    uint8_t advData[32];

    if(cmd == SETUP){
        blevbBinding(advData);
    }
    else if(cmd == ON){
        blevbLampOnOff(true, advData);
    }
    else if(cmd == OFF){
        blevbLampOnOff(false, advData);
    }
    else if(cmd == LIGHT){
        blevbDimming(valCh1, valCh2, advData);
    }

    memcpy(output, &advData[5], 26);
}

void AdvProtocolF0::getId(uint8_t* id){
    const uint8_t* btMac = esp_bt_dev_get_address();
    id[0] = btMac[2];
    id[1] = btMac[3];
    id[2] = btMac[4];
    id[3] = btMac[5];
}

void AdvProtocolF0::prepareFunctionData(uint8_t* data){
    int param_3 = 256, param_5 = 0;
    
    uint8_t id[4] = {0,};
    getId(id);

    memset(data, 0, 24);

    data[4] = (uint8_t)(param_3 & 0xFF); //LSB
    data[5] = (uint8_t)((param_3 >> 8) & 0xFF); //MSB
    
    data[8] = id[0];
    data[9] = id[1];
    data[10] = id[2];
    data[11] = id[3];

    data[12] = (uint8_t)(param_5 & 0xFF);
}

void AdvProtocolF0::blevbBinding(uint8_t* result){
    uint8_t data[25];
    prepareFunctionData(data);

    data[14] = 0x28;

    ble_v2_encode(data, result);
}

void AdvProtocolF0::blevbLampOnOff(bool state, uint8_t* result){
    uint8_t data[25];
    prepareFunctionData(data);

    data[14] = (uint8_t)((0x11 - state) & 0xFF);

    ble_v2_encode(data, result);
}

void AdvProtocolF0::blevbDimming(uint8_t valCh1, uint8_t valCh2, uint8_t* result){
    uint8_t data[25];
    prepareFunctionData(data);

    data[14] = 0x21;
    data[18] = valCh1;
    data[19] = valCh2;

    ble_v2_encode(data, result);
}

uint16_t AdvProtocolF0::v2_crc16_ccitt(long data, uint8_t numBytes, uint16_t init){
  uint16_t s;
  s = init;

  for (uint8_t n = 0; n < numBytes; ++n){
    s = s ^ ((uint16_t)*((uint8_t *)(data + n)) << 8);

    for (uint8_t i = 0; i < 8; ++i){
      if ((s & 0x8000) == 0){
        s = s << 1;
      }
      else{
        s = (s << 1) ^ 0x1021;
      }
    }
  }
  return s;
}

bool AdvProtocolF0::ble_v2_encode(uint8_t *param_1, uint8_t *param_2){
    unsigned short crc;
    unsigned int rnd;
    uint8_t rnd_msb;
    uint8_t crc_msb;
    uint8_t res;
    uint8_t rnd_lsb;
    uint8_t tmp;
    uint8_t tx_count = 5; // for example tx_count=5

    *(unsigned long long *)(param_2 + 23) = 0;
    *(unsigned long long *)(param_2 + 16) = 0;
    *(unsigned long long *)(param_2 + 8) = 0;
    *(unsigned long long *)param_2 = 0;

    if (param_1 == NULL) {
        res = 0xff;
    } else {
        rnd = rand();
        *param_2 = 2;
        param_2[1] = 1;
        param_2[2] = 2;
        param_2[3] = 27;
        param_2[4] = 0x16; 
        // all bytes above will be overwritten when preparing the package later
        
        param_2[5] = 0xf0; // real data starts here
        param_2[6] = 0x8;
        param_2[7] = 0x10;
        param_2[8] = 0x80;
        param_2[9] = 0;
        param_2[10] = tx_count++;
        param_2[11] = param_1[4];
        param_2[12] = param_1[5];
        param_2[13] = param_1[8];
        param_2[14] = param_1[9];
        param_2[15] = param_1[10];
        param_2[16] = param_1[11];
        param_2[17] = param_1[12];
        param_2[18] = param_1[14];
        param_2[19] = param_1[15];
        param_2[20] = param_1[16];
        param_2[21] = param_1[17];
        param_2[22] = param_1[18];
        param_2[23] = param_1[19];
        param_2[24] = param_1[20];
        param_2[25] = param_1[21];
        param_2[26] = param_1[22];

        rnd_lsb = (uint8_t)rnd;

        for (uint8_t i = 9; i < 27; ++i) {
            param_2[i] = param_2[i] ^ XBOXES[((rnd_lsb + i) & 0b00011111) + ((param_1[1] & 0b00000011) << 5)];
            param_2[i] = param_2[i] ^ rnd_lsb;
        }

        param_2[27] = rnd_lsb;
        rnd_msb = (uint8_t)(rnd >> 8);
        param_2[28] = rnd_msb;
        crc = v2_crc16_ccitt((long)(param_2 + 7), 22, ~(rnd & 0xffff));
        param_2[29] = (uint8_t)crc;
        crc_msb = (uint8_t)((ushort)crc >> 8);
        param_2[30] = crc_msb;
        res = 0;
    }
    return (bool)res;
}

void AdvProtocolF0::reverse(uint8_t* data){
    for (int i = 0; i < 25; i+=2){
        uint8_t tmp = data[i];
        data[i] = data[i+1];
        data[i+1] = tmp;
    }
}
