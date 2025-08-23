
#include <stdint.h>

#include <avr/pgmspace.h>

#include "tiny0/delay.hpp"
#include "tiny0/i2c.hpp"

#define AQM0802_INLINE inline __attribute__((always_inline))
#define AQM0802_NOINLINE __attribute__((noinline))

namespace aqm0802 {

static constexpr int CGRAM_DEPTH = 6;

enum DCX : uint8_t {
  DATA = 0x40,
  COMMAND = 0x00,
};

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

class Display {
 public:
  const uint8_t devAddr;

  AQM0802_INLINE Display(uint8_t devaddr = (0x3E << 1)) : devAddr(devaddr) {}

  AQM0802_INLINE void init() {
    for (uint8_t i = 0; i < sizeof(INIT_SEQUENCE); i += 2) {
      uint8_t byte1 = pgm_read_byte(&INIT_SEQUENCE[i]);
      uint8_t byte2 = pgm_read_byte(&INIT_SEQUENCE[i + 1]);
      if (byte1 == 0) {
        // Command
        writeCommand(DCX::COMMAND, byte2);
        delayUs(50);
      } else {
        // Delay in ms
        for (uint8_t t = byte2; t > 0; t--) {
          delayMs(1);
        }
      }
    }
  }

  AQM0802_NOINLINE void writeCommand(DCX dc, uint8_t cmd);
  AQM0802_NOINLINE void writeArray(const uint8_t *data, uint8_t length);
  AQM0802_NOINLINE void writeString(const char *str);
  AQM0802_NOINLINE void writeProgmemArray(const uint8_t *data, uint8_t length);
  AQM0802_NOINLINE void writeProgmemString(const char *str);

  AQM0802_INLINE void writeChar(uint8_t c) { writeCommand(DCX::DATA, c); }

  AQM0802_INLINE void setDdramAddress(uint8_t offset) {
    writeCommand(DCX::COMMAND,
                 static_cast<uint8_t>(Command::SET_DDRAM_ADDR) | offset);
  }

  AQM0802_INLINE void setCgramAddress(uint8_t offset) {
    writeCommand(DCX::COMMAND,
                 static_cast<uint8_t>(Command::SET_CGRAM_ADDR) | offset);
  }
};

#ifdef AQM0802_INCLUDE_IMPL

AQM0802_NOINLINE void Display::writeCommand(DCX dc, uint8_t cmd) {
  uint8_t data[2] = {static_cast<uint8_t>(dc), cmd};
  i2c::start(devAddr);
  i2c::writeArray(data, sizeof(data));
  i2c::stop();
}

AQM0802_NOINLINE void Display::writeArray(const uint8_t *data, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    writeChar(data[i]);
  }
}

AQM0802_NOINLINE void Display::writeString(const char *str) {
  const uint8_t *ptr = reinterpret_cast<const uint8_t *>(str);
  while (*ptr) {
    writeChar(*ptr++);
  }
}

AQM0802_NOINLINE void Display::writeProgmemArray(const uint8_t *data,
                                                 uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    writeChar(pgm_read_byte(&data[i]));
  }
}

AQM0802_NOINLINE void Display::writeProgmemString(const char *str) {
  const uint8_t *ptr = reinterpret_cast<const uint8_t *>(str);
  char c;
  while (c = pgm_read_byte(ptr++)) {
    writeChar(c);
  }
}
#endif

}  // namespace aqm0802