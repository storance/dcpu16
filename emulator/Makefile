CXX=clang
CXX_FLAGS:=-std=c++11 -Wall -stdlib=libstdc++ `wx-config --cxxflags`
LIBS:=`wx-config --libs` -lstdc++ -lboost_program_options
TEST_CXX_FLAGS=-I./src

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXX_FLAGS += -g
	OUTPUT_DIR = target/debug
else
	CXX_FLAGS += -O2
	OUTPUT_DIR = target/release
endif

OBJECTS = $(OUTPUT_DIR)/emulator.o

TEST_OBJECTS = $(OBJECTS)

TEST_FILTER = *

all: emulator

emulator: $(OUTPUT_DIR)/emulator.o $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ $(LIBS) -o $@
	
$(OUTPUT_DIR)/emulator.o: src/emulator.cpp src/emulator.h | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR):
	mkdir -p $@

$(OUTPUT_DIR)/lexer_test.o: test/lexer_test.cpp $(LEXER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/expression_eval_test.o: test/expression_eval_test.cpp $(EXPRESSION_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/parser_test.o: test/parser_test.cpp $(PARSER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/expression_parser_test.o: test/expression_parser_test.cpp $(EXPRESSION_PARSER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/compiler_test.o: test/compiler_test.cpp $(COMPILER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/resolve_symbols_test.o: test/resolve_symbols_test.cpp $(SYMBOL_TABLE_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

unittest: $(TEST_OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -lstdc++ -lpthread -lgtest -lgtest_main -o $@

test: unittest
	./unittest --gtest_filter=$(TEST_FILTER)

clean:
	rm -Rf target
	rm -f emulator
	rm -f unittest
