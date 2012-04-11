#pragma once

#include <boost/spirit/include/qi.hpp>

namespace grammar {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template <typename Iterator>
    struct skipper : qi::grammar<Iterator> {
        skipper() : skipper::base_type(start) {
            start = (ascii::space - qi::eol)
                | ';' >> *(qi::char_ - qi::eol);
        }

        qi::rule<Iterator> start;
    };
}