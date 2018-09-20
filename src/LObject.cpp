#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "LObject.h"
#include "NetworkUtil.h"

#define INT_GETSET(x, y, ct, cf)                                     \
    union x##_u {                                                    \
        x data;                                                      \
        uint8_t arr[sizeof(x)];                                      \
    };                                                               \
    x LObject::y(uint8_t index) {                                    \
        if (this->indexOf(index) == 0) {                             \
            return false;                                            \
        }                                                            \
        x##_u data;                                                  \
        for (uint8_t i = 0; i < sizeof(x); i++) {                    \
            data.arr[i] = this->dataTable[this->indexOf(index) + i]; \
        }                                                            \
        return cf(data.data);                                        \
    }                                                                \
    bool LObject::y(uint8_t index, x value) {                        \
        if (this->indexOf(index) == 0) {                             \
            return false;                                            \
        }                                                            \
        x##_u data;                                                  \
        data.data = ct(value);                                       \
        for (uint8_t i = 0; i < sizeof(x); i++) {                    \
            this->dataTable[this->indexOf(index) + i] = data.arr[i]; \
        }                                                            \
        return true;                                                 \
    }

#define SIMPLE_FUNC(x) x

INT_GETSET(int8_t, int8At, SIMPLE_FUNC, SIMPLE_FUNC)
INT_GETSET(int16_t, int16At, htons, ntohs)
INT_GETSET(int32_t, int32At, htonl, ntohl)
INT_GETSET(int64_t, int64At, htonll, ntohll)
INT_GETSET(uint8_t, uint8At, SIMPLE_FUNC, SIMPLE_FUNC)
INT_GETSET(uint16_t, uint16At, htons, ntohs)
INT_GETSET(uint32_t, uint32At, htonl, ntohl)
INT_GETSET(uint64_t, uint64At, htonll, ntohll)
INT_GETSET(float, floatAt, SIMPLE_FUNC, SIMPLE_FUNC)

typedef struct {
    uint8_t type;
    uint8_t size;
} ObjectType;

static ObjectType typesArray[] = {
    {LObject::T_STRING, STR_SIZE},        // string - number of bytes to use for
                                          // strlen
    {LObject::T_INT8, sizeof(int8_t)},    // int8_t
    {LObject::T_UINT8, sizeof(uint8_t)},  // uint8_t
    {LObject::T_INT16, sizeof(int16_t)},  // int16_t
    {LObject::T_UINT16, sizeof(uint16_t)},  // uint16_t
    {LObject::T_INT32, sizeof(int32_t)},    // int32_t
    {LObject::T_UINT32, sizeof(uint32_t)},  // uint32_t
    {LObject::T_INT64, sizeof(int64_t)},    // int64_t
    {LObject::T_UINT64, sizeof(uint64_t)},  // uint64_t
    {LObject::T_FLOAT, sizeof(float)}       // float
};

LObject::LObject(GenericBuffer<uint8_t> &buffer) : dataTable(buffer) {
    // Nothing to do.
}

LObject::~LObject() {
    // Nothing to do.
}

uint8_t LObject::getLengthAt(uint8_t index) {
    if (index >= this->getItemCount()) {
        return 0;
    }
    if (getTypeAt(index) == T_STRING || getTypeAt(index) == T_ARRAY) {
        return this->dataTable[this->getItemCount() + this->strNum(index) +
                               INDEX_TABLE_OFFSET];
    } else {
        return typeSize(getTypeAt(index));
    }
}

uint8_t LObject::setLengthAt(uint8_t index, uint8_t length) {
    if (getTypeAt(index) != T_STRING && getTypeAt(index) != T_ARRAY) {
        return 0;
    }
    this->dataTable[this->getItemCount() + this->strNum(index) +
                    INDEX_TABLE_OFFSET] = length;
    return length;
}

uint8_t LObject::getItemCount() {
    if (this->dataTable.size() == 0) {
        return 0;
    }
    return this->dataTable[NUM_OBJ_OFFSET];
}

uint8_t LObject::setItemCount(uint8_t itemCount) {
    if (this->dataTable.size() == 0) {
        return 0;
    }
    this->dataTable[NUM_OBJ_OFFSET] = itemCount;
    return itemCount;
}

uint8_t LObject::setTypeAt(uint8_t offset, uint8_t type) {
    if (this->dataTable.size() == 0) {
        return 0;
    }
    this->dataTable[INDEX_TABLE_OFFSET + offset] = type;
    return type;
}

uint8_t LObject::getTypeAt(uint8_t index) {
    if (this->getItemCount() <= 0 || index >= this->getItemCount()) {
        return T_NONE;
    }
    return this->dataTable[index + INDEX_TABLE_OFFSET];
}

uint16_t LObject::setStrAt(uint8_t index, char *str, uint16_t stringLen) {
    if (stringLen == 0) {
        return 0;
    }
    if (str == NULL) {
        return 0;
    }

    uint16_t maxLength = this->getLengthAt(index);
    for (uint16_t i = 0; i < stringLen && i < maxLength; i++) {
        this->dataTable[this->indexOf(index) + i] =
            static_cast<uint8_t>(str[i]);
    }

    return this->getLengthAt(index);
}

uint16_t LObject::getStrAt(uint8_t index, char *str, uint16_t stringLen,
                           bool nullTerminate) {
    if (stringLen == 0) {
        return 0;
    }
    if (str == NULL) {
        return this->getLengthAt(index) + nullTerminate
                   ? 1
                   : 0;  // For null terminator when querying buffer size.
    }
    uint16_t copyCount = this->getLengthAt(index);
    if (nullTerminate) {
        if (copyCount > stringLen - 1) {
            copyCount = stringLen - 1;
        }
    }

    uint16_t stringStart = this->indexOf(index);
    for (uint16_t i = 0; i < copyCount; i++) {
        str[i] = static_cast<char>(this->dataTable[stringStart + i]);
    }
    if (nullTerminate) {
        str[copyCount] = '\0';
    }

    return copyCount;
}

void LObject::setDataBuffer(GenericBuffer<uint8_t> &buffer) {
    this->dataTable = buffer;
}

uint16_t LObject::getSize() { return this->indexOf(this->getItemCount()); }

int8_t LObject::make(DynamicBuffer<uint8_t> &buffer, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    ArrayBufferWrapper<uint8_t> emptyBuffer(nullptr, 0);
    uint16_t size = LObject::makeImpl(emptyBuffer, fmt, argp);
    va_end(argp);
    int8_t slot = buffer.allocate(size);
    if (slot == SLOT_FREE) {
        return SLOT_FREE;
    }
    DynamicBuffer<uint8_t>::Buffer slotBuffer = buffer.getBuffer(slot);
    va_start(argp, fmt);

    uint16_t objSize = LObject::makeImpl(slotBuffer, fmt, argp);
    if (size != objSize) {
        va_end(argp);
        buffer.free(slot);
        return SLOT_FREE;
    }
    va_end(argp);
    return slot;
}

uint16_t LObject::make(GenericBuffer<uint8_t> &buffer, const char *fmt, ...) {
    va_list argp;
    va_start(argp, fmt);
    uint16_t return_value = LObject::makeImpl(buffer, fmt, argp);
    va_end(argp);
    return return_value;
}

uint16_t LObject::makeImpl(GenericBuffer<uint8_t> &buffer, const char *fmt,
                           va_list argp) {
    uint16_t numArgs = strlen(fmt);

    // Put the buffer and index table into and object
    LObject o(buffer);
    o.setItemCount(numArgs);

    uint16_t length = INDEX_TABLE_OFFSET + numArgs;
    va_list argpCopy;
    va_copy(argpCopy, argp);
    for (uint16_t i = 0; i < numArgs; i++) {
        uint8_t type = LObject::getType(fmt[i]);
        o.setTypeAt(i, type);
        switch (type) {
            case T_ARRAY: {
                uint8_t arrLen = (uint8_t)va_arg(argpCopy, int);
                o.setLengthAt(i, arrLen);
                length += arrLen + 1;
                break;
            }
            case T_STRING: {
                uint8_t charLen = (uint8_t)strlen(va_arg(argpCopy, char *));
                o.setLengthAt(i, charLen);
                length += charLen + 1;
                break;
            }
            case T_INT8:
            case T_UINT8:
            case T_INT16:
            case T_UINT16:
                va_arg(argpCopy, int);
                length += typeSize(type);
                break;
            case T_INT32:
            case T_UINT32:
                va_arg(argpCopy, long);
                length += typeSize(type);
                break;
            case T_INT64:
            case T_UINT64:
                va_arg(argpCopy, long long);
                length += typeSize(type);
                break;
            case T_FLOAT:
                va_arg(argpCopy, double);
                length += typeSize(type);
                break;
        }
    }
    va_end(argpCopy);

    if (buffer.size() == 0) {
        return length;
    }

    // For each item in the object, assign it's data from the temporary buffer
    for (uint8_t i = 0; i < o.getItemCount(); i++) {
        switch (o.getTypeAt(i)) {
            case LObject::T_STRING: {
                char *str = va_arg(argp, char *);
                o.setStrAt(i, str, (uint8_t)(strlen(str)));
                break;
            }
            case LObject::T_ARRAY:
                va_arg(argp,
                       int);  // Skip copying this size but move the buffer.
                break;
            case LObject::T_INT8:
                o.int8At(i, (int8_t)va_arg(argp, int));
                break;
            case LObject::T_UINT8:
                o.uint8At(i, (uint8_t)va_arg(argp, int));
                break;
            case LObject::T_INT16:
                o.int16At(i, (int16_t)va_arg(argp, int));
                break;
            case LObject::T_UINT16:
                o.uint16At(i, (uint16_t)va_arg(argp, int));
                break;
            case LObject::T_INT32:
                o.int32At(i, (int32_t)va_arg(argp, long));
                break;
            case LObject::T_UINT32:
                o.uint32At(i, (uint32_t)va_arg(argp, long));
                break;
            case LObject::T_INT64:
                o.int64At(i, (int64_t)va_arg(argp, long long));
                break;
            case LObject::T_UINT64:
                o.uint64At(i, (uint64_t)va_arg(argp, long long));
                break;
            case LObject::T_FLOAT:
                o.floatAt(i, (float)va_arg(argp, double));
                break;
            case LObject::T_NONE:
                return 0;
        }
    }

    return o.getSize();
}

// Private methods.

LObject::TYPES LObject::getType(char c) {
    switch (c) {
        case 's':
            return LObject::T_STRING;
        case 'a':
            return LObject::T_ARRAY;
        case 'c':
            return LObject::T_INT8;
        case 'C':
            return LObject::T_UINT8;
        case 'd':
            return LObject::T_INT16;
        case 'D':
            return LObject::T_UINT16;
        case 'l':
            return LObject::T_INT32;
        case 'L':
            return LObject::T_UINT32;
        case 'm':
            return LObject::T_INT64;
        case 'M':
            return LObject::T_UINT64;
        case 'f':
            return LObject::T_FLOAT;
        default:
            return LObject::T_NONE;
    }
}

uint8_t LObject::typeSize(uint8_t type) {
    for (uint8_t i = 0; i < NUM_TYPES; i++) {
        if (typesArray[i].type == type) {
            return typesArray[i].size;
        }
    }

    return 0;
}

uint16_t LObject::getDataOffset() {
    uint8_t stringCount = this->strNum(this->getItemCount());
    return this->getItemCount() + (stringCount * this->typeSize(T_STRING)) +
           NUM_ITEMS_OFFSET;
}

uint8_t LObject::strNum(uint8_t index) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < index && i < this->getItemCount(); i++) {
        if (this->dataTable[i + INDEX_TABLE_OFFSET] == T_STRING ||
            this->dataTable[i + INDEX_TABLE_OFFSET] == T_ARRAY) {
            count++;
        }
    }

    return count;
}

uint16_t LObject::indexOf(uint8_t index) {
    uint16_t dataIndex = this->getDataOffset();

    if (index > this->getItemCount()) {
        return 0;
    }

    for (uint16_t i = 0; i < index; i++) {
        dataIndex += this->getLengthAt(i);
    }

    return dataIndex;
}
