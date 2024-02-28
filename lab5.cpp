#ifndef TESTING

#include "csg.hpp"

int main()
try {
    using csg::operator<<;

    auto edges = csg::parse("text.tx");

    std::cout << edges;

    return 0;
}
catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
}

#else
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#endif
