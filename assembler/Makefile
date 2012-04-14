GTEST_DIR=/opt/gtest-1.6.0
CXX=g++
CPP_FLAGS=-std=c++0x
CPP_TEST_FLAGS=$(CPP_FLAGS) -I$(GTEST_DIR)/include
CXX_FLAGS=
OUTPUT=target

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
				$(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

all:

$(OUTPUT)/Token.o: Token.cpp $(OUTPUT) Token.hpp
	$(CXX) $(CPP_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT):
	mkdir $@

$(OUTPUT)/LexerTest.o: test/LexerTest.cpp $(OUTPUT) Lexer.hpp Lexer.cpp
	$(CXX) $(CPP_TEST_FLAGS) $(CXX_FLAGS) -c -o $@ $<

$(OUTPUT)/gtest-all.o : $(OUTPUT) $(GTEST_SRCS)
	$(CXX) $(CPP_TEST_FLAGS) -I$(GTEST_DIR) $(CXX_FLAGS) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

$(OUTPUT)/gtest_main.o : $(OUTPUT) $(GTEST_SRCS)
	$(CXX) $(CPP_TEST_FLAGS) -I$(GTEST_DIR) $(CXX_FLAGS) -c $(GTEST_DIR)/src/gtest_main.cc -o $@

$(OUTPUT)/gtest.a : $(OUTPUT)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

$(OUTPUT)/gtest_main.a : $(OUTPUT)/gtest-all.o $(OUTPUT)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

unittest: $(OUTPUT)/LexerTest.o $(OUTPUT)/Token.o $(OUTPUT)/gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $^ -lpthread -o $@

test: unittest
	./unittest

clean:
	rm -Rf target
	rm assembler
