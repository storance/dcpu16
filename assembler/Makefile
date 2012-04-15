CXX=g++
CPP_FLAGS=-std=c++0x
CPP_TEST_FLAGS=$(CPP_FLAGS) -I$(GTEST_DIR)/include
CXX_FLAGS=
OUTPUT_DIR=target
GTEST_DIR=/opt/gtest-1.6.0

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
				$(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

all:

$(OUTPUT_DIR)/Token.o: Token.cpp Token.hpp $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/AbstractSyntaxTree.o: AbstractSyntaxTree.cpp AbstractSyntaxTree.hpp $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/Parser.o: Parser.cpp Parser.hpp AbstractSyntaxTree.hpp Token.hpp Lexer.hpp $(OUTPUT_DIR)
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR):
	mkdir $@

$(OUTPUT_DIR)/LexerTest.o: test/LexerTest.cpp $(OUTPUT_DIR) Lexer.hpp Lexer.cpp Token.hpp
	$(CXX) $(CPP_TEST_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT_DIR)/gtest-all.o : $(OUTPUT_DIR) $(GTEST_SRCS)
	$(CXX) $(CPP_TEST_FLAGS) -I$(GTEST_DIR) $(CXX_FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

$(OUTPUT_DIR)/gtest_main.o : $(OUTPUT_DIR) $(GTEST_SRCS)
	$(CXX) $(CPP_TEST_FLAGS) -I$(GTEST_DIR) $(CXX_FLAGS) -c $(GTEST_DIR)/src/gtest_main.cc -o $@

$(OUTPUT_DIR)/gtest.a : $(OUTPUT_DIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(OUTPUT_DIR)/gtest_main.a : $(OUTPUT_DIR)/gtest-all.o $(OUTPUT_DIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

unittest: $(OUTPUT_DIR)/LexerTest.o $(OUTPUT_DIR)/Token.o $(OUTPUT_DIR)/gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lpthread -o $@

test: unittest
	./unittest

clean:
	rm -Rf target
	rm assembler
	rm unittest
