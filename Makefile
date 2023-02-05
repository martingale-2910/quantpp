SRC_DIR = ./src
SRCS := $(SRC_DIR)/main.cxx

BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)/obj
OBJS = $(SRCS:$(SRC_DIR)/%.cxx=$(OBJ_DIR)/%.o)

TARGET_DIR := $(BUILD_DIR)/target
TARGET := $(TARGET_DIR)/app

.PHONY: all clean release

all: debug
debug: CXX_FLAGS=-std=c++20 -O0 -Wall -Wextra -Wpedantic -g
release: CXX_FLAGS=-std=c++20 -Ofast
debug release: clean build
build: $(TARGET)

$(TARGET): $(OBJS)
	@echo 'Linking $@ from $<'
	@echo '[Rule] $@: $<'
	mkdir -p $(dir $@)
	$(CXX) $< -o $@ $(LDFLAGS)

$(OBJS): $(SRCS)
	@echo 'Compiling $@ from $<'
	@echo '[Rule] $@: $<'
	mkdir -p $(dir $@)
	$(CXX) -c $(CXX_FLAGS) $(CPP_FLAGS) $< -o $@

clean: 
	@echo 'Cleaning $(BUILD_DIR)'
	rm -rf $(BUILD_DIR)