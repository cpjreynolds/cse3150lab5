#ifndef TESTING
#include <stdexcept>

#include "csg.hpp"
#include "matrix.hpp"
#include "tcolor.hpp"

#include <fstream>

using std::ifstream;

static constexpr std::string_view USAGE =
    BOLD "USAGE: " RESET "\n\t./lab5.out [csgfile=<stdin>]"
         "\n\t./lab5.out file(-1) file(0) file(1)";

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
    else if (argc == 4) {
        std::ifstream ifiles[] = {ifstream(argv[1]), ifstream(argv[2]),
                                  ifstream(argv[3])};
        for (int i = 0; i < argc - 1; ++i) {
            if (!ifiles[i].is_open()) {
                std::string msg(argv[i + 1]);
                throw std::runtime_error(msg + ": no such file");
            }
        }

        adjmat mats[3];
        for (int i = 0; i < 3; ++i) {
            ifiles[i] >> mats[i];
        }
        for (int i = 0; i < 3; ++i) {
            std::cout << mats[i] << '\n';
        }
    }
    else {
        throw std::runtime_error("invalid arguments");
    }

    if (!edges.empty()) {
        std::cout << edges;
        std::cout << adjmat(edges);
    }

    return 0;
}
catch (const csg::parse_error& e) {
    std::cerr << e.what() << '\n';
}
catch (const std::exception& e) {
    std::cerr << RED BOLD "error: " RESET;
    std::cerr << e.what() << "\n\n" << USAGE << '\n';
    return 1;
}
catch (...) {
    std::cerr << RED BOLD "unexpected error\n" RESET;
    std::cerr << USAGE << '\n';
    return 1;
}

#else
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#endif
