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

OBJECTS = $(OUTPUT_DIR)/dcpu.o \
	$(OUTPUT_DIR)/hardware.o \
	$(OUTPUT_DIR)/opcodes.o \
	$(OUTPUT_DIR)/argument.o

TEST_OBJECTS = $(OBJECTS) \
	$(OUTPUT_DIR)/opcodes_test.o

TEST_FILTER = *

all: emulator test

emulator: $(OUTPUT_DIR)/emulator.o $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ $(LIBS) -o $@
	
$(OUTPUT_DIR)/emulator.o: src/emulator.cpp src/emulator.h | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/hardware.o: src/hardware.cpp src/hardware.hpp | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/dcpu.o: src/dcpu.cpp src/dcpu.hpp | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/opcodes.o: src/opcodes.cpp src/opcodes.hpp | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/argument.o: src/argument.cpp src/argument.hpp | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR):
	mkdir -p $@

$(OUTPUT_DIR)/opcodes_test.o: test/opcodes_test.cpp $(LEXER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

unittest: $(TEST_OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -lstdc++ -lpthread -lgtest -lgtest_main -o $@

test: unittest
	./unittest --gtest_filter=$(TEST_FILTER)

clean:
	rm -Rf target
	rm -f emulator
	rm -f unittest
