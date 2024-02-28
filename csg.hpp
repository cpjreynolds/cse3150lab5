#ifndef CSG_HPP
#define CSG_HPP

#include <map>
#include <iostream>
#include <stdexcept>

namespace csg {

// {v1, v2, w} = v1 -> {v2, w}
std::map<std::pair<int, int>, int> parse(std::istream& is);
std::map<std::pair<int, int>, int> parse(const std::string& fname);

// parse_context owns the current line string.
struct parse_context {
    std::streamsize lineno;
    std::streamsize col;
    std::string line;
    std::string fname;
};

class parse_error : public std::runtime_error {
public:
    parse_error(std::string_view msg, parse_context ctx);

    const parse_context& where() const { return _where; }

private:
    parse_context _where;
};

// keep these in csg namespace because I'm overloading for a stdlib type which
// is technically very poor form.
std::ostream& operator<<(std::ostream&,
                         const std::map<std::pair<int, int>, int>&);
std::ostream& operator<<(std::ostream&, const std::pair<int, int>&);
std::ostream& operator<<(std::ostream&,
                         const std::pair<std::pair<int, int>, int>&);

} // namespace csg

#endif
