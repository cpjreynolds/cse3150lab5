#include "csg.hpp"
#include "tcolor.hpp"

#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <charconv>
#include <system_error>
#include <vector>
#include <map>
#include <fstream>

using std::istream, std::ostringstream;
using std::pair, std::map;
using std::string, std::string_view;

namespace csg {

namespace {

string make_point(parse_context& ctx)
{
    ostringstream output;
    output << "  " << ctx.lineno << " |\t";
    auto offset = std::streamoff(output.tellp()); // cast from fpos to integral
    output << ctx.line << " \n";
    string point(ctx.line.size() + 1, ' ');
    point.at(ctx.col - 1) = '^';
    output << string(offset - 2, ' ') << "|\t" << BOLD << MAGENTA << point
           << RESET << '\n';
    return output.str();
}

string make_error_message(string_view msg, parse_context& ctx)
{
    ostringstream output;
    output << BOLD << RED << "parse error: " << DEFAULT << ctx.fname << ":"
           << ctx.lineno << ":" << ctx.col << ": " << RESET << msg << "\n\n";
    output << make_point(ctx);

    return output.str();
}

// eat/digest_x() functions return the number of characters consumed.

// consumes leading whitespace
size_t eat_space(string_view& s, parse_context& ctx)
{
    if (s.empty()) {
        return 0;
    }
    size_t pos = s.find_first_not_of(" \t");
    pos = pos == s.npos ? s.size() : pos;
    s.remove_prefix(pos);
    ctx.col += pos;
    return pos;
}

// consumes a leading comma
size_t eat_comma(string_view& s, parse_context& ctx)
{
    if (!s.empty() && s.front() == ',') {
        s.remove_prefix(1);
        ctx.col += 1;
        return 1;
    }
    else {
        return 0;
    }
}

int maybe_eat_alt_symbol(string_view& s, parse_context& ctx)
{
    if (s.empty()) {
        return 0;
    }
    char sym = s.front();
    if (sym == '+' || sym == '-') {
        s.remove_prefix(1);
        ctx.col += 1;
        return sym == '+' ? 1 : -1;
    }
    else {
        return 0;
    }
}

// consumes an integer and places it in `value`
size_t digest_integer(string_view& s, parse_context& ctx, int& value)
{
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec == std::errc()) { // success
        ptrdiff_t parselen = ptr - s.data();
        s.remove_prefix(parselen);
        ctx.col += parselen;
        return parselen;
    }
    else if (ec == std::errc::invalid_argument) {
        throw parse_error("expected integer", ctx);
    }
    else if (ec == std::errc::result_out_of_range) {
        throw parse_error("integer too large", ctx);
    }
    else {
        [[unlikely]] throw parse_error("unreachable", ctx);
    }
}

void parse_line(map<pair<int, int>, int>& edges, parse_context& ctx)
{
    // starting a new line.
    ctx.col = 1;
    // first read in a vector of ints, then deal with the semantics.
    std::vector<int> vals;
    string_view line = ctx.line;

    // return early if the line is empty or only whitespace
    if (eat_space(line, ctx) == ctx.line.size()) {
        return;
    }

    while (!line.empty()) {
        eat_space(line, ctx);
        int val;
        digest_integer(line, ctx, val);
        vals.push_back(val);

        if (vals.size() > 2 && (vals.size() - 3) % 2 == 0) {
            size_t i = vals.size() - 1;
            auto [v, ins] = edges.insert({{vals[i - 2], vals[i]}, vals[i - 1]});
            if (!ins) {
                ostringstream output;
                output << "edge " << v->first << " already exists";
                throw parse_error(output.str(), ctx);
            }
        }
        eat_space(line, ctx);
        if ((val = maybe_eat_alt_symbol(line, ctx))) {
            vals.push_back(val);
            // + or - edge, skip comma check
            continue;
        }
        // successfully parsed an int and any following whitespace
        //
        // options at this point are:
        //      * end of line (line.empty())
        //      * comma and another value
        if (eat_comma(line, ctx) && line.empty()) {
            // if we have eaten a comma and the line is empty, that is an error
            throw parse_error("expected integer", ctx);
        }
        // at this point the line is either empty, or pointing to the next
        // integer to parse
    }

    if (vals.empty()) {
        [[unlikely]] throw parse_error("unreachable", ctx);
    }
    else if (vals.size() == 1) {
        throw parse_error("expected edge", ctx);
    }
    else if (vals.size() == 2 || (vals.size() - 3) % 2 != 0) {
        throw parse_error("expected vertex", ctx);
    }
}

map<pair<int, int>, int> parse_with(istream& is, parse_context& ctx)
{
    map<pair<int, int>, int> edges;

    for (string line; std::getline(is, line);) {
        ctx.line = std::move(line); // ctx takes ownership
        ctx.lineno += 1;
        parse_line(edges, ctx);
    }
    return edges;
}

} // namespace

parse_error::parse_error(string_view msg, parse_context ctx)
    : runtime_error{make_error_message(msg, ctx)}, _where{ctx}
{
}

// parse a stream of comma-separated graph values into a mapping of edges to
// weights.
map<pair<int, int>, int> parse(istream& is)
{

    parse_context ctx{0, 1};
    if (&is == &std::cin) {
        ctx.fname = "<stdin>";
    }
    else {
        ctx.fname = "<istream>";
    }
    return parse_with(is, ctx);
}

map<pair<int, int>, int> parse(const std::string& fname)
{
    parse_context ctx{0, 1};
    std::ifstream ifile(fname);
    if (!ifile.is_open()) {
        ostringstream output;
        output << fname << ": no such file";
        throw std::runtime_error(output.str());
    }
    else {
        ctx.fname = fname;
    }
    return parse_with(ifile, ctx);
}

std::ostream& operator<<(std::ostream& os,
                         const pair<pair<int, int>, int>& edge)
{
    return os << "{" << edge.first.first << " ━━❮" << edge.second << "❯━» "
              << edge.first.second << "}";
}

std::ostream& operator<<(std::ostream& os, const pair<int, int>& edge)
{
    return os << "{" << edge.first << " ━━» " << edge.second << "}";
}

std::ostream& operator<<(std::ostream& os,
                         const map<pair<int, int>, int>& edges)
{
    for (auto edge : edges) {
        os << edge << std::endl;
    }
    return os;
}

} // namespace csg

#ifdef TESTING
#include "doctest.h"

TEST_CASE("csg::parse(istream&)")
{
    std::istringstream in1("1+2-3+4\n1-4");
    std::istringstream in2("1,1,2,-1,3,1,4\n1,-1,4");

    std::map<std::pair<int, int>, int> edges{
        {{1, 2}, 1}, {{2, 3}, -1}, {{3, 4}, 1}, {{1, 4}, -1}};

    auto t1 = csg::parse(in1);
    auto t2 = csg::parse(in2);

    CHECK(t1 == edges);
    CHECK(t2 == edges);
}

#endif
