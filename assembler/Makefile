CXX=clang++
CXX_FLAGS=-std=c++11 -Wall -stdlib=libc++
TEST_CXX_FLAGS=-I./src -DGTEST_USE_OWN_TR1_TUPLE

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXX_FLAGS += -g
	OUTPUT_DIR = target/debug
else
	CXX_FLAGS += -O2
	OUTPUT_DIR = target/release
endif

OBJECTS = $(OUTPUT_DIR)/lexer.o \
		  $(OUTPUT_DIR)/log.o \
		  $(OUTPUT_DIR)/symbol_table.o \
		  $(OUTPUT_DIR)/compiler.o \
		  $(OUTPUT_DIR)/token.o \
		  $(OUTPUT_DIR)/mnemonics.o \
		  $(OUTPUT_DIR)/expression.o \
		  $(OUTPUT_DIR)/statement.o \
		  $(OUTPUT_DIR)/expression_parser.o \
		  $(OUTPUT_DIR)/parser.o

TEST_OBJECTS = $(OBJECTS) \
			   $(OUTPUT_DIR)/lexer_test.o \
			   $(OUTPUT_DIR)/expression_eval_test.o \
			   $(OUTPUT_DIR)/resolve_symbols_test.o \
			   $(OUTPUT_DIR)/compiler_test.o \
			   $(OUTPUT_DIR)/expression_parser_test.o \
			   $(OUTPUT_DIR)/parser_test.o

MNEMONICS_DEP=src/mnemonics.hpp
TOKEN_DEPS=$(sort src/token.hpp $(MNEMONICS_DEP))
LOG_DEPS=$(sort src/log.hpp $(TOKEN_DEPS))
LEXER_DEPS=$(sort src/lexer.hpp $(LOG_DEPS))
EXPRESSION_DEPS=$(sort src/expression.hpp src/symbol_table.hpp $(TOKEN_DEPS))
STATEMENT_DEPS=$(sort src/statement.hpp $(EXPRESSION_DEPS))
EXPRESSION_PARSER_DEPS=$(sort src/expression_parser.hpp $(LOG_DEPS) $(EXPRESSION_DEPS))
PARSER_DEPS=$(sort src/parser.hpp $(EXPRESSION_PARSER_DEPS) $(STATEMENT_DEPS) $(LEXER_DEPS))
SYMBOL_TABLE_DEPS=$(sort src/symbol_table.hpp $(STATEMENT_DEPS) $(LOG_DEPS))
COMPILER_DEPS=$(sort src/compiler.hpp $(SYMBOL_TABLE_DEPS))
ASSEMBLER_DEPS=$(sort $(PARSER_DEPS) $(SYMBOL_TABLE_DEPS))

TEST_FILTER = *

all: test assembler

assembler: $(OUTPUT_DIR)/assembler.o $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -lboost_program_options -o $@
	
$(OUTPUT_DIR)/mnemonics.o: src/mnemonics.cpp $(MNEMONICS_DEP) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/token.o: src/token.cpp $(TOKEN_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/log.o: src/log.cpp $(LOG_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/lexer.o: src/lexer.cpp $(LEXER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/expression.o: src/expression.cpp $(EXPRESSION_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/statement.o: src/statement.cpp $(STATEMENT_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/expression_parser.o: src/expression_parser.cpp $(EXPRESSION_PARSER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/parser.o: src/parser.cpp $(PARSER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/symbol_table.o: src/symbol_table.cpp $(SYMBOL_TABLE_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/compiler.o: src/compiler.cpp $(COMPILER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/assembler.o: src/assembler.cpp $(ASSEMBLER_DEPS) | $(OUTPUT_DIR)
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
	$(CXX) $(CXX_FLAGS) $^ -lpthread -lgtest -lgtest_main -o $@

test: unittest
	./unittest --gtest_filter=$(TEST_FILTER)

clean:
	rm -Rf target
	rm -f assembler
	rm -f unittest
