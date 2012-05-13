CXX=g++
CXX_FLAGS=-std=c++0x -Wall
TEST_CXX_FLAGS=-I.

DEBUG ?= 1
ifeq ($(DEBUG), 1)
	CXX_FLAGS += -g
	OUTPUT_DIR = target/debug
else
	CXX_FLAGS += -O2
	OUTPUT_DIR = target/release
endif

OBJECTS = $(OUTPUT_DIR)/lexer.o \
		  $(OUTPUT_DIR)/error_handler.o \
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

MNEMONICS_DEP=mnemonics.hpp
TOKEN_DEPS=$(sort token.hpp $(MNEMONICS_DEP))
ERROR_HANDLER_DEPS=$(sort error_handler.hpp $(TOKEN_DEPS))
LEXER_DEPS=$(sort lexer.hpp $(ERROR_HANDLER_DEPS))
EXPRESSION_DEPS=$(sort expression.hpp symbol_table.hpp $(TOKEN_DEPS))
STATEMENT_DEPS=$(sort statement.hpp $(EXPRESSION_DEPS))
EXPRESSION_PARSER_DEPS=$(sort expression_parser.hpp $(ERROR_HANDLER_DEPS) $(EXPRESSION_DEPS))
PARSER_DEPS=$(sort parser.hpp $(EXPRESSION_PARSER_DEPS) $(STATEMENT_DEPS) $(LEXER_DEPS))
SYMBOL_TABLE_DEPS=$(sort symbol_table.hpp $(STATEMENT_DEPS) $(ERROR_HANDLER_DEPS))
COMPILER_DEPS=$(sort compiler.hpp $(SYMBOL_TABLE_DEPS))
ASSEMBLER_DEPS=$(sort $(PARSER_DEPS) $(SYMBOL_TABLE_DEPS))

TEST_FILTER = *

all: test assembler

assembler: $(OUTPUT_DIR)/assembler.o $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $^ -lboost_program_options -o $@
	
$(OUTPUT_DIR)/mnemonics.o: mnemonics.cpp $(MNEMONICS_DEP) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/token.o: token.cpp $(TOKEN_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/error_handler.o: error_handler.cpp $(ERROR_HANDLER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/lexer.o: lexer.cpp lexer.hpp $(LEXER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/expression.o: expression.cpp $(EXPRESSION_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/statement.o: statement.cpp $(STATEMENT_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/expression_parser.o: expression_parser.cpp $(EXPRESSION_PARSER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/parser.o: parser.cpp parser.hpp $(PARSER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/symbol_table.o: symbol_table.cpp $(SYMBOL_TABLE_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/compiler.o: compiler.cpp $(COMPILER_DEPS) | $(OUTPUT_DIR)
	$(CXX) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/assembler.o: assembler.cpp $(ASSEMBLER_DEPS) | $(OUTPUT_DIR)
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
