#ifndef __L_OBJECT_H__
#define __L_OBJECT_H__

#include <stdarg.h>
#include <DynamicBuffer.h>

#define NUM_TYPES 10
#define NUM_ITEMS_OFFSET 1
#define INDEX_TABLE_OFFSET 1
#define NUM_OBJ_OFFSET 0
#define STR_SIZE 0x01

class LObject {
	public:
        LObject(GenericBuffer<uint8_t> &buffer);
		~LObject();

		uint16_t getSize();

		uint8_t getTypeAt(uint8_t index);
        uint8_t setTypeAt(uint8_t index, uint8_t type);

        uint8_t getLengthAt(uint8_t index);
        uint8_t setLengthAt(uint8_t index, uint8_t length = 0);

		uint16_t setStrAt(uint8_t index, char *str, uint16_t stringLen);
		uint16_t getStrAt(uint8_t index, char *str, uint16_t stringLen, bool nullTerminate = true);

		uint8_t setItemCount(uint8_t itemCount);
        uint8_t getItemCount();

        void setDataBuffer(GenericBuffer<uint8_t> &buffer);

		int8_t int8At(uint8_t index);
		bool int8At(uint8_t index, int8_t data);
		int16_t int16At(uint8_t index);
		bool int16At(uint8_t index, int16_t data);
		int32_t int32At(uint8_t index);
		bool int32At(uint8_t index, int32_t data);
		int64_t int64At(uint8_t index);
		bool int64At(uint8_t index, int64_t data);

		uint8_t uint8At(uint8_t index);
		bool uint8At(uint8_t index, uint8_t data);
		uint16_t uint16At(uint8_t index);
		bool uint16At(uint8_t index, uint16_t data);
		uint32_t uint32At(uint8_t index);
		bool uint32At(uint8_t index, uint32_t data);
		uint64_t uint64At(uint8_t index);
		bool uint64At(uint8_t index, uint64_t data);

		float floatAt(uint8_t index);
		bool floatAt(uint8_t index, float data);

		enum TYPES {
			T_NONE = 0x00,
			T_STRING = 0x01,
			T_INT8 = 0x02,
			T_UINT8 = 0x03,
			T_INT16 = 0x04,
			T_UINT16 = 0x05,
			T_INT32 = 0x06,
			T_UINT32 = 0x07,
			T_INT64 = 0x08,
			T_UINT64 = 0x09,
			T_FLOAT = 0x0C,
			T_ARRAY = 0x0D
		};

        static int8_t make(DynamicBuffer<uint8_t> &buffer, const char *fmt, ...);
		static uint16_t make(GenericBuffer<uint8_t> &buffer, const char *fmt, ...);
		static uint16_t makeImpl(GenericBuffer<uint8_t> &buffer, const char *fmt, va_list argp);

	private:
        static LObject::TYPES getType(char c);
        static uint8_t typeSize(uint8_t type);

		GenericBuffer<uint8_t> &dataTable;
		uint16_t indexOf(uint8_t objIndex);
		uint8_t strNum(uint8_t index);
		uint16_t getDataOffset();
};

#endif
