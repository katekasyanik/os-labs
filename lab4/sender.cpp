#define NOMINMAX
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <cstring>
#include "shared.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: sender <filename> <id>\n";
        return 1;
    }

    std::string filename = argv[1];
    int id = std::stoi(argv[2]);

    std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        std::cerr << "Ошибка открытия файла\n";
        return 1;
    }

    SharedHeader header;
    file.seekg(0);
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    HANDLE mutex = OpenMutexA(SYNCHRONIZE, FALSE, (filename + "_mutex").c_str());
    if (!mutex) {
        std::cerr << "Ошибка открытия мьютекса\n";
        return 1;
    }

    std::string eventName = "SenderReady_" + std::to_string(id);
    HANDLE readyEvent = OpenEventA(EVENT_MODIFY_STATE, FALSE, eventName.c_str());
    if (!readyEvent) {
        std::cerr << "Ошибка открытия события\n";
        return 1;
    }

    SetEvent(readyEvent);
    std::cout << "[Sender " << id << "] Готов к работе\n";

    while (true) {
        std::string command;
        std::cout << "[Sender " << id << "] Команда (send/exit): ";
        std::cin >> command;

        if (command == "exit") break;
        if (command != "send") continue;

        std::string msgText;
        std::cout << "Введите сообщение: " << std::flush;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(std::cin, msgText);
        if (msgText.size() >= MAX_MESSAGE_LENGTH)
            msgText = msgText.substr(0, MAX_MESSAGE_LENGTH - 1);

        WaitForSingleObject(mutex, INFINITE);

        file.seekg(0);
        file.read(reinterpret_cast<char*>(&header), sizeof(header));

        file.seekg(sizeof(SharedHeader) + header.writeIndex * sizeof(Message));
        Message msg;
        file.read(reinterpret_cast<char*>(&msg), sizeof(Message));

        if (strlen(msg.text) != 0) {
            ReleaseMutex(mutex);
            std::cout << "[Sender " << id << "] Очередь заполнена. Ожидание...\n";
            Sleep(1000);
            continue;
        }

        strncpy(msg.text, msgText.c_str(), MAX_MESSAGE_LENGTH);
        file.seekp(sizeof(SharedHeader) + header.writeIndex * sizeof(Message));
        file.write(reinterpret_cast<char*>(&msg), sizeof(Message));

        header.writeIndex = (header.writeIndex + 1) % header.maxMessages;
        file.seekp(0);
        file.write(reinterpret_cast<char*>(&header), sizeof(header));
        file.flush();

        ReleaseMutex(mutex);
    }

    return 0;

}