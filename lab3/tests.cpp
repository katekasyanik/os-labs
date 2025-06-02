#include "marker_logic.h"
#include <gtest/gtest.h>

TEST(MarkerSyncTests, InitialArrayState) {
    sharedArray.clear();
    EXPECT_EQ(sharedArray.size(), 0);
}

TEST(MarkerSyncTests, ArrayAfterInitialization) {
    int size = 10;
    sharedArray.resize(size, 0);
    EXPECT_EQ(sharedArray.size(), size);
    for (auto v : sharedArray) EXPECT_EQ(v, 0);
}

TEST(MarkerSyncTests, CriticalSectionLock) {
    InitializeCriticalSection(&cs);
    EnterCriticalSection(&cs);
    sharedArray.push_back(42);
    LeaveCriticalSection(&cs);
    EXPECT_EQ(sharedArray.back(), 42);
    DeleteCriticalSection(&cs);
}

TEST(MarkerSyncTests, WaitingThreadsCount) {
    waitingThreads = 0;
    EXPECT_EQ(waitingThreads.load(), 0);
    waitingThreads++;
    EXPECT_EQ(waitingThreads.load(), 1);
}