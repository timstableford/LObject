#ifdef __LINUX_BUILD

#include <cstdio>
#include <cstring>
#include <functional>

#include <gtest/gtest.h>

#include <DynamicBuffer.h>
#include <LObject.h>

TEST(IndependentMethod, TestBasicLifecycle) {
    char testStr[] = "hello world!";
    ArrayBufferWrapper<uint8_t> wrapper1(nullptr, 0);
    uint16_t size = LObject::make(wrapper1, "ls", 23434535, testStr);
    uint8_t buffer[512];
    ArrayBufferWrapper<uint8_t> wrapper2(buffer, size);
    LObject::make(wrapper2, "ls", 23434535, "hello world!");

    uint8_t expectedBuilt[] = {0x2,  0x6,  0x1,  0xc,  0x1,  0x65, 0x95,
                               0x27, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                               0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21};
    ASSERT_EQ(sizeof(expectedBuilt), size);
    for (int i = 0; i < size; i++) {
        EXPECT_EQ(expectedBuilt[i], buffer[i]);
    }

    LObject obj(wrapper2);
    ASSERT_EQ(2, obj.getItemCount());
    ASSERT_EQ(23434535, obj.int32At(0));
    ASSERT_EQ(12, obj.getLengthAt(1));

    char strBuf[sizeof(testStr) + 1];
    obj.getStrAt(1, strBuf, obj.getLengthAt(1) + 1, true);
    ASSERT_EQ(strlen(testStr), obj.getLengthAt(1));
    for (int i = 0; i < obj.getLengthAt(1) + 1; i++) {
        EXPECT_EQ(testStr[i], strBuf[i]);
    }
}

TEST(IndependentMethod, TestMakeFromBuffer) {
    DynamicBuffer<uint8_t> buffer(32, 8);
    int8_t result = LObject::make(buffer, "ls", 23434535, "hello world!");
    ASSERT_NE(SLOT_FREE, result);

    DynamicBuffer<uint8_t>::Buffer slotBuffer = buffer.getBuffer(result);
    uint8_t expectedBuilt[] = {0x2,  0x6,  0x1,  0xc,  0x1,  0x65, 0x95,
                               0x27, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                               0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21};
    // The assigned slot should at least be big enough.
    ASSERT_GE(slotBuffer.size(), sizeof(expectedBuilt));
    for (unsigned int i = 0; i < sizeof(expectedBuilt); i++) {
        EXPECT_EQ(expectedBuilt[i], slotBuffer[i]);
    }
}

TEST(IndependentMethod, TestFloat) {
    DynamicBuffer<uint8_t> buffer(32, 8);
    int8_t result = LObject::make(buffer, "f", 0.123456f);
    ASSERT_NE(SLOT_FREE, result);

    DynamicBuffer<uint8_t>::Buffer slotBuffer = buffer.getBuffer(result);

    LObject obj(slotBuffer);
    ASSERT_EQ(0.123456f, obj.floatAt(0));
}

TEST(IndependentMethod, TestManualArray) {
    DynamicBuffer<uint8_t> buffer(16, 8);
    int8_t slot = buffer.allocate(128);
    DynamicBuffer<uint8_t>::Buffer slotBuffer = buffer.getBuffer(slot);
    LObject obj(slotBuffer);
    obj.setItemCount(1);
    obj.setTypeAt(0, LObject::T_ARRAY);
    obj.setLengthAt(0, 24);
    ASSERT_EQ(1, obj.getItemCount());
    ASSERT_EQ(LObject::T_ARRAY, obj.getTypeAt(0));
    ASSERT_EQ(24, obj.getLengthAt(0));
    uint8_t data[24];
    EXPECT_EQ(24, obj.setStrAt(0, (char *)data, 24));
    uint8_t fetchedData[24];
    // Don't add a null terminator.
    EXPECT_EQ(24, obj.getStrAt(0, (char *)fetchedData, 24, false));
    for (unsigned int i = 0; i < 24; i++) {
        EXPECT_EQ(data[i], fetchedData[i]);
    }
}

#endif  // __LINUX_BUILD
