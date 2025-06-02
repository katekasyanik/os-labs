#pragma once

#include <windows.h>
#include <vector>
#include <thread>
#include <random>
#include <atomic>
#include <chrono>

extern CRITICAL_SECTION cs;
extern HANDLE startEvent;
extern std::vector<HANDLE> resumeEvents, stopEvents, doneEvents, threadHandles;
extern std::vector<std::vector<int>> markedIndices;
extern std::vector<int> sharedArray;
extern int arraySize;
extern std::atomic<int> waitingThreads;

DWORD WINAPI markerThread(LPVOID param);
void printArray();