#include "HandleInput.h"

ScanBLE bleScan;

void HandleInput::handle(){
    recvWithStartEndMarkers();

    if(newMessage){
        bool success = false;

        processArguments();
        getArgument(1);

        if(argEquals("send")){
            getArgument(2);

            if(argEquals("on")){
                advert.start(ON);
                success = true;
            }
            else if(argEquals("off")){
                advert.start(OFF);
                success = true;
            }
            else if(argEquals("setup")){
                advert.start(SETUP);
                success = true;
            }
            else if(argEquals("cw")){
                // cold white with medium brightness
                advert.colour = 100;
                advert.brightness = 50;
                advert.start(LIGHT);
                success = true;
            }
            else if(argEquals("ww")){
                // warm white with medium brightness
                advert.colour = 0;
                advert.brightness = 50;
                advert.start(LIGHT);
                success = true;
            }
            else if(argEquals("neutral")){
                // neutral with medium brightness
                advert.colour = 50;
                advert.brightness = 50;
                advert.start(LIGHT);
                success = true;
            }
            else if(argEquals("sleep")){
                // neutral with low brightness
                advert.colour = 50;
                advert.brightness = 10;
                advert.start(LIGHT);
                success = true;
            }
            else if(argEquals("cct_lvl")){
                getArgument(3);

                int colorTemp = getAndValidateNumber(argument, 0, 100);
                if(colorTemp != -1){
                    advert.colour = colorTemp;
                    advert.start(LIGHT);
                    success = true;
                }
            }
            else if(argEquals("brightness")){
                getArgument(3);

                int brightness = getAndValidateNumber(argument, 0, 100);
                if(brightness != -1){
                    advert.brightness = brightness;
                    advert.start(LIGHT);
                    success = true;
                }
            }
        }
        else if(argEquals("sendraw")){
            getArgument(2);
            
            if(currArgLen < 63 && currArgLen % 2 == 0){
                advert.advRaw(argument, currArgLen);
                success = true;
            }
        }
        else if(argEquals("set")){
            getArgument(2);
            
            if(argEquals("protocol")){
                getArgument(3);
                
                if(argEquals("F9")){
                    advert.advProt = AdvData::F9;
                    success = true;
                }
                else if(argEquals("F0")){
                    advert.advProt = AdvData::F0;
                    success = true;
                }
                else if(argEquals("ALL")){
                    advert.advProt = AdvData::ALL;
                    success = true;
                }
            }
            if(argEquals("pks_to_send")){
                getArgument(3);

                int cmdsCount = getAndValidateNumber(argument, 1, MAX_REPEAT_CMDS);
                if(cmdsCount != -1){
                    advert.setRepeatCmdsCount(cmdsCount);
                    success = true;
                }
            }
            if(argEquals("adv_delay")){
                getArgument(3);

                int advDelay = getAndValidateNumber(argument, 0, MAX_ADV_DELAY);
                if(advDelay != -1){
                    advert.advDelay = advDelay;
                    success = true;
                }
            }
            if(argEquals("mode")){
                getArgument(3);

                if(argEquals("count")){
                    advert.advertiseByCount = true;
                    success = true;
                }
                else if(argEquals("time")){
                    advert.advertiseByCount = false;
                    success = true;
                }
            }
            if(argEquals("inverted")){
                getArgument(3);

                if(argEquals("true")){
                    advert.invertCh = true;
                    success = true;
                }
                else if(argEquals("false")){
                    advert.invertCh = false;
                    success = true;
                }
            }
        }
        else if(argEquals("get_settings")){
            advert.printSettings();
            success = true;
        }
        else if(argEquals("scan")){
            getArgument(2);
            int seconds = getAndValidateNumber(argument, 2, 300);

            getArgument(3);
            int rssi = getAndValidateNumber(argument, 1, 100);

            if(seconds != -1 && rssi != -1){
                bleScan.rssiTres = rssi * -1;
                bleScan.scan(seconds);

                success = true;
            }
        }
        else if(argEquals("help") || argEquals("h")){
            Serial.println("\nTool developed by Yordan Yordanov and Rostislav Kandilarov, July 2023");

            Serial.println("\n/send {on|off|setup|cw|ww|neutral|sleep}");
            Serial.println("\t- cw, ww correspond to cool white and warm white, neutral means neutral white.");
            Serial.println("/send brightness {0-100}");
            Serial.println("\t- 0 = off");
            Serial.println("\t- 100 = max");
            Serial.println("/send cct_lvl {0-100}");
            Serial.println("\t- Correlated colour temperature (CCT) level where 0 and 100 stand for coldest and warmest cct and every value in between is a mix between cold and warm light.");
            
            Serial.println("\n/sendraw {packet}");
            Serial.println("\t- Packet should be 1-31 bytes (2-62 chars) and should contain a data length byte as a first byte.");
            Serial.println("\t- The /scan command can be used first and then the desired packet could be sent with this command. Copying advertising data from Wireshark is done by selecting Advertising data > Right click > Copy > as a Hex Stream.");

            Serial.println("\n/set protocol {F9|F0|ALL}");
            Serial.println("\t- Default = ALL");
            Serial.println("\t- The tool supports a number of protocols to be used with the /send command. Depending on the led controller a suitable protocol could be selected or it can be left at default state.");
            Serial.print("/set pks_to_send {1-");
            Serial.print(MAX_REPEAT_CMDS);
            Serial.println("}");
            Serial.println("\t- Default = 4");
            Serial.println("\t- How many advertising packets to send per command.");
            Serial.print("/set adv_delay {0-");
            Serial.print(MAX_ADV_DELAY);
            Serial.println("}");
            Serial.println("\t- Default = 150");
            Serial.println("\t- Time (in ms) before sending the next packet.");
            Serial.println("/set mode {count|time}");
            Serial.println("\t- Default = count");
            Serial.println("\t- How packets are expected to be sent when advertising. When advertising by time packets are sent for 5 seconds.");
            Serial.println("/set inverted {true|false}");
            Serial.println("\t- Default = false");
            Serial.println("\t- Inversion of warm and cold channels.");

            Serial.println("\n/get_settings");
            Serial.println("\t- Returns all current settings.");

            Serial.println("\n/scan {2-300} {1-100}");
            Serial.println("\t- First argument is for how much time to scan (in seconds). Second argument is used for an RSSI filter and only packets with higher RSSI value are shown. Its value is interpreted as a NEGATIVE number.");
            Serial.println("\t- /scan is actually a basic BLE packet sniffer.");
            Serial.println("");

            success = true;
        }

        if(!success)
            Serial.println("Incorrect syntax!");

        newMessage = false;
    }
}

bool HandleInput::argEquals(const char* str){
    return !strcmp(argument, str);
}

int HandleInput::getAndValidateNumber(const char* str, int min, int max) {
    const uint8_t len = strlen(str);

    // is it a number
    for(uint8_t i = 0; i < len; ++i) {
        if(!isdigit(str[i]))
            return -1; // not a number
    }

    int num = atoi(str);
    if(num >= min && num <= max)
        return num;
    else
        return -1;
}

bool HandleInput::getArgument(uint8_t argNum){
    argument[0] = '\0'; // If argument is invalid this should serve as a safety measure

    if(argNum > argCount)
        return false;
    
    //including
    uint8_t start = stringsLocations[argNum * 2 - 2];
    uint8_t stop = stringsLocations[argNum * 2 - 1];
    uint8_t len = stop - start + 1;

    if(len > ARG_LENGTH - 1)
        return false;

    memcpy(argument, &message[start], len);
    argument[len] = '\0';

    currArgLen = len;

    Serial.print("Arg");
    Serial.print(argNum);
    Serial.print(": ");
    Serial.println(argument);
}

void HandleInput::processArguments(){
    uint8_t currMsgInd = 0;

    argCount = 1;
    stringsLocations[0] = 0;

    while(message[currMsgInd] != '\0'){
        if(message[currMsgInd] == ' '){
            if(argCount == MAX_ARGS)
                break;
            argCount++;

            // Save last argument end
            stringsLocations[(argCount-1)*2 - 1] = (currMsgInd != 0) ? (currMsgInd - 1) : 0;

            // Save current argument start
            stringsLocations[(argCount)*2 - 2] = currMsgInd + 1;
        }
        currMsgInd++;
    }
    stringsLocations[argCount*2 - 1] = currMsgInd - 1;

    // Safety precaution if last char was a space
    if(message[currMsgInd-1] == ' ')
        argCount--;
}

void HandleInput::recvWithStartEndMarkers() {
    bool recvInProgress = false;
    uint8_t ndx = 0;
    char rc;
 
    while (Serial.available() > 0 && newMessage == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != END_MARKER) {
                message[ndx] = rc;
                ndx++;
                if (ndx >= MSG_LENGTH) {
                    ndx = MSG_LENGTH - 1;
                }
            }
            else {
                message[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;

                newMessage = true;
            }
        }
        else if (rc == START_MARKER) {
            recvInProgress = true;
        }
    }
}