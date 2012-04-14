#include "Parser.hpp"

using namespace std;

template<typename Iterator>
void Parser<Iterator>::skipWhitespace() {
    skipUtil([] (char c) { c != ' ' && c != '\t';});
}

template<typename Iterator> template<typename Function>
void Parser<Iterator>::skipUntil(Function predicate) {
    for (;current != end; current++) {
        if (predicate(*current)) {
            break;
        }
    }
}
