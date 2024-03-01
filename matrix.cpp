#include "matrix.hpp"
#include "tcolor.hpp"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <set>
#include <ranges>
#include <iomanip>
#include <string_view>

using std::ostream, std::istream;
using std::string, std::string_view;

std::set<int>
adjmat::get_vertices(const std::map<std::pair<int, int>, int>& edges)
{
    std::set<int> vertices;
    for (const auto& [v1, v2] : edges | std::views::keys) {
        vertices.insert({v1, v2});
    }
    return vertices;
}

std::map<int, size_t> adjmat::default_vmap(size_t dim)
{
    std::map<int, size_t> vmap;
    for (auto i = 0u; i < dim; ++i) {
        vmap.insert({i, i});
    }
    return vmap;
}

std::map<int, size_t>
adjmat::gen_vmap(const std::map<std::pair<int, int>, int>& edges)
{
    std::map<int, size_t> vmap;
    auto vertices = get_vertices(edges);
    size_t i = 0;
    for (const auto& vert : vertices) {
        vmap.insert({vert, i});
        ++i;
    }
    return vmap;
}

adjmat::adjmat(const std::map<std::pair<int, int>, int>& edges)
    : _vmap{gen_vmap(edges)}
{
    _dim = _vmap.size();
    _data = std::vector<int>(_dim * _dim, _inf);
    for (const auto& [vertices, wt] : edges) {
        auto& [v1, v2] = vertices;
        (*this)[{v1, v2}] = wt;
    }
}

adjmat::adjmat(std::initializer_list<std::initializer_list<int>> data)
    : _dim{data.size()}
{
    if (data.size() != 0) {
        for (const auto& row : data) {
            if (_dim != row.size()) {
                throw std::logic_error("matrix dimension/data mismatch");
            }
            _data.insert(_data.cend(), row);
        }
    }
}

adjmat::adjmat(const std::vector<std::vector<int>>& data) : _dim{data.size()}
{
    if (data.size() != 0) {
        for (const auto& row : data) {
            if (_dim != row.size()) {
                throw std::logic_error("matrix dimension/data mismatch");
            }
            _data.insert(_data.cend(), row.cbegin(), row.cend());
        }
    }
}

// necessary Unicode box drawing characters.
static constexpr string_view DVBAR = "\u2551";
static constexpr string_view DHBAR = "\u2550";
static constexpr string_view DCROSS = "\u256c";

static constexpr string_view DVDLEFT = "\u2563";
static constexpr string_view DVDRIGHT = "\u2560";
static constexpr string_view DVSRIGHT = "\u255f";
static constexpr string_view DVSLEFT = "\u2562";

static constexpr string_view DHDUP = "\u2569";
static constexpr string_view DHDDOWN = "\u2566";

static constexpr string_view DUPRIGHT = "\u255a";
static constexpr string_view DUPLEFT = "\u255d";
static constexpr string_view DDOWNRIGHT = "\u2554";
static constexpr string_view DDOWNLEFT = "\u2557";

static constexpr string_view DHDOWN = "\u2564";
static constexpr string_view DHUP = "\u2567";
static constexpr string_view DHSV = "\u256a";
static constexpr string_view DVSH = "\u256b";

static constexpr string_view VBAR = "\u2502";
static constexpr string_view HBAR = "\u2500";
static constexpr string_view CROSS = "\u253c";

// my matrices bring all the boys to the yard.
// yes. this code is the stuff of nightmares.
// but it works.
ostream& operator<<(ostream& os, const adjmat& self)
{
    auto wide1 = std::to_string(self._vmap.rbegin()->first).size();
    auto wide2 = std::to_string(*std::ranges::max_element(self._data)).size();
    // minus sign
    auto wide3 = std::to_string(*std::ranges::min_element(self._data)).size();
    auto wide = std::max({wide1, wide2, wide3, 2ul});

    // n=`wide` DHBARs
    // cant use string(wide, DHBAR) because DHBAR is multibyte
    // and I need to keep string<CharT> consistent
    string hdwide;
    for (auto i = 0u; i < wide; ++i) {
        hdwide += DHBAR;
    }

    // top line
    os << DDOWNRIGHT << hdwide << DHDDOWN;
    for (auto i = 0u; i < self.dim() - 1; ++i) {
        os << hdwide << DHDOWN;
    }
    os << hdwide << DDOWNLEFT << '\n';

    // vertex column label line
    os << DVBAR << BOLD << "𝑽" << RESET << string(wide - 1, ' ') << DVBAR;
    auto verts = self._vmap | std::views::keys;
    for (auto it = verts.begin(); it != std::prev(verts.end()); ++it) {
        os << BOLD << std::setw(wide) << *it << RESET << VBAR;
    }
    os << BOLD << std::setw(wide) << verts.back() << RESET << DVBAR << '\n';

    // third line
    os << DVDRIGHT << hdwide << DCROSS;
    string hsep = hdwide;
    for (auto i = 0u; i < wide * (self.dim() - 1) + self.dim() - 1; ++i) {
        if (i % (wide + 1) == 0) {
            hsep += DHSV;
        }
        else {
            hsep += DHBAR;
        }
    }
    hsep += DVDLEFT;
    os << hsep << '\n';

    // string(wide, HBAR)
    string hswide;
    for (auto i = 0u; i < wide; ++i) {
        hswide += HBAR;
    }

    // row lines
    auto vx = (self._vmap | std::views::keys).begin();
    for (auto r = 0u; r < self.dim(); ++r) {
        // vertex label
        os << DVBAR << BOLD << std::setw(wide) << std::left << *vx++ << RESET
           << DVBAR << std::right;
        // adjacencies
        string val;
        for (auto c = 0u; c < self.dim() - 1; ++c) {
            if (self._inf == self(r, c)) {
                val = string(wide - 1, ' ') + "∞";
            }
            else {
                val = std::to_string(self(r, c));
            }
            if (self(r, c) == 0) {
                os << MAGENTA;
            }
            os << std::setw(wide) << val << RESET << VBAR;
        }
        if (self._inf == self(r, self.dim() - 1)) {
            val = string(wide - 1, ' ') + "∞";
        }
        else {
            val = std::to_string(self(r, self.dim() - 1));
        }
        if (self(r, self.dim() - 1) == 0) {
            os << MAGENTA;
        }

        os << std::setw(wide) << val << RESET << DVBAR << '\n';
        // separator
        if (r != self.dim() - 1) {
            os << DVSRIGHT << hswide << DVSH;
            for (auto c = 0u; c < self.dim() - 1; ++c) {
                os << hswide << CROSS;
            }
            os << hswide << DVSLEFT << '\n';
        }
    }

    // final line
    os << DUPRIGHT << hdwide << DHDUP;
    for (auto i = 0u; i < self.dim() - 1; ++i) {
        os << hdwide << DHUP;
    }
    os << hdwide << DUPLEFT << '\n';

    return os;
}

istream& operator>>(istream& is, adjmat& self)
{
    std::vector<std::vector<int>> vals;

    for (string line; std::getline(is, line);) {
        std::istringstream iss{std::move(line)};
        std::vector<int> linevals;
        for (int i; iss >> i;) {
            linevals.push_back(i);
        }
        vals.push_back(std::move(linevals));
    }

    self = adjmat(vals); // move into
    return is;
}

#ifdef TESTING
#include "doctest.h"

#include <sstream>

TEST_CASE("adjmat")
{
    SUBCASE("operator>>")
    {
        std::istringstream input("1 2 3\n4 5 6\n7 8 9");
        adjmat tmat{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

        adjmat m;
        input >> m;

        CHECK(m == tmat);
    }

    SUBCASE("adjmat::adjmat(edges)")
    {
        std::map<std::pair<int, int>, int> edges{
            {{1, 1}, 1}, {{2, 2}, 1}, {{3, 3}, 1}, {{4, 4}, 1}, {{5, 5}, 1}};

        adjmat tmat{{1, 2, 2, 2, 2},
                    {2, 1, 2, 2, 2},
                    {2, 2, 1, 2, 2},
                    {2, 2, 2, 1, 2},
                    {2, 2, 2, 2, 1}};

        CHECK(adjmat(edges) == tmat);
    }

    SUBCASE("adjmat::adjmat(size_t, int)")
    {
        adjmat tmat{{1, 1, 1}, {1, 1, 1}, {1, 1, 1}};
        CHECK(adjmat(3, 1) == tmat);
    }

    SUBCASE("adjmat::adjmat(vector<vector<int>>)")
    {
        adjmat tmat{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
        std::vector<std::vector<int>> tvec{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
        CHECK(tmat == tvec);
    }

    SUBCASE("adjmat::adjmat(vector<int>, size_t)")
    {
        adjmat tmat{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
        std::vector<int> tvec = {1, 2, 3, 4, 5, 6, 7, 8, 9};
        CHECK(tmat == adjmat(tvec, 3));
    }

    SUBCASE("adjmat::operator()")
    {
        adjmat tmat{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};

        for (auto i = 0u; i < tmat.dim(); ++i) {
            for (auto j = 0u; j < tmat.dim(); ++j) {
                CHECK(tmat(i, j) == i * tmat.dim() + j + 1);
            }
        }
    }

    SUBCASE("adjmat::operator[]")
    {
        std::map<std::pair<int, int>, int> edges{
            {{1, 1}, 1}, {{2, 2}, 1}, {{3, 3}, 1}, {{4, 4}, 1}, {{5, 5}, 1}};
        adjmat tmat(edges);

        for (auto& e : edges) {
            CHECK(tmat[e.first] == e.second);
        }
    }
}

#endif
