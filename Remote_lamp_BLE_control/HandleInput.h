#ifndef HANDLE_INPUT_H
#define HANDLE_INPUT_H

#include <Arduino.h>
#include "AdvData.h"
#include "ScanBLE.h"

#define MSG_LENGTH 128
#define ARG_LENGTH 64
#define MAX_ARGS 5
#define START_MARKER '/'
#define END_MARKER '\n'


class HandleInput{
    public:
        void handle();
    private:
        void recvWithStartEndMarkers();
        void processArguments();
        int getAndValidateNumber(const char* str, int min, int max);
        bool getArgument(uint8_t argNum);
        bool argEquals(const char* str);

        char argument[ARG_LENGTH];
        char message[MSG_LENGTH];

        //even indexes mean argument string start position,
        //odd indexes mean end string position
        uint8_t stringsLocations[MAX_ARGS * 2] = {0,};
        
        bool newMessage = false;
        uint8_t argCount = 1;
        uint8_t currArgLen = 0;
        
        AdvData advert;
};

#endif