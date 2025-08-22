
#include <stdint.h>

#include <avr/pgmspace.h>

#include "tiny0/delay.hpp"
#include "tiny0/i2c.hpp"

namespace aqm0802 {

enum Command : uint8_t {
  CLEAR = 0x01,
  HOME = 0x02,
  ENTRY_MODE_SET = 0x04,
  DISPLAY_CONTROL = 0x08,
  CURSOR_SHIFT = 0x10,
  FUNCTION_SET = 0x20,
  SET_CGRAM_ADDR = 0x40,
  SET_DDRAM_ADDR = 0x80
};

extern const uint8_t INIT_SEQUENCE[];

class Display {
 public:
  static constexpr int CGRAM_DEPTH = 6;
  const uint8_t devAddr;

  Display(uint8_t devaddr = (0x3E << 1));
  void writeCommand(uint8_t cmd);
  void init();
  void setDdramAddress(uint8_t offset);
  void setCgramAddress(uint8_t offset);
  void writeChar(uint8_t c);
  void writeArray(const uint8_t *data, uint8_t length);
  void writeString(const char *str);
  void writeProgmemArray(const uint8_t *data, uint8_t length);
  void writeProgmemString(const char *str);
};

#ifdef AQM0802_INCLUDE_IMPL

const uint8_t INIT_SEQUENCE[] PROGMEM = {
    // clang-format off
    1, 50,
    0, 0x38,
    0, 0x39,
    0, 0x14,
    0, 0x70,
    0, 0x56,
    0, 0x6C,
    1, 200,
    0, 0x38,
    0, 0x0C,
    0, 0x01,
    // clang-format on
};

Display::Display(uint8_t devaddr) : devAddr(devaddr) {}

void Display::writeCommand(uint8_t cmd) {
  uint8_t data[2] = {0x00, cmd};
  i2c::start(devAddr);
  i2c::writeArray(data, sizeof(data));
  i2c::stop();
}

void Display::init() {
  for (uint8_t i = 0; i < sizeof(INIT_SEQUENCE); i += 2) {
    uint8_t byte1 = pgm_read_byte(&INIT_SEQUENCE[i]);
    uint8_t byte2 = pgm_read_byte(&INIT_SEQUENCE[i + 1]);
    if (byte1 == 0) {
      // Command
      writeCommand(byte2);
      delayUs(50);
    } else {
      // Delay in ms
      for (uint8_t t = byte2; t > 0; t--) {
        delayMs(1);
      }
    }
  }
}

void Display::setDdramAddress(uint8_t offset) {
  writeCommand(static_cast<uint8_t>(Command::SET_DDRAM_ADDR) | offset);
}

void Display::setCgramAddress(uint8_t offset) {
  writeCommand(static_cast<uint8_t>(Command::SET_CGRAM_ADDR) | offset);
}

void Display::writeChar(uint8_t c) {
  uint8_t tmp[] = {0x40, static_cast<uint8_t>(c)};
  i2c::start(devAddr);
  i2c::writeArray(tmp, sizeof(tmp));
  i2c::stop();
}

void Display::writeArray(const uint8_t *data, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    writeChar(data[i]);
  }
}

void Display::writeString(const char *str) {
  const uint8_t *ptr = reinterpret_cast<const uint8_t *>(str);
  while (*ptr) {
    writeChar(*ptr++);
  }
}

void Display::writeProgmemArray(const uint8_t *data, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    writeChar(pgm_read_byte(&data[i]));
  }
}

void Display::writeProgmemString(const char *str) {
  const uint8_t *ptr = reinterpret_cast<const uint8_t *>(str);
  while (*ptr) {
    writeChar(pgm_read_byte(ptr++));
  }
}

#endif

}  // namespace aqm0802