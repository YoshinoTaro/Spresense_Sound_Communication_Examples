#include "AsyncSerialReader.h"
AsyncSerialReader theReader;

static const int baudrate = 20;

void setup() {
  Serial.begin(115200);
  theReader.begin(baudrate);
}

void loop() {
  if (theReader.available()) {
    char c = theReader.read();
    Serial.print(c, BIN);
    Serial.println(":" + String(c));
    //Serial.println(c);
  }
}