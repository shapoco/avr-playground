#pragma once

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

void init(uint8_t baud);
void start(uint8_t devAddrRxW);
void write(const uint8_t data);
void writeArray(const uint8_t* data, uint8_t length);
void stop();
void waitIdle();
void disable();
void enable();
void busReset();

void init(uint8_t baud) {
  TWI0.MBAUD = baud;
  TWI0.MCTRLA = TWI_ENABLE_bm | TWI_TIMEOUT_DISABLED_gc;
  TWI0.MCTRLB |= TWI_FLUSH_bm;
  TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
  TWI0.MSTATUS |= TWI_RIF_bm | TWI_WIF_bm;
}

void start(uint8_t devAddrRxW) {
  TWI0.MADDR = devAddrRxW;
  TWI0.MCTRLB |= TWI_MCMD_REPSTART_gc;
}

void write(const uint8_t data) {
  waitIdle();
  TWI0.MDATA = data;
  TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;
}

void writeArray(const uint8_t* data, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    write(data[i]);
  }
}

void stop() {
  waitIdle();
  TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
}

void waitIdle() { while (!(TWI0.MSTATUS & (TWI_WIF_bm | TWI_RIF_bm))); }

void disable() { TWI0.MCTRLA &= ~TWI_ENABLE_bm; }

void enable() {
  gpio::setDirMulti((1 << Ports::SDA) | (1 << Ports::SCL), false);
  TWI0.MCTRLA |= TWI_ENABLE_bm;
  TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
}

void busReset() {
  constexpr uint8_t MAX_RETRIES = 3;
  constexpr uint8_t MAX_SCL_PULSES = 16;
  constexpr uint8_t SCL_PERIOD_US = 100;

  // change I2C pins to GPIO mode
  disable();
  gpio::setPullup(Ports::SDA, true);
  gpio::setPullup(Ports::SCL, true);
  gpio::setDirMulti((1 << Ports::SDA) | (1 << Ports::SCL), false);
  gpio::writeMulti((1 << Ports::SDA) | (1 << Ports::SCL), 0);

  for (int i = 0; i < MAX_RETRIES; i++) {
    // check SDA state
    if (gpio::read(Ports::SDA)) {
      // SDA is high, no need to reset
      break;
    }

    // send SCL pulses until SDA is high
    gpio::setDir(Ports::SDA, false);
    gpio::setDir(Ports::SCL, true);
    delayUs(SCL_PERIOD_US);
    for (int j = 0; j < MAX_SCL_PULSES; j++) {
      gpio::setDir(Ports::SCL, false);
      delayUs(SCL_PERIOD_US / 2);
      gpio::setDir(Ports::SCL, true);
      delayUs(SCL_PERIOD_US / 2);
      if (gpio::read(Ports::SDA)) break;
    }

    // send stop condition
    gpio::setDir(Ports::SDA, true);
    delayUs(SCL_PERIOD_US);
    gpio::setDir(Ports::SCL, false);
    delayUs(SCL_PERIOD_US);
    gpio::setDir(Ports::SDA, false);
    delayUs(SCL_PERIOD_US);
  }

  enable();
}
}  // namespace i2c
