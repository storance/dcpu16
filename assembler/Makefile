CXX=g++
CPP_FLAGS=-std=c++0x
CXX_FLAGS=-g -Wall
TEST_CXX_FLAGS=-I.
OUTPUT_DIR=target

OBJECTS = $(OUTPUT_DIR)/Parser.o \
		  $(OUTPUT_DIR)/ExpressionParser.o \
		  $(OUTPUT_DIR)/Lexer.o \
		  $(OUTPUT_DIR)/ErrorHandler.o \
		  $(OUTPUT_DIR)/Common.o \
		  $(OUTPUT_DIR)/Statement.o \
		  $(OUTPUT_DIR)/Argument.o \
		  $(OUTPUT_DIR)/Expression.o \
		  $(OUTPUT_DIR)/Token.o \
		  $(OUTPUT_DIR)/RegisterDefinition.o \
		  $(OUTPUT_DIR)/OpcodeDefinition.o \
		  $(OUTPUT_DIR)/SymbolTable.o \
		  $(OUTPUT_DIR)/Compiler.o

TEST_OBJECTS = $(OBJECTS) \
			   $(OUTPUT_DIR)/ParserTest.o \
			   $(OUTPUT_DIR)/LexerTest.o \
			   $(OUTPUT_DIR)/CompileTest.o \
			   $(OUTPUT_DIR)/ResolveLabelTest.o \
			   $(OUTPUT_DIR)/ExprParserTest.o \
			   $(OUTPUT_DIR)/ExprEvalTest.o

TEST_FILTER = *

all: test assembler

assembler: $(OUTPUT_DIR)/Assembler.o $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(OUTPUT_DIR)/Assembler.o: Assembler.cpp Lexer.hpp Parser.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Compiler.o: Compiler.cpp Compiler.hpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Token.o: Token.cpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Common.o: ast/Common.cpp ast/Common.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Statement.o: ast/Statement.cpp ast/*.hpp *.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Argument.o: ast/Argument.cpp ast/*.hpp *.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Expression.o: ast/Expression.cpp ast/*.hpp *.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/OpcodeDefinition.o: ast/OpcodeDefinition.cpp ast/OpcodeDefinition.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/RegisterDefinition.o: ast/RegisterDefinition.cpp ast/RegisterDefinition.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/SymbolTable.o: SymbolTable.cpp SymbolTable.hpp ast/Statement.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ErrorHandler.o: ErrorHandler.cpp ErrorHandler.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ExpressionParser.o: ExpressionParser.cpp ExpressionParser.hpp ast/Expression.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Parser.o: Parser.cpp Parser.hpp ast/*.hpp Token.hpp Lexer.hpp ExpressionParser.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Lexer.o: Lexer.cpp Lexer.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR):
	mkdir $@

$(OUTPUT_DIR)/LexerTest.o: test/LexerTest.cpp Lexer.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ParserTest.o: test/ParserTest.cpp Parser.hpp Parser.cpp ErrorHandler.hpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ExprParserTest.o: test/ExprParserTest.cpp ExpressionParser.hpp ExpressionParser.cpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) $(TEST_CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ExprEvalTest.o: test/ExprEvalTest.cpp ast/*.hpp | $(OUTPUT_DIR)
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
