#include <array>
#include "paths.hpp"

static std::array<adjmat, 3> init_adjmats(const adjmat& mat)
{
    auto blank = adjmat(mat.dim(), 2);
    blank.vmap(mat.vmap());
    std::array<adjmat, 3> mats{blank, blank, blank};
    for (auto r = 0u; r < mat.dim(); ++r) {
        for (auto c = 0u; c < mat.dim(); ++c) {
            int val = mat(r, c);
            if (val == -1) {
                mats[0](r, c) = val;
            }
            else if (val == 1) {
                mats[2](r, c) = val;
            }
            else if (val == 0) {
                // this doesn't actually occur
                // zero adjacency to start means inf
                [[unlikely]] throw std::runtime_error("unreachable");
            }
        }
    }
    return mats;
}

adjmat exact0paths(const std::map<std::pair<int, int>, int>& edges)
{
    return exact0paths(adjmat(edges));
}

adjmat exact0paths(const adjmat& mat)
{
    auto mats = init_adjmats(mat);
    return exact0paths(mats[0], mats[1], mats[2]);
}

adjmat exact0paths(adjmat& dm1, adjmat& d0, adjmat& d1)
{
    int n = d0.dim();
    for (int l = 2; l < 3 * n * n + 1; ++l) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                for (int k = 0; k < n; ++k) {
                    if (dm1(i, k) + d1(k, j) == 0 ||
                        d1(i, k) + dm1(k, j) == 0) {
                        d0(i, j) = 0;
                    }
                    if (d1(i, k) + d0(k, j) == 1 || d0(i, k) + d1(k, j) == 1) {
                        d1(i, j) = 1;
                    }
                    if (dm1(i, k) + d0(k, j) == -1 ||
                        d0(i, k) + dm1(k, j) == -1) {
                        dm1(i, j) = -1;
                    }
                }
            }
        }
    }

    return d0;
}
