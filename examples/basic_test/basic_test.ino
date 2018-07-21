#include <DynamicBuffer.h>
#include <LObject.h>

DynamicBuffer<uint8_t> buffer(32, 8);
uint8_t expectedBuilt[] = { 0x2, 0x6, 0x1, 0xc, 0x27, 0x95, 0x65, 0x1, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x77, 0x6f, 0x72, 0x6c, 0x64, 0x21 };

void setup() {
  Serial.begin(9600);
  int8_t result = LObject::make(buffer, "ls", 23434535, "hello world!");
  DynamicBuffer<uint8_t>::Buffer slotBuffer = buffer.getBuffer(result);
  for (unsigned int i = 0; i < sizeof(expectedBuilt); i++) {
    if(expectedBuilt[i] != slotBuffer[i]) {
      Serial.println(String("Byte mismatch at ") + i + ", expected: " + expectedBuilt[i] + ", got: " + slotBuffer[i]);
    }
  }
  Serial.println("Created: ");
  for (unsigned int i = 0; i < sizeof(expectedBuilt); i++) {
    Serial.print("0x");
    Serial.print(slotBuffer[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  Serial.println("Expected: ");
  for (unsigned int i = 0; i < sizeof(expectedBuilt); i++) {
    Serial.print("0x");
    Serial.print(expectedBuilt[i], HEX);
    Serial.print(", ");
  }
  Serial.println();
  buffer.free(result);
}

void loop() {

}
