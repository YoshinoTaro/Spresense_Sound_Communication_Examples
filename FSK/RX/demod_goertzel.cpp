#include "demodulator.h"

static float s_coeff;
static float m_coeff;
static float s_freq;
static float m_freq;

void init_goertzel(uint16_t signal_length, uint32_t space_freq, uint32_t mark_freq) {
  UNUSED(signal_length);
  s_freq = space_freq;
  m_freq = mark_freq;
  const float s_normalized_freq = 2.0f*M_PI*s_freq / sampling_rate;
  const float m_normalized_freq = 2.0f*M_PI*m_freq / sampling_rate;
  s_coeff = 2.0f*arm_cos_f32(s_normalized_freq);  
  m_coeff = 2.0f*arm_cos_f32(m_normalized_freq);  
}

bool mark_detector_goertzel(float *pSrc, uint32_t signal_length) {
  // judged by Goertzel method
  float s_prev = 0, s_prev2 = 0, m_prev = 0, m_prev2 = 0;
  for (uint32_t i = 0; i < signal_length; ++i) {
    float s = pSrc[i] + s_coeff * s_prev - s_prev2;
    float m = pSrc[i] + m_coeff * m_prev - m_prev2;
    s_prev2 = s_prev; s_prev = s;
    m_prev2 = m_prev; m_prev = m;
  }
  float s_power = s_prev2 * s_prev2 + s_prev * s_prev - s_coeff * s_prev * s_prev2;
  float m_power = m_prev2 * m_prev2 + m_prev * m_prev - m_coeff * m_prev * m_prev2;
  return m_power > s_power;
}