#ifdef __LINUX_BUILD

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <functional>

#include <gtest/gtest.h>

#include <DynamicBuffer.h>
#include <LObject.h>

class LTest : public ::testing::Test {
public:
    LTest() : m_buffer(m_rawBuffer, 512) {}

    void SetUp() override {
        memset(m_rawBuffer, 0, 512);
    }

    void compare(uint8_t *expected, uint16_t length) {
        ASSERT_EQ(0, memcmp(expected, m_rawBuffer, length));
    }

    uint8_t m_rawBuffer[512];
    ArrayBufferWrapper<uint8_t> m_buffer;
};

#define TEST_GET_SET(suffix, ident, code, type, val) \
TEST_F(LTest, TestGetSet_##type##_##suffix) { \
    ASSERT_GT(LObject::make(m_buffer, ident, (val)), 0); \
    LObject obj(m_buffer); \
    ASSERT_EQ((code), obj.getTypeAt(0)); \
    ASSERT_EQ((val), obj.type##At(0)); \
}

TEST_GET_SET(max, "c", LObject::TYPES::T_INT8, int8, 127)
TEST_GET_SET(min, "c", LObject::TYPES::T_INT8, int8, -128)
TEST_GET_SET(max, "C", LObject::TYPES::T_UINT8, uint8, 255)
TEST_GET_SET(min, "C", LObject::TYPES::T_UINT8, uint8, 0)

TEST_GET_SET(max, "d", LObject::TYPES::T_INT16, int16, 32767)
TEST_GET_SET(min, "d", LObject::TYPES::T_INT16, int16, -32768)
TEST_GET_SET(max, "D", LObject::TYPES::T_UINT16, uint16, 65535)
TEST_GET_SET(min, "D", LObject::TYPES::T_UINT16, uint16, 0)

TEST_GET_SET(max, "l", LObject::TYPES::T_INT32, int32, 2147483647)
TEST_GET_SET(min, "l", LObject::TYPES::T_INT32, int32, -2147483648)
TEST_GET_SET(max, "L", LObject::TYPES::T_UINT32, uint32, 4294967295)
TEST_GET_SET(min, "L", LObject::TYPES::T_UINT32, uint32, 0)

TEST_GET_SET(max, "m", LObject::TYPES::T_INT64, int64, 922337204775807ll)
TEST_GET_SET(min, "m", LObject::TYPES::T_INT64, int64, -922337204775808ll)
TEST_GET_SET(max, "M", LObject::TYPES::T_UINT64, uint64, 18446744073709551615ull)
TEST_GET_SET(min, "M", LObject::TYPES::T_UINT64, uint64, 0ull)

TEST_GET_SET(val, "f", LObject::TYPES::T_FLOAT, float, 0.123456789f)

TEST_F(LTest, TestBasicLifecycle) {
    char testStr[] = "hello world!";
    uint8_t expectedBuilt[] = {0x2,  0x6,  0x1,  0xc,  0x1,  0x65, 0x95,
                               0x27, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                               0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21};

    ASSERT_EQ(sizeof(expectedBuilt), LObject::make(m_buffer, "ls", 23434535, testStr));
    compare(expectedBuilt, sizeof(expectedBuilt));

    LObject obj(m_buffer);
    ASSERT_EQ(2, obj.getItemCount());
    ASSERT_EQ(23434535, obj.int32At(0));
    ASSERT_EQ(12, obj.getLengthAt(1));

    char strBuf[sizeof(testStr) + 1];
    obj.getStrAt(1, strBuf, obj.getLengthAt(1) + 1, true);

    ASSERT_EQ(strlen(testStr), obj.getLengthAt(1));
    ASSERT_EQ(0, memcmp(testStr, strBuf, obj.getLengthAt(1) + 1));
}

TEST_F(LTest, TestMakeFromBufferSlot) {
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

TEST_F(LTest, TestFromArray) {
    uint8_t testData[] = {0x2, 0x6,  0x1,  0xc,  0x1,  0x65, 0x95,
                               0x27, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                               0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21};
    LObject obj(testData, sizeof(testData) / sizeof(testData[0]));
    ASSERT_EQ(2, obj.getItemCount());
    ASSERT_EQ(23434535, obj.int32At(0));
    ASSERT_EQ(12, obj.getLengthAt(1));
}

TEST_F(LTest, TestFloat) {
    uint16_t size = LObject::make(m_buffer, "f", 0.123456f);
    ASSERT_EQ(6, size);

    LObject obj(m_buffer);
    ASSERT_EQ(0.123456f, obj.floatAt(0));
}

TEST_F(LTest, TestManualArray) {
    LObject obj(m_buffer);
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
    ASSERT_EQ(24, obj.getStrAt(0, (char *)fetchedData, 24, false));
    ASSERT_EQ(0, memcmp(data, fetchedData, 24));
}

#endif  // __LINUX_BUILD
