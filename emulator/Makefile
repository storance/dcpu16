CXX=clang
CXX_FLAGS=-std=c++11 -Wall -stdlib=libstdc++ `wx-config --cxxflags`
LIBS=`wx-config --libs` -lstdc++ -lboost_program_options
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

OBJECTS = $(OUTPUT_DIR)/dcpu.o \
	$(OUTPUT_DIR)/hardware.o \
	$(OUTPUT_DIR)/opcodes.o \
	$(OUTPUT_DIR)/argument.o

TEST_OBJECTS = $(OBJECTS) \
	$(OUTPUT_DIR)/opcodes_test.o \
	$(OUTPUT_DIR)/arguments_test.o

TEST_FILTER = *

all: emulator test

emulator: $(OUTPUT_DIR)/emulator.o $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ $(LIBS) -o $@
	
$(OUTPUT_DIR)/emulator.o: src/emulator.cpp src/emulator.h | $(OUTPUT_DIR)
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

$(OUTPUT_DIR)/arguments_test.o: test/arguments_test.cpp $(ARGUMENT_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

unittest: $(TEST_OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -lstdc++ -lpthread -lgtest -lgtest_main -o $@

test: unittest
	./unittest --gtest_filter=$(TEST_FILTER)

clean:
	rm -Rf target
	rm -f emulator
	rm -f unittest
