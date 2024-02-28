#include <stdexcept>
#ifndef TESTING

#include "csg.hpp"

static constexpr std::string_view USAGE = "USAGE: ./lab5.out [file=<stdin>]";

int main(int argc, char** argv)
try {
    using csg::operator<<;

    std::map<std::pair<int, int>, int> edges;

    if (argc == 1) {
        edges = csg::parse(std::cin);
    }
    else if (argc == 2) {
        edges = csg::parse(argv[1]);
    }
    else {
        throw std::runtime_error("invalid arguments");
    }

    std::cout << edges;

    return 0;
}
catch (const csg::parse_error& e) {
    std::cerr << e.what() << '\n';
}
catch (const std::exception& e) {
    std::cerr << e.what() << "\n\n" << USAGE << '\n';
    return 1;
}

#else
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#endif
