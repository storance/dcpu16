CXX=g++
CPP_FLAGS=-std=c++0x
CXX_FLAGS=-g -Wall
TEST_CXX_FLAGS=-I.
OUTPUT_DIR=target

OBJECTS = $(OUTPUT_DIR)/lexer.o \
		  $(OUTPUT_DIR)/error_handler.o \
		  $(OUTPUT_DIR)/symbol_table.o \
		  $(OUTPUT_DIR)/compiler.o \
		  $(OUTPUT_DIR)/token.o \
		  $(OUTPUT_DIR)/mnemonics.o \
		  $(OUTPUT_DIR)/expression.o \
		  $(OUTPUT_DIR)/statement.o \
		  $(OUTPUT_DIR)/expression_parser.o 

TEST_OBJECTS = $(OBJECTS) \
			   $(OUTPUT_DIR)/lexer_test.o \
			   $(OUTPUT_DIR)/expression_eval_test.o \
			   $(OUTPUT_DIR)/resolve_symbols_test.o \
			   $(OUTPUT_DIR)/compiler_test.o \
			   $(OUTPUT_DIR)/expression_parser_test.o 
#			   $(OUTPUT_DIR)/ParserTest.o \


TEST_FILTER = *

all: test assembler

assembler: $(OUTPUT_DIR)/Assembler.o $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(OUTPUT_DIR)/Assembler.o: Assembler.cpp Lexer.hpp Parser.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/compiler.o: compiler.cpp compiler.hpp expression.hpp statement.hpp token.hpp symbol_table.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/token.o: token.cpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/mnemonics.o: mnemonics.cpp mnemonics.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/statement.o: statement.cpp statement.hpp expression.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/expression.o: expression.cpp expression.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/symbol_table.o: symbol_table.cpp symbol_table.hpp statement.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/error_handler.o: error_handler.cpp error_handler.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/lexer.o: lexer.cpp lexer.hpp token.hpp error_handler.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/expression_parser.o: expression_parser.cpp expression_parser.hpp expression.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Parser.o: Parser.cpp Parser.hpp ast/*.hpp Token.hpp Lexer.hpp ExpressionParser.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR):
	mkdir $@

$(OUTPUT_DIR)/lexer_test.o: test/lexer_test.cpp lexer.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<
	
$(OUTPUT_DIR)/expression_eval_test.o: test/expression_eval_test.cpp expression.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ParserTest.o: test/ParserTest.cpp Parser.hpp Parser.cpp ErrorHandler.hpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/expression_parser_test.o: test/expression_parser_test.cpp expression_parser.hpp expression.hpp lexer.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/compiler_test.o: test/compiler_test.cpp statement.hpp compiler.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/resolve_symbols_test.o: test/resolve_symbols_test.cpp symbol_table.hpp statement.hpp expression.hpp token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

unittest: $(TEST_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lpthread -lgtest -lgtest_main -o $@

test: unittest
	./unittest --gtest_filter=$(TEST_FILTER)

clean:
	rm -Rf target
	rm -f assembler
	rm -f unittest
