// clang-format off
//                ______   
//       VDD 1 --| o    |-- 8 GND
//       PA6 2 --|      |-- 7 PA3
//       PA7 3 --|      |-- 6 PA0 (UPDI)
// (SDA) PA1 4 --|______|-- 5 PA2 (SCL)
// 
// # Name  Other/Special ADC0  AC0   YSART0  SPI0    TWI0  TCA0    TCB0  CCL
// 6 PA0   RESET/UDPI    AIN0  XDIR  SS_N                                LUT0-IN0
// 4 PA1                 AIN1        TxD(3)  MOSI    SDA   WO1           LUT0-IN1
// 5 PA2   EVOUT0        AIN2        RxD(3)  MISO    SCL   WO2           LUT0-IN2
// 7 PA3   EXTCLK        AIN3  OUT   XCK     SCK           WO0/WO3
// 8 GND
// 1 VDD
// 2 PA6                 AIN6  AINN0 TxD     MOSI(3)               WO0   LUT0-OUT
// 3 PA7                 AIN7  AINP0 RxD     MISO(3)       WO0(3)        LUT1-OUT
// clang-format on

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#define DELAY_INCLUDE_IMPL
#define I2C_INCLUDE_IMPL
#define AQM0802_INCLUDE_IMPL

#include "tiny0/aqm0802.hpp"
#include "tiny0/delay.hpp"
#include "tiny0/gpio.hpp"
#include "tiny0/i2c.hpp"

static constexpr uint8_t LED_PORT = 3;

aqm0802::Display display;

const char msg[] PROGMEM = "Hello, AQM0802!";

const uint8_t openEye[] PROGMEM = {
    0x00, 0x0E, 0x1F, 0x1F, 0x1F, 0x0E, 0x00, 0x00,
};

const uint8_t closedEyeLeft[] PROGMEM = {
    0x00, 0x18, 0x06, 0x01, 0x06, 0x18, 0x00, 0x00,
};

const uint8_t closedEyeRight[] PROGMEM = {
    0x00, 0x03, 0x0C, 0x10, 0x0C, 0x03, 0x00, 0x00,
};

const uint8_t closedMouth[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x0E,
};

const uint8_t openMouth[] PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x0E,
};

const uint8_t smallHeart[] PROGMEM = {
    0x00, 0x00, 0x00, 0x0A, 0x0E, 0x04, 0x00, 0x00,
};

const uint8_t largeHeart[] PROGMEM = {
    0x00, 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00,
};

int main() {
  // Clock = 32.768kHz (Prescaler Disabled)
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL_CLKSEL_OSCULP32K_gc);
  _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, ~CLKCTRL_PEN_bm);

  delayMs(1000);

  i2c::init(i2c::getBaud(F_CPU, 15000, 0));
  i2c::busReset();
  display.init();

  gpio::setDir(LED_PORT, true);

  // Initialize CGRAM
  display.setCgramAddress(0x08);
  display.writeProgmemArray(openEye, sizeof(openEye));          // \x01: Eye
  display.writeProgmemArray(closedMouth, sizeof(closedMouth));  // \x02: Mouth
  display.writeProgmemArray(openEye, sizeof(openEye));          // \x03: Eye
  display.writeProgmemArray(largeHeart, sizeof(largeHeart));    // \x04: Heart

  // Initialize Face
  display.setDdramAddress(0x01);
  display.writeString("(\x01\x02\x03)\x04");

  const uint8_t msgLen = sizeof(msg) - 1;  // Exclude null terminator
  int8_t msgPos = 8;

  uint8_t aniPos = 0;

  while (true) {
    // Message Scrolling
    display.setDdramAddress(0x40);
    for (uint8_t i = 0; i < 8; i++) {
      int8_t idx = -msgPos + i;
      if (0 <= idx && idx < msgLen) {
        display.writeChar(pgm_read_byte(&msg[idx]));
      } else {
        display.writeChar(' ');
      }
    }
    msgPos--;
    if (msgPos < -msgLen) {
      msgPos = 8;
    }

    // Face Animation
    if (aniPos < 4) {
      // Open/Close Eyes
      if ((aniPos & 1) == 0) {
        display.setCgramAddress(0x08);
        display.writeProgmemArray(openEye, sizeof(openEye));
        display.setCgramAddress(0x18);
        display.writeProgmemArray(openEye, sizeof(openEye));
      } else {
        display.setCgramAddress(0x08);
        display.writeProgmemArray(closedEyeLeft, sizeof(closedEyeLeft));
        display.setCgramAddress(0x18);
        display.writeProgmemArray(closedEyeRight, sizeof(closedEyeRight));
      }
    } else {
      // Open/Close Mouth
      display.setCgramAddress(0x10);
      if ((aniPos & 1) == 0) {
        display.writeProgmemArray(openMouth, sizeof(openMouth));
      } else {
        display.writeProgmemArray(closedMouth, sizeof(closedMouth));
      }
    }

    // Heart Animation
    display.setCgramAddress(0x20);
    if ((aniPos & 1) == 0) {
      display.writeProgmemArray(largeHeart, sizeof(largeHeart));
    } else {
      display.writeProgmemArray(smallHeart, sizeof(smallHeart));
    }

    aniPos++;
    if (aniPos >= 8) {
      aniPos = 0;
    }

    gpio::toggle(LED_PORT);
    delayMs(300);
  }
}
