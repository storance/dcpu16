CXX=g++
CPP_FLAGS=-std=c++0x
CPP_TEST_FLAGS=$(CPP_FLAGS) -I$(GTEST_DIR)/include
CXX_FLAGS=-g
OUTPUT_DIR=target
GTEST_DIR=/opt/gtest-1.6.0

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
				$(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

OBJECTS = $(OUTPUT_DIR)/Parser.o \
		  $(OUTPUT_DIR)/ErrorHandler.o \
		  $(OUTPUT_DIR)/Statement.o \
		  $(OUTPUT_DIR)/Argument.o \
		  $(OUTPUT_DIR)/Expression.o \
		  $(OUTPUT_DIR)/Token.o

TEST_OBJECTS = $(OBJECTS) \
			   $(OUTPUT_DIR)/LexerTest.o \
			   $(OUTPUT_DIR)/ParserTest.o \
			   $(OUTPUT_DIR)/gtest_main.a

all:

$(OUTPUT_DIR)/Token.o: Token.cpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Statement.o: ast/Statement.cpp ast/Statement.hpp ast/Argument.hpp ast/Expression.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Argument.o: ast/Argument.cpp ast/Argument.hpp ast/Expression.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Expression.o: ast/Expression.cpp ast/Expression.hpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ErrorHandler.o: ErrorHandler.cpp ErrorHandler.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Parser.o: Parser.cpp Parser.hpp ast/*.hpp Token.hpp Lexer.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR):
	mkdir $@

$(OUTPUT_DIR)/LexerTest.o: test/LexerTest.cpp Lexer.hpp Lexer.cpp Token.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_TEST_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/ParserTest.o: test/ParserTest.cpp Parser.hpp Parser.cpp ErrorHandler.hpp ast/*.hpp | $(OUTPUT_DIR)
	$(CXX) $(CPP_TEST_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/gtest-all.o : $(GTEST_SRCS) | $(OUTPUT_DIR) 
	$(CXX) $(CPP_TEST_FLAGS) -I$(GTEST_DIR) $(CXX_FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

$(OUTPUT_DIR)/gtest_main.o : $(GTEST_SRCS) | $(OUTPUT_DIR) 
	$(CXX) $(CPP_TEST_FLAGS) -I$(GTEST_DIR) $(CXX_FLAGS) -c $(GTEST_DIR)/src/gtest_main.cc -o $@

$(OUTPUT_DIR)/gtest.a : $(OUTPUT_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(OUTPUT_DIR)/gtest_main.a : $(OUTPUT_DIR)/gtest-all.o $(OUTPUT_DIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

unittest: $(TEST_OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lpthread -o $@

test: unittest
	./unittest

clean:
	rm -Rf target
	rm assembler
	rm unittest
