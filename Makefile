SRC_DIR = ./src
SRCS := $(SRC_DIR)/main.cxx

BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)/obj
OBJS = $(SRCS:$(SRC_DIR)/%.cxx=$(OBJ_DIR)/%.o)

TARGET_DIR := $(BUILD_DIR)/target
TARGET := $(TARGET_DIR)/quantpp_app

CXX = g++
ifeq ($(CXX), clang++)
	LDFLAGS = -lstdc++ -ldl -lm
endif

.PHONY: all clean debug release build run

all: debug
debug: CXX_FLAGS=-std=c++17 -O0 -Wall -Wextra -Wpedantic -g -Wpointer-arith 
release: CXX_FLAGS=-std=c++17 -Ofast -finline-functions -ffast-math -funroll-all-loops
debug release: clean build
build: $(TARGET)

run: $(TARGET)
	@echo 'Running $(TARGET)'
	$(TARGET)

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