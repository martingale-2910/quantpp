SRC_DIR = ./src
SRCS := $(shell find $(SRC_DIR) -name '*.cxx' -or -name '*.s')

INC_DIRS = $(shell find $(SRC_DIR) -type d)
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

BUILD_DIR := ./build
OBJ_DIR := $(BUILD_DIR)/obj
OBJS = $(SRCS:$(SRC_DIR)/%.cxx=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

TARGET_DIR := $(BUILD_DIR)/target
TARGET := $(TARGET_DIR)/quantpp_app

CXX = g++
CXX_FLAGS = -std=c++17
ifeq ($(CXX), clang++)
	LDFLAGS = -lstdc++ -ldl -lm
endif
CPP_FLAGS = $(INC_FLAGS) -MMD -MP

.PHONY: all clean debug release build run

all: debug
debug: CXX_FLAGS+=-O0 -Wall -Wextra -Wpedantic -g -Wpointer-arith 
release: CXX_FLAGS+=-Ofast -finline-functions -ffast-math -funroll-all-loops
debug release: clean build
build: $(TARGET)

run: $(TARGET)
	@echo 'Running $(TARGET) with args rate=$(RATE), vol=$(VOL), spot=$(SPOT), strike=$(STRIKE), ttm=$(TTM), npaths=$(NPATHS), nsteps=$(NSTEPS)'
	$(TARGET) --rate $(RATE) --vol $(VOL) --spot $(SPOT) --strike $(STRIKE) --ttm $(TTM) --npaths $(NPATHS) --nsteps $(NSTEPS)

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

-include $(DEPS)