#ifndef DEMODULATOR_HEADER_GUARD__
#define DEMODULATOR_HEADER_GUARD__

#include <Arduino.h>
#include <Audio.h>
/* Use CMSIS library */
#define ARM_MATH_CM4
#define __FPU_PRESENT 1U
#include <arm_math.h>

#define ARRAY_CHAR_TO_FLOAT(cptr_in, fptr_out, length) arm_q15_to_float((int16_t*)cptr_in, fptr_out, length)
#define DISPLAY_BIT(bit) digitalWrite(LED1, bit)

#define STARTBIT (0)
#define BITREC   (1)
#define PARITY   (2)
#define STOPBIT  (3)

const int sampling_rate = AS_SAMPLINGRATE_192000;
const int mark_frequency = 41000;
const int space_frequency = 38000;

typedef uint8_t (*callback)(uint8_t, uint32_t);

void demod_init();
void demod_set_bitrec(callback cb);
void demod_set_interval(uint32_t duration_us);
bool demod_start();
void demod_stop();


// fft methods
void init_fft(uint16_t signal_length, uint32_t space_freq, uint32_t mark_freq);
bool mark_detector_fft(float *pSrc, uint32_t signal_length);

// goertzel mothods
void init_goertzel(uint16_t signal_length, uint32_t space_freq, uint32_t mark_freq);
bool mark_detector_goertzel(float *pSrc, uint32_t signal_length);

#endif // DEMODULATOR_HEADER_GUARD__
