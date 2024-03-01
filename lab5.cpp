#ifndef TESTING

#include "csg.hpp"
#include "matrix.hpp"
#include "tcolor.hpp"
#include "paths.hpp"

#include <fstream>
#include <stdexcept>
#include <getopt.h>

using std::ifstream;
using std::string;

static std::string usage()
{
    std::string use(BOLD "usage:\n" RESET);
    use += "\tlab5.out f(-1) f(0) f(1)" + string(4, ' ') +
           "read initial adjacency matrices\n";
    use += "\tlab5.out -c file" + string(4 * 3, ' ') + "read .csg file\n";
    use +=
        "\tlab5.out -i" + string(4 * 5 - 3, ' ') + "read.csg file from stdin\n";
    return use;
}

// does the thing the assignment page requires
static adjmat do_3file_input(char** argv)
{
    std::ifstream ifiles[] = {ifstream(argv[0]), ifstream(argv[1]),
                              ifstream(argv[2])};

    for (int i = 0; i < 3; ++i) {
        if (!ifiles[i].is_open()) {
            std::string msg(argv[i + 1]);
            throw std::runtime_error(msg + ": no such file");
        }
    }

    adjmat mats[3];
    for (int i = 0; i < 3; ++i) {
        ifiles[i] >> mats[i];
    }

    return exact0paths(mats[0], mats[1], mats[2]);
}

static constexpr uint8_t itact = 0b01;
static constexpr uint8_t csgf = 0b10;

int main(int argc, char** argv)
try {
    uint8_t flags = 0;
    std::string csgfname;
    int opt;

    while ((opt = getopt(argc, argv, "ic:")) != -1) {
        switch (opt) {
        case 'i':
            flags |= itact;
            break;
        case 'c':
            flags |= csgf;
            csgfname = std::string(optarg);
            break;
        case '?':
        default:
            throw std::runtime_error("invalid arguments");
        }
    }

    std::map<std::pair<int, int>, int> edges;
    adjmat result;

    int nargs = argc - optind;

    if (flags == 3 || (!flags && nargs != 3) || (flags && nargs)) {
        throw std::runtime_error("invalid arguments");
    }

    if (flags & itact) {
        edges = csg::parse(std::cin);
        result = exact0paths(edges);
    }
    else if (flags & csgf) {
        edges = csg::parse(csgfname);
        result = exact0paths(edges);
    }
    else {
        result = do_3file_input(argv + optind);
    }

    std::cout << result;
    return 0;
}
catch (const csg::parse_error& e) {
    std::cerr << e.what() << '\n';
    return 1;
}
catch (const std::exception& e) {
    std::cerr << RED BOLD "error: " RESET;
    std::cerr << e.what() << "\n\n" << usage();
    return 1;
}
catch (...) {
    std::cerr << RED BOLD "unexpected error\n" RESET;
    std::cerr << usage();
    return 1;
}

#else
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#endif
