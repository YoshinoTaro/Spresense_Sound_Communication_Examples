#ifndef ASYNCSERIALWRITER_HEADER_GUARD
#define ASYNCSERIALWRITER_HEADER_GUARD

#include "modulator.h"

#define START_BIT (1)
#define STOP_BIT  (0)
#define DATA_BIT_LENGTH (8)
#define PARITY_BIT (1)
#define BIT_ARRAY_LENGTH (DATA_BIT_LENGTH+PARITY_BIT+2)

#define TX_BUFFER_SIZE (128)

class AsyncSerialWriter {
  public:
    AsyncSerialWriter();
    ~AsyncSerialWriter();
    virtual bool begin(uint32_t baudrate);
    virtual void write(char c);
};

#endif // ASYNCSERIALWRITER_HEADER_GUARD
