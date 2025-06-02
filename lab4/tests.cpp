#include <gtest/gtest.h>
#include <fstream>
#include <cstring>
#include "shared.h"

TEST(SharedHeaderTest, InitializeHeader) {
    SharedHeader header = {0, 0, 5};
    EXPECT_EQ(header.readIndex, 0);
    EXPECT_EQ(header.writeIndex, 0);
    EXPECT_EQ(header.maxMessages, 5);
}

TEST(MessageTest, WriteAndReadMessage) {
    const char* filename = "test_msg.bin";
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    ASSERT_TRUE(out.is_open());

    Message msg = {};
    strncpy(msg.text, "hello", MAX_MESSAGE_LENGTH);
    out.write(reinterpret_cast<char*>(&msg), sizeof(msg));
    out.close();

    std::ifstream in(filename, std::ios::binary);
    ASSERT_TRUE(in.is_open());

    Message readMsg = {};
    in.read(reinterpret_cast<char*>(&readMsg), sizeof(readMsg));
    in.close();

    EXPECT_STREQ(readMsg.text, "hello");

    remove(filename);
}

TEST(FIFOQueueTest, CircularBehavior) {
    SharedHeader header = {0, 0, 3};

    for (int i = 0; i < 6; ++i) {
        header.writeIndex = (header.writeIndex + 1) % header.maxMessages;
    }

    EXPECT_EQ(header.writeIndex, 0);
}