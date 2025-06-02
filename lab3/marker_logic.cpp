#include "marker_logic.h"
#include <iostream>
using namespace std;

CRITICAL_SECTION cs;
HANDLE startEvent;
vector<HANDLE> resumeEvents, stopEvents, doneEvents, threadHandles;
vector<vector<int>> markedIndices;
vector<int> sharedArray;
int arraySize;
atomic<int> waitingThreads = 0;

DWORD WINAPI markerThread(LPVOID param) {
    int id = *(int*)param;
    srand(id);
    vector<int>& myMarks = markedIndices[id];

    WaitForSingleObject(startEvent, INFINITE);

    while (true) {
        bool marked = false;
        for (int i = 0; i < 10; ++i) {
            int idx = rand() % arraySize;

            EnterCriticalSection(&cs);
            if (sharedArray[idx] == 0) {
                sharedArray[idx] = id + 1;
                myMarks.push_back(idx);
                LeaveCriticalSection(&cs);
                this_thread::sleep_for(chrono::milliseconds(5));
                marked = true;
                break;
            }
            LeaveCriticalSection(&cs);
        }

        if (!marked) {
            EnterCriticalSection(&cs);
            cout << "Thread " << id << " can't continue. Marked: " << myMarks.size() << endl;
            for (int idx : myMarks) cout << idx << " ";
            cout << "\n\n";
            LeaveCriticalSection(&cs);

            waitingThreads++;
            SetEvent(doneEvents[id]);

            HANDLE events[2] = { resumeEvents[id], stopEvents[id] };
            DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);

            if (waitResult == WAIT_OBJECT_0 + 1) {
                EnterCriticalSection(&cs);
                for (int idx : myMarks) sharedArray[idx] = 0;
                LeaveCriticalSection(&cs);
                break;
            }

            waitingThreads--;
        }

        this_thread::sleep_for(chrono::milliseconds(5));
    }

    return 0;
}

void printArray() {
    EnterCriticalSection(&cs);
    cout << "Array: ";
    for (int v : sharedArray) cout << v << " ";
    cout << "\n";
    LeaveCriticalSection(&cs);
}