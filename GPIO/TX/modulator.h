#ifndef MODULATOR_HEADER_GUARD__
#define MODULATOR_HEADER_GUARD__

#include <Arduino.h>
typedef bool (*callback)();

void mod_init();
void mod_set_bitwriter(callback cb);
void mod_set_interval(uint32_t duration_us);
bool mod_start();
void mod_stop();

#endif // MODULATOR_HEADER_GUARD__
