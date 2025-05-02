#include "demodulator.h"
#define RX (21)

static callback my_cb = NULL;
static uint8_t  my_read_bit = 0x00;
static unsigned int my_duration_us = 0;
static unsigned int my_timer_us = 0;

// local
static unsigned int gpio_demod_read() {
  my_read_bit = !digitalRead(RX);
  digitalWrite(LED1, my_read_bit);
  my_cb(my_read_bit, my_timer_us);
  return my_timer_us;
}

// public
void demod_init() {
  pinMode(RX, INPUT_PULLUP);
}

void demod_set_bitrec(callback cb) {
  my_cb = cb;
}

void demod_set_interval(uint32_t duration_us) {
  if (duration_us == 0) fprintf(stderr, "duration_us is 0\n");
  my_timer_us = my_duration_us = duration_us;
}

bool demod_start() {
  if (my_cb == NULL) {
    fprintf(stderr, "Fatal Error: demod callback is not set\n");
    return false;
  }
  attachTimerInterrupt(gpio_demod_read, my_timer_us);
  return true;
}

void demod_stop() {
  my_timer_us = 0; // timer interrupt will be stopped
}

