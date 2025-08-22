#pragma once

#include <util/delay.h>

#define DELAY_INLINE inline __attribute__((always_inline))

static DELAY_INLINE void delayMs(uint16_t ms) { _delay_ms(ms); }
static DELAY_INLINE void delayUs(uint16_t us) { _delay_us(us); }
static DELAY_INLINE void delayVariableMs(uint16_t ms);

#ifdef DELAY_INCLUDE_IMPL

static DELAY_INLINE void delayVariableMs(uint16_t ms) {
  while (ms >= 256) {
    delayMs(256);
    ms -= 256;
  }
  while (ms >= 16) {
    delayMs(16);
    ms -= 16;
  }
  while (ms > 0) {
    delayMs(1);
    ms -= 1;
  }
}

#endif