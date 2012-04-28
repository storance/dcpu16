CXX=g++
CPP_FLAGS=-std=c++0x
CXX_FLAGS=-g -Wall
OUTPUT_DIR=target

OBJECTS = $(OUTPUT_DIR)/Parser.o \
		  $(OUTPUT_DIR)/ExpressionParser.o \
		  $(OUTPUT_DIR)/Lexer.o \
		  $(OUTPUT_DIR)/ErrorHandler.o \
		  $(OUTPUT_DIR)/Statement.o \
		  $(OUTPUT_DIR)/Argument.o \
		  $(OUTPUT_DIR)/Expression.o \
		  $(OUTPUT_DIR)/Token.o \
		  $(OUTPUT_DIR)/RegisterDefinition.o \
		  $(OUTPUT_DIR)/OpcodeDefinition.o \
		  $(OUTPUT_DIR)/SymbolTable.o

TEST_OBJECTS = $(OBJECTS) \
			   $(OUTPUT_DIR)/Utils.o \
			   $(OUTPUT_DIR)/LexerTest.o \
			   $(OUTPUT_DIR)/ParserTest.o \
			   $(OUTPUT_DIR)/ExpressionParserTest.o 

all: assembler

assembler: $(OUTPUT_DIR)/Assembler.o $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@

$(OUTPUT_DIR)/Assembler.o: Assembler.cpp Lexer.hpp Parser.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Token.o: Token.cpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Statement.o: ast/Statement.cpp ast/Statement.hpp ast/Argument.hpp ast/Expression.hpp ast/OpcodeDefinition.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Argument.o: ast/Argument.cpp ast/Argument.hpp ast/Expression.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Expression.o: ast/Expression.cpp ast/Expression.hpp Token.hpp ast/RegisterDefinition.hpp | $(OUTPUT_DIR)
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

$(OUTPUT_DIR)/Utils.o: test/Utils.cpp test/Utils.hpp ast/Expression.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/LexerTest.o: test/LexerTest.cpp Lexer.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ParserTest.o: test/ParserTest.cpp Parser.hpp Parser.cpp ErrorHandler.hpp ast/*.hpp test/Utils.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ExpressionParserTest.o: test/ExpressionParserTest.cpp ExpressionParser.hpp ExpressionParser.cpp ast/*.hpp test/Utils.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

unittest: $(TEST_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lpthread -lgtest -lgtest_main -o $@

test: unittest
	./unittest

clean:
	rm -Rf target
	rm -f assembler
	rm -f unittest
