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

uint16_t AdvProtocolF0::v2_crc16_ccitt(long param_1, uint8_t param_2, uint16_t param_3){
  uint8_t local_10;
  uint8_t local_f;
  uint16_t local_e;
  
  local_e = param_3;
  for (local_f = 0; local_f < param_2; local_f = local_f + 1)
  {
    local_e = local_e ^ ((uint16_t)*((uint8_t *)(param_1 + (uint64_t)local_f)) << 8);
    for (local_10 = 8; local_10 != 0; local_10 = local_10 - 1)
    {
      if ((local_e & 0x8000) == 0)
      {
        local_e = local_e << 1;
      }
      else
      {
        local_e = (local_e << 1) ^ 0x1021;
      }
    }
  }
  return local_e;
}

bool AdvProtocolF0::ble_v2_encode(uint8_t *param_1, uint8_t *param_2){
    unsigned short crc;
    unsigned int rnd;
    uint8_t i;
    uint8_t rnd_msb;
    uint8_t crc_msb;
    uint8_t res;
    uint8_t rnd2;
    uint8_t tmp;
    uint8_t tx_count = 5; // for example tx_count=5

    *(unsigned long long *)(param_2 + 23) = 0;
    *(unsigned long long *)(param_2 + 16) = 0;
    *(unsigned long long *)(param_2 + 8) = 0;
    *(unsigned long long *)param_2 = 0;

    if (param_1 == NULL) {
        res = 0xff;
    } else {
        rnd = esp_random();
        *param_2 = 2;
        param_2[1] = 1;
        param_2[2] = 2;
        param_2[3] = 27;
        param_2[4] = 0x16;
        param_2[5] = 0xf0;
        param_2[6] = 0x8;
        param_2[7] = 0x10;
        param_2[8] = 0x80;
        param_2[9] = 0;
        tmp = tx_count + 1;
        param_2[10] = tx_count;
        tx_count = tmp;
        param_2[11] = param_1[4];
        param_2[12] = param_1[5];
        param_2[13] = (uint8_t)*(unsigned int *)(param_1 + 8);
        param_2[14] = (uint8_t)(((unsigned int)*(unsigned int *)(param_1 + 8)) >> 8);
        param_2[15] = (uint8_t)*(unsigned short *)(param_1 + 10);
        param_2[16] = param_1[11];
        param_2[0x11] = param_1[12];
        param_2[0x12] = param_1[14];
        param_2[0x13] = param_1[15];
        param_2[0x14] = param_1[16];
        param_2[0x15] = param_1[17];
        param_2[0x16] = param_1[18];
        param_2[0x17] = param_1[19];
        param_2[0x18] = param_1[20];
        param_2[25] = param_1[21];
        param_2[26] = param_1[22];
        tmp = param_1[1];
        for (i = 9; rnd2 = (uint8_t)rnd, i < 27; i = i + 1) {
            param_2[i] = param_2[i] ^ XBOXES[(unsigned long)(rnd2 + i & 0b00011111) + (unsigned long)(tmp & 0b00000011) * 32];
            param_2[i] = param_2[i] ^ rnd2;
        }
        param_2[27] = rnd2;
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
