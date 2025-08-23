#pragma once

#include <stdint.h>

#define DELAY_INLINE inline __attribute__((always_inline))

static DELAY_INLINE void delayMs(uint16_t ms) {
  extern void __builtin_avr_delay_cycles(uint32_t);
  const uint32_t ticks = ((uint64_t)F_CPU * ms + 999) / 1000;
  __builtin_avr_delay_cycles(ticks);
}

static DELAY_INLINE void delayUs(uint16_t us) {
  extern void __builtin_avr_delay_cycles(uint32_t);
  const uint32_t ticks = ((uint64_t)F_CPU * us + 999999) / 1000000;
  __builtin_avr_delay_cycles(ticks);
}
