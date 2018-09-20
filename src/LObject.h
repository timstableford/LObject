#ifndef __L_OBJECT_H__
#define __L_OBJECT_H__

#include <DynamicBuffer.h>
#include <stdarg.h>

#define INDEX_TABLE_OFFSET (1) // Offset for the start of the types index table.
#define NUM_OBJ_OFFSET (0) // Offset for the object count.
#define STR_SIZE (0x01) // Number of bytes to use for string length.

/**
  * \brief A class to serialise and de-serialise objects into a buffer.
           When manually creating a class, set the number of items, then the
  item types,
           and then if there's strings set the items from first to last.
  */
class LObject {
   public:
    LObject(GenericBuffer<uint8_t> &buffer);
    LObject(uint8_t *data, uint16_t length);
    ~LObject();

    /**
     * \brief Gets the size of the object in bytes.
     */
    uint16_t getSize();

    /**
     * \brief Gets the type at the given index.
     * \param index The index to get the type at.
     * \return The type according to LObject::TYPES.
     */
    uint8_t getTypeAt(uint8_t index);
    /**
     * \brief Sets the type at the given index.
     * \param index The index to get the type at.
     * \param type The type according to LObject::TYPES.
     */
    uint8_t setTypeAt(uint8_t index, uint8_t type);

    /**
      * \brief Gets the data size at the given index.
               This could be either the type size or string length.
      */
    uint8_t getLengthAt(uint8_t index);
    /**
      * \brief Sets the lengths of strings or arrays at an index.
               This must be called before calling setStrAt.
      */
    uint8_t setLengthAt(uint8_t index, uint8_t length = 0);

    /**
      * \brief Sets a string or array at an index up to the given length.
               This could be either the type size or string length.
               Must call setLengthAt first.
      */
    uint16_t setStrAt(uint8_t index, char *str, uint16_t stringLen);
    /**
      * \brief Gets the string or array at a given index.
               Gets it up to string length, if null terminate is true then
               the final character is replaces by a null terminator.
      * \param nullTerminate This defaults to true and replaces the last
      character at
               stringLen with a null terminator.
      */
    uint16_t getStrAt(uint8_t index, char *str, uint16_t stringLen,
                      bool nullTerminate = false);

    /**
     * \brief Sets the number of items in the object.
     */
    uint8_t setItemCount(uint8_t itemCount);
    /**
     * \brief Gets the number of items in the object.
     */
    uint8_t getItemCount();

    /**
     * \brief Sets the data buffer used by this class.
     */
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

    /**
     * \brief An enum mapping supported types to their numeric values.
     */
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

    /**
      * \brief Assigns a slot big enough to construct the given object and
      builds it based on the arguments.
      * \param buffer The dynamic buffer to allocate the required memory in.
      * \param fmt The format string, each character representing a type.
                   s - string, a - array (args expects a uint8_t for the array
      length), c - int8, C - uint8,
                   d - int16, D - uint16, l - int32, L - uint32, m - int64, M -
      uint64, f - float.
      * \param ... Variable argument data.
      * \return The slot in the passed in buffer.
      */
    static int8_t make(DynamicBuffer<uint8_t> &buffer, const char *fmt, ...);
    /**
      * \brief Construct the given objectinto the provided buffer based on
      provided arguments.
      * \param buffer The generic buffer to build the object in.
      * \param fmt The format string, each character representing a type.
                   s - string, a - array (args expects a uint8_t for the array
      length), c - int8, C - uint8,
                   d - int16, D - uint16, l - int32, L - uint32, m - int64, M -
      uint64, f - float.
      * \param ... Variable argument data.
      * \return The size of the object in bytes.
      */
    static uint16_t make(GenericBuffer<uint8_t> &buffer, const char *fmt, ...);
    static uint16_t makeImpl(GenericBuffer<uint8_t> &buffer, const char *fmt,
                             va_list argp);

   private:
    static LObject::TYPES getType(char c);
    static uint8_t typeSize(uint8_t type);

    ArrayBufferWrapper<uint8_t> m_arrayBufferWrapper;
    GenericBuffer<uint8_t> &dataTable;
    uint16_t indexOf(uint8_t objIndex);
    uint8_t strNum(uint8_t index);
    uint16_t getDataOffset();
};

#endif
