#pragma once

const int MAX_MESSAGE_LENGTH = 20;

struct Message {
    char text[MAX_MESSAGE_LENGTH];
};

struct SharedHeader {
    int readIndex;
    int writeIndex;
    int maxMessages;
};