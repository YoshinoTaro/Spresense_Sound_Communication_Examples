#ifndef ASYNCSERIALREAD_HEADER_GUARD
#define ASYNCSERIALREAD_HEADER_GUARD

#include "demodulator.h"

#define DATA_BIT_LENGTH (8)
#define RX_BUFFER_SIZE (128)

const uint8_t wait_startbit = STARTBIT;
const uint8_t bit_recording = BITREC;
const uint8_t parity_check  = PARITY;
const uint8_t wait_stopbit  = STOPBIT;

class AsyncSerialReader {
  public:
    AsyncSerialReader();
    ~AsyncSerialReader();
    virtual bool begin(uint32_t baudrate);
    virtual char read();
    virtual bool available();
};


#endif // ASYNCSERIALREAD_HEADER_GUARD
