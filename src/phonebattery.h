#ifndef __TIMEBOXED_PHONEBATTERY_
#define __TIMEBOXED_PHONEBATTERY_

#if !defined PBL_PLATFORM_APLITE
#include <pebble.h>

bool is_phonebattery_enabled();
void update_phonebattery_value(int, int);
void toggle_phonebattery(uint8_t);
void update_phonebattery(bool);
#endif
#endif
