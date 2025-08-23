#pragma once

#define I2C_INLINE inline __attribute__((always_inline))
#define I2C_NOINLINE __attribute__((noinline))

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "tiny0/delay.hpp"
#include "tiny0/gpio.hpp"

// MCTRLA
//   [7]    RIEN: Read Interrupt Enable
//   [6]    WIEN: Write Interrupt Enable
//   [4]    QCEN: Quick Command Enable
//   [3:2]  TIMEOUT: 0=DISABLED, 1=50us, 2=100us, 3=200us
//   [1]    SMEN: Smart Mode Enable
//   [0]    ENABLE

// MCTRLB
//   [3]    FLUSH
//   [2]    ACKACT
//   [1:0]  MCMD: 0=NOACT, 1=REPSTART, 2=RECVTRANS, 3=STOP

namespace i2c {

enum Ports : uint8_t {
  SDA = 1,
  SCL = 2,
};

static constexpr uint8_t getBaud(uint32_t cpuFreq, uint32_t sclFreq,
                                 uint32_t tRise) {
  return (((float)cpuFreq / sclFreq) - 10 -
          ((float)cpuFreq * tRise / 1000000)) /
         2;
}

I2C_NOINLINE void busReset();
I2C_NOINLINE void init(uint8_t baud);
I2C_NOINLINE void writeArray(const uint8_t* data, uint8_t length);

static I2C_INLINE void waitIdle() {
  while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm)));
}

static I2C_INLINE void start(uint8_t devAddrRxW) {
  TWI0.MADDR = devAddrRxW;
  TWI0.MCTRLB |= TWI_MCMD_REPSTART_gc;
}

static I2C_INLINE void write(const uint8_t data) { writeArray(&data, 1); }

static I2C_INLINE void stop() {
  waitIdle();
  TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

static I2C_INLINE void disable() { TWI0.MCTRLA &= ~TWI_ENABLE_bm; }

static I2C_INLINE void enable() {
  gpio::setDirMulti((1 << Ports::SDA) | (1 << Ports::SCL), false);
  TWI0.MCTRLA |= TWI_ENABLE_bm;
  TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

#ifdef I2C_INCLUDE_IMPL

static void delay50us() { delayUs(50); }

I2C_NOINLINE void init(uint8_t baud) {
  TWI0.MBAUD = baud;
  TWI0.MCTRLA = TWI_ENABLE_bm | TWI_TIMEOUT_DISABLED_gc;
  TWI0.MCTRLB |= TWI_FLUSH_bm;
  TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
  TWI0.MSTATUS |= TWI_RIF_bm | TWI_WIF_bm;
}

I2C_NOINLINE void writeArray(const uint8_t* data, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    waitIdle();
    TWI0.MDATA = data[i];
    TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;
  }
}

I2C_NOINLINE void busReset() {
  constexpr uint8_t MAX_RETRIES = 3;
  constexpr uint8_t MAX_SCL_PULSES = 16;

  // change I2C pins to GPIO mode
  disable();
  gpio::setPullup(Ports::SDA, true);
  gpio::setPullup(Ports::SCL, true);
  gpio::setDirMulti((1 << Ports::SDA) | (1 << Ports::SCL), false);
  gpio::writeMulti((1 << Ports::SDA) | (1 << Ports::SCL), 0);

  for (uint8_t i = MAX_RETRIES; i != 0; i--) {
    // check SDA state
    if (gpio::read(Ports::SDA)) {
      // SDA is already high, no need to reset
      break;
    }

    // send SCL pulses until SDA is high
    gpio::setDir(Ports::SDA, false);
    gpio::setDir(Ports::SCL, true);
    delay50us();
    for (uint8_t j = MAX_SCL_PULSES; j != 0; j--) {
      gpio::setDir(Ports::SCL, false);
      delay50us();
      gpio::setDir(Ports::SCL, true);
      delay50us();
      if (gpio::read(Ports::SDA)) break;
    }

    // send stop condition
    gpio::setDir(Ports::SDA, true);
    delay50us();
    gpio::setDir(Ports::SCL, false);
    delay50us();
    gpio::setDir(Ports::SDA, false);
    delay50us();
  }

  enable();
}

#endif

}  // namespace i2c
