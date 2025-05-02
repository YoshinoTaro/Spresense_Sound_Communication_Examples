#include "demodulator.h"

static arm_rfft_fast_instance_f32 S;
static uint16_t fft_length;
static float *pDst, *tmpBuf;
static uint32_t s_freq;
static uint32_t m_freq;
static float df;
static uint32_t fc;

void init_fft(uint16_t signal_length, uint32_t space_freq, uint32_t mark_freq) {
  int n = 0, m = signal_length;
  s_freq = space_freq; 
  m_freq = mark_freq;

  while (m > 2) { m = m/2; ++n; }
  fft_length = pow(2, n);
  printf("fft_length: %d\n", fft_length);
  arm_rfft_fast_init_f32(&S, fft_length); 

  df = AS_SAMPLINGRATE_192000/fft_length;
  fc = ((s_freq + m_freq)/2) / df;

  pDst = (float*)malloc(sizeof(float)*fft_length); 
  tmpBuf = (float*)malloc(sizeof(float)*fft_length); 
}

bool mark_detector_fft(float *pSrc, uint32_t signal_length) {
  UNUSED(signal_length);

  float maxValue; 
  uint32_t index;

  arm_rfft_fast_f32(&S, pSrc, tmpBuf, 0);
  arm_cmplx_mag_f32(tmpBuf, pDst, fft_length/2);
  arm_max_f32(pDst, fft_length/2, &maxValue, &index);

  return index > fc; 
}