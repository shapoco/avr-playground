#pragma once

#ifndef GPIO_ALWAYS_INLINE
#define GPIO_ALWAYS_INLINE inline __attribute__((always_inline))
#endif

#include <avr/io.h>
#include <stdint.h>

namespace gpio {

static GPIO_ALWAYS_INLINE void setDirMulti(uint8_t portMask, bool output) {
  if (output) {
    PORTA_DIRSET = portMask;
  } else {
    PORTA_DIRCLR = portMask;
  }
}

static GPIO_ALWAYS_INLINE void setDir(uint8_t port, bool output) {
  setDirMulti(1 << port, output);
}

static GPIO_ALWAYS_INLINE void writeMulti(uint8_t portMask, uint8_t value) {
  if (value) {
    PORTA_OUTSET = portMask;
  } else {
    PORTA_OUTCLR = portMask;
  }
}

static GPIO_ALWAYS_INLINE void write(uint8_t port, uint8_t value) {
  writeMulti(1 << port, value);
}

static GPIO_ALWAYS_INLINE void toggleMulti(uint8_t portMask) {
  PORTA_OUTTGL = portMask;
}

static GPIO_ALWAYS_INLINE void toggle(uint8_t port) { toggleMulti(1 << port); }

static GPIO_ALWAYS_INLINE uint8_t readMulti(uint8_t portMask) {
  return PORTA_IN & portMask;
}

static GPIO_ALWAYS_INLINE bool read(uint8_t port) {
  return readMulti(1 << port) != 0;
}

static GPIO_ALWAYS_INLINE void setPullup(uint8_t port, bool enable) {
  if (enable) {
    *(&PORTA_PIN0CTRL + port) |= PORT_PULLUPEN_bm;
  } else {
    *(&PORTA_PIN0CTRL + port) &= ~PORT_PULLUPEN_bm;
  }
}

}  // namespace gpio
