#include "AsyncSerialWriter.h"
AsyncSerialWriter theWriter;

//const char send_data[] = {'H','E','L','L','O','!','?','\n' };
//const char send_data[] = {'U','U','U','U','U','U','U','U'};
const char send_data[] = {'0','1','2','3','4','5','6','7','8','9','\n'};

const int send_data_size = sizeof(send_data);

static const int baudrate = 20;

void setup() {
  Serial.begin(115200);
  theWriter.begin(baudrate);
}

void loop() {
  static int n = 0;
  char c = send_data[n];
  theWriter.write(c);
  Serial.print(c, BIN);
  Serial.println(":" + String(c));
  ++n;
  if (n == send_data_size) n = 0;
  // to avoid FIFO overwrite
  delayMicroseconds(1000000/(baudrate/BIT_ARRAY_LENGTH));
  usleep(100*1000);
}
