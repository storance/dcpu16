CXX=g++
CPP_FLAGS=-std=c++0x
CXX_FLAGS=-g -Wall
TEST_CXX_FLAGS=-I.
OUTPUT_DIR=target

OBJECTS = $(OUTPUT_DIR)/lexer.o \
		  $(OUTPUT_DIR)/error_handler.o \
		  $(OUTPUT_DIR)/symbol_table.o \
		  $(OUTPUT_DIR)/token.o \
		  $(OUTPUT_DIR)/mnemonics.o \
		  $(OUTPUT_DIR)/expression.o \
		  $(OUTPUT_DIR)/statement.o 
		  

#OBJECTS = $(OUTPUT_DIR)/Parser.o \
#		  $(OUTPUT_DIR)/ExpressionParser.o \
#		  $(OUTPUT_DIR)/Lexer.o \
#		  $(OUTPUT_DIR)/ErrorHandler.o \
#		  $(OUTPUT_DIR)/Common.o \
#		  $(OUTPUT_DIR)/Statement.o \
#		  $(OUTPUT_DIR)/Argument.o \
#		  $(OUTPUT_DIR)/Expression.o \
#		  $(OUTPUT_DIR)/Token.o \
#		  $(OUTPUT_DIR)/RegisterDefinition.o \
#		  $(OUTPUT_DIR)/OpcodeDefinition.o \
#		  $(OUTPUT_DIR)/SymbolTable.o \
#		  $(OUTPUT_DIR)/Compiler.o

TEST_OBJECTS = $(OBJECTS) \
			   $(OUTPUT_DIR)/lexer_test.o \
			   $(OUTPUT_DIR)/expression_eval_test.o
#			   $(OUTPUT_DIR)/ParserTest.o \
#			   $(OUTPUT_DIR)/CompileTest.o \
#			   $(OUTPUT_DIR)/ResolveLabelTest.o \
#			   $(OUTPUT_DIR)/ExprParserTest.o \


TEST_FILTER = *

all: test assembler

assembler: $(OUTPUT_DIR)/Assembler.o $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(OUTPUT_DIR)/Assembler.o: Assembler.cpp Lexer.hpp Parser.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Compiler.o: Compiler.cpp Compiler.hpp ast/*.hpp | $(OUTPUT_DIR)
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
	
$(OUTPUT_DIR)/ExpressionParser.o: ExpressionParser.cpp ExpressionParser.hpp ast/Expression.hpp Token.hpp | $(OUTPUT_DIR)
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

$(OUTPUT_DIR)/ExprParserTest.o: test/ExprParserTest.cpp ExpressionParser.hpp ExpressionParser.cpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/CompileTest.o: test/CompileTest.cpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ResolveLabelTest.o: test/ResolveLabelTest.cpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

unittest: $(TEST_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lpthread -lgtest -lgtest_main -o $@

test: unittest
	./unittest --gtest_filter=$(TEST_FILTER)

clean:
	rm -Rf target
	rm -f assembler
	rm -f unittest
