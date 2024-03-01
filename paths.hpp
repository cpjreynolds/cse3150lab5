#ifndef PATHS_HPP
#define PATHS_HPP

#include <map>
#include "matrix.hpp"

adjmat exact0paths(adjmat& dm1, adjmat& d0, adjmat& d1);

// takes a complete adjacency list instead of 3 separate ones
adjmat exact0paths(const adjmat& mat);

// takes just an edge set
adjmat exact0paths(const std::map<std::pair<int, int>, int>& edges);

#endif
