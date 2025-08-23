.PHONY: all build fuse-read fuse-write flash-write clean

APP_NAME := tiny402_ssd1306_scroll

REPO_DIR := ..

BUILD_DIR := build
TOOLS_DIR := tools

MCU := attiny402
F_CPU := 32768

CXX := avr-g++
OBJCOPY := avr-objcopy
AVR_SIZE := avr-size

COMMON_INC_DIR := $(REPO_DIR)/common/include
COMMON_HPP_LIST := $(wildcard $(COMMON_INC_DIR)/tiny0/*.hpp)

APP_SRC_DIR := src
APP_INC_DIR := include
APP_HPP_LIST := $(wildcard $(APP_INC_DIR)/*.hpp) $(COMMON_HPP_LIST)
APP_CPP_LIST := $(wildcard $(APP_SRC_DIR)/*.cpp)
APP_OBJ_LIST := $(patsubst $(APP_SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(APP_CPP_LIST)) 

ELF := $(BUILD_DIR)/$(APP_NAME).elf
HEX := $(BUILD_DIR)/$(APP_NAME).hex

CXXFLAGS := \
	-Os \
	-ffunction-sections \
	-fdata-sections \
	-flto \
	-mmcu=$(MCU) \
	-DF_CPU=$(F_CPU) \
	-I$(APP_INC_DIR) \
	-I$(COMMON_INC_DIR)

LDFLAGS := \
	-Wl,--gc-sections \
	-mmcu=$(MCU)

EXTRA_DEPENDENCIES := \
	Makefile

all: build

build: $(HEX)

$(HEX): $(ELF) $(EXTRA_DEPENDENCIES)
	$(OBJCOPY) -j .text -j .data -j .rodata -O ihex $< $@

$(ELF): $(APP_OBJ_LIST) $(EXTRA_DEPENDENCIES)
	$(CXX) $(LDFLAGS) -o $@ $(APP_OBJ_LIST)
	$(AVR_SIZE) $(ELF)

$(BUILD_DIR)/%.o: $(APP_SRC_DIR)/%.cpp $(APP_HPP_LIST) $(EXTRA_DEPENDENCIES)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

fuse-read:
	avrdude \
		${AVRDUDE_UPDI_OPTS} \
		-p $(MCU) \
		-U bodcfg:r:-:h \
		-U osccfg:r:-:h \
		-U syscfg0:r:-:h

fuse-write:
	avrdude \
		${AVRDUDE_UPDI_OPTS} \
		-p $(MCU) \
		-x rtsdtr=low \
		-U bodcfg:w:0x10:m \
		-U osccfg:w:0x01:m \
		-U syscfg0:w:0xf7:m

flash-write: $(HEX)
	avrdude \
		${AVRDUDE_UPDI_OPTS} \
		-p $(MCU) \
		-x rtsdtr=low \
		-U flash:w:$(HEX)

clean:
	rm -rf $(BUILD_DIR)
