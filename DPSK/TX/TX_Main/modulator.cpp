#ifdef SUBCORE
#error "Core selection is wrong"
#endif

#include "modulator.h"
#include <MP.h>

static callback my_cb = NULL;
static unsigned int my_duration_us = 0;
static unsigned int my_timer_us = 0;

static const int pwmcore = 1;

// local
static unsigned int score_write() {
  bool bit = my_cb();
  if (bit) {
    const int8_t sndid = 100;
    MP.Send(sndid, bit, pwmcore);
  }
  digitalWrite(LED0, bit);
  return my_timer_us;
}


void mod_init() {
  MP.begin(pwmcore);
}

void mod_set_bitwriter(callback cb) {
  my_cb = cb;
}

void mod_set_interval(uint32_t duration_us) {
  if (duration_us == 0) fprintf(stderr, "duration_us is 0\n");
  my_timer_us = my_duration_us = duration_us;
}

bool mod_start() {
  if (my_cb == NULL) {
    fprintf(stderr, "Fatal Error: mod callback is not set\n");
    return false;
  }  
  attachTimerInterrupt(score_write, my_duration_us);
  return true;
}

void mod_stop() {
  my_timer_us = 0; // timer interrupt will be stopped
}
