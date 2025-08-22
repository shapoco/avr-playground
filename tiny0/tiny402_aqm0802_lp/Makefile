.PHONY: all build flash clean

APP_NAME := tiny402_ssd1306_scroll

REPO_DIR := ..

BUILD_DIR := build
TOOLS_DIR := tools

MCU := attiny402
F_CPU := 32768

CXX := $(AVR_GCC_PATH)/bin/avr-g++
OBJCOPY := $(AVR_GCC_PATH)/bin/avr-objcopy
AVR_SIZE := $(AVR_GCC_PATH)/bin/avr-size

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
	-mmcu=$(MCU) \
	-DF_CPU=$(F_CPU) \
	-I$(APP_INC_DIR) \
	-I$(COMMON_INC_DIR)

LDFLAGS := \
	-Os \
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

include $(TOOLS_DIR)/avrdude_wsl/tools.mk

clean:
	rm -rf $(BUILD_DIR)
