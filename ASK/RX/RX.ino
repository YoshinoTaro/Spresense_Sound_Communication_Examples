#include "AsyncSerialReader.h"
AsyncSerialReader theReader;

const int baudrate = 2400;

void setup() {
  Serial.begin(115200);
  theReader.begin(baudrate);
}

void loop() {
  if (theReader.available()) {
    char c = theReader.read();
    Serial.print(c);
  }
  usleep(1);
}