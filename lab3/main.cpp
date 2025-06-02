#include "marker_logic.h"
#include <iostream>
#include <algorithm>
using namespace std;

int main() {
    InitializeCriticalSection(&cs);

    cout << "Enter array size: ";
    cin >> arraySize;
    sharedArray.resize(arraySize, 0);

    cout << "Enter number of marker threads: ";
    int n;
    cin >> n;

    startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    resumeEvents.resize(n);
    stopEvents.resize(n);
    doneEvents.resize(n);
    markedIndices.resize(n);
    threadHandles.resize(n);

    vector<int> ids(n);
    vector<bool> active(n, true);

    for (int i = 0; i < n; ++i) {
        ids[i] = i;
        resumeEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        stopEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        doneEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        threadHandles[i] = CreateThread(NULL, 0, markerThread, &ids[i], 0, NULL);
    }

    SetEvent(startEvent);

    while (true) {
        cout << "Waiting for all active threads to pause...\n";
        while (waitingThreads < count(active.begin(), active.end(), true)) {
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        printArray();

        cout << "Enter thread number to terminate: ";
        int killId;
        cin >> killId;

        if (killId < 0 || killId >= n || !active[killId]) {
            cout << "Invalid thread number. Try again.\n";
            continue;
        }

        SetEvent(stopEvents[killId]);
        WaitForSingleObject(threadHandles[killId], INFINITE);
        active[killId] = false;

        printArray();

        for (int i = 0; i < n; ++i) {
            if (active[i])
                SetEvent(resumeEvents[i]);
        }

        if (count(active.begin(), active.end(), true) == 0)
            break;
    }

    DeleteCriticalSection(&cs);
    return 0;
}
