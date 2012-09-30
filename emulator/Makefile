CXX=g++-4.7
CXX_FLAGS=-std=c++11 -Wall `wx-config --cxxflags`
LIBS=-lpthread `wx-config --libs`
TEST_LIBS=-lpthread -lgtest -lgtest_main
TEST_CXX_FLAGS=-I./src

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXX_FLAGS += -g
	OUTPUT_DIR = target/debug
else
	CXX_FLAGS += -O2
	OUTPUT_DIR = target/release
endif

HARDWARE_DEPS=src/dcpu.hpp src/hardware.hpp
DCPU_DEPS=src/dcpu.hpp src/hardware.hpp
ARGUMENT_DEPS=src/dcpu.hpp src/argument.hpp
OPCODES_DEPS=src/dcpu.hpp src/argument.hpp src/opcodes.hpp
DCPU_THREAD_DEPS=src/ui/dcpu_thread.hpp src/dcpu.hpp
EMULATOR_DEPS=src/emulator.hpp src/ui/*.hpp

OBJECTS = $(OUTPUT_DIR)/dcpu.o \
	$(OUTPUT_DIR)/hardware.o \
	$(OUTPUT_DIR)/opcodes.o \
	$(OUTPUT_DIR)/argument.o 

UI_OBJECTS = $(OBJECTS) \
    $(OUTPUT_DIR)/emulator.o \
    $(OUTPUT_DIR)/dcpu_thread.o

TEST_OBJECTS = $(OBJECTS) \
	$(OUTPUT_DIR)/opcodes_test.o \
	$(OUTPUT_DIR)/opcodes_parse_test.o \
	$(OUTPUT_DIR)/arguments_test.o \
	$(OUTPUT_DIR)/test_hardware.o

TEST_FILTER = *

all: emulator test

emulator: $(UI_OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ $(LIBS) -o $@
	
$(OUTPUT_DIR)/emulator.o: src/emulator.cpp $(EMULATOR_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/dcpu_thread.o: src/ui/dcpu_thread.cpp $(DCPU_THREAD_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/hardware.o: src/hardware.cpp $(HARDWARE_DEPS)| $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/dcpu.o: src/dcpu.cpp $(DCPU_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/opcodes.o: src/opcodes.cpp $(OPCODES_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/argument.o: src/argument.cpp $(ARGUMENT_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR):
	mkdir -p $@

$(OUTPUT_DIR)/opcodes_test.o: test/opcodes_test.cpp $(OPCODES_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/opcodes_parse_test.o: test/opcodes_parse_test.cpp $(OPCODES_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/arguments_test.o: test/arguments_test.cpp $(ARGUMENT_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/test_hardware.o: test/utils/test_hardware.cpp test/utils/test_hardware.hpp $(HARDWARE_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

unittest: $(TEST_OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ $(TEST_LIBS) -o $@

test: unittest
	./unittest --gtest_filter=$(TEST_FILTER)

clean:
	rm -Rf target
	rm -f emulator
	rm -f unittest
