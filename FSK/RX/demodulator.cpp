#include "demodulator.h"

#define ENABLE_FFT

static callback my_cb = NULL;
static uint8_t  my_read_bit = 0x00;
static unsigned int my_duration_us = 0;
static unsigned int my_timer_us = 0;
static uint8_t   mode  = 0;


AudioClass *theAudio = AudioClass::getInstance();

static int signal_length;
static uint32_t buffer_size; 
static char *s_buffer;
static float *pSrc;

static const int fetch_interval = 4;
static int fetch_timing = 1;

// local
static int sound_listener(int argc, char *argv[]) {

  // "wait_time" is not effective under 20msec that is NuttX tick time
  const uint32_t wait_time = 1000000UL*signal_length/sampling_rate/2; 
  printf("buffer_size %ld, signal_length: %d\n", buffer_size, signal_length);

  while (my_timer_us) {
    uint32_t read_size;
    err_t err = theAudio->readFrames(s_buffer, buffer_size, &read_size);
    if (err != AUDIOLIB_ECODE_OK && err != AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA) {
      fprintf(stderr, "Recording Error\n");
      theAudio->stopRecorder();
      break;
    } 
 
    if (read_size < buffer_size) { 
      usleep(wait_time); continue; 
    }

    ARRAY_CHAR_TO_FLOAT(s_buffer, pSrc, signal_length);

#ifndef ENABLE_FFT
    my_read_bit = mark_detector_goertzel(pSrc, signal_length);
#else // ENABLE_FFT
    my_read_bit = mark_detector_fft(pSrc, signal_length);
#endif // ENABLE_FFT

    static uint32_t last_time_us = 0;
    uint32_t current_time_us = micros();
    uint32_t duration_us = current_time_us - last_time_us;

    static int frame_counter  = 0;
    if (++frame_counter != fetch_timing) continue;
    fetch_timing += fetch_interval;

    DISPLAY_BIT(my_read_bit);

    last_time_us = current_time_us;
    mode = my_cb(my_read_bit, duration_us);
    if (mode == STARTBIT) { frame_counter = 0; fetch_timing = 1; }
  }
  return 0;
}


// public
void demod_init() {
  theAudio->begin();
  if (sampling_rate == AS_SAMPLINGRATE_48000) theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  else if (sampling_rate == AS_SAMPLINGRATE_192000) theAudio->setRenderingClockMode(AS_CLKMODE_HIRES);
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC);
  theAudio->initRecorder(AS_CODECTYPE_PCM, "/mnt/sd0/BIN", sampling_rate, AS_CHANNEL_MONO);
  theAudio->startRecorder();
  digitalWrite(LED0, HIGH);
}

void demod_set_bitrec(callback cb) {
  my_cb = cb;
}

void demod_set_interval(uint32_t duration_us) {
  if (duration_us == 0) fprintf(stderr, "duration_us is 0\n");

  int baudrate = 1000000/duration_us;
  // ovservation period is determined by fetch_interval
  signal_length = (sampling_rate / baudrate) / fetch_interval;

#ifdef ENABLE_FFT
  init_fft(signal_length, space_frequency, mark_frequency);
#else
  init_goertzel(signal_length, space_frequency, mark_frequency);
#endif // ENABLE_FFT

  buffer_size = signal_length * sizeof(int16_t);
  s_buffer = (char*)malloc(buffer_size*sizeof(char));
  pSrc = (float*)malloc(signal_length*sizeof(float));

  printf("duration %ld, baudrate: %d, buffer_size %ld, signal_length: %d\n", duration_us, baudrate, buffer_size, signal_length);

  my_timer_us = my_duration_us = duration_us;
}

bool demod_start() {
  if (my_cb == NULL) {
    fprintf(stderr, "Fatal Error: demod callback is not set\n");
    return false;
  }

  task_create("sound_listener", 120, 1024, sound_listener, NULL);
  return true;
}

void demod_stop() {
  my_timer_us = 0; // timer interrupt will be stopped
}
