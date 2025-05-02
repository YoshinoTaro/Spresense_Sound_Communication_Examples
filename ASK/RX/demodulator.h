#ifndef DEMODULATOR_HEADER_GUARD__
#define DEMODULATOR_HEADER_GUARD__

#include <Arduino.h>
#define STARTBIT (0)
#define BITREC   (1)
#define PARITY   (2)
#define STOPBIT  (3)

typedef uint8_t (*callback)(uint8_t, uint32_t);

void demod_init();
void demod_set_bitrec(callback cb);
void demod_set_interval(uint32_t duration_us);
bool demod_start();
void demod_stop();

#endif // DEMODULATOR_HEADER_GUARD__
