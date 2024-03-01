#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <map>
#include <set>
#include <vector>
#include <initializer_list>
#include <iostream>

class adjmat {
public:
    adjmat() : _dim{0} {};
    adjmat(const std::vector<int>& data, size_t dim) : _dim{dim}, _data{data}
    {
        if (_data.size() != _dim * _dim) {
            throw std::logic_error("matrix dimension/data mismatch");
        }
    };

    adjmat(size_t dim, const int& val) : _dim{dim}, _data(dim * dim, val){};

    adjmat(std::initializer_list<std::initializer_list<int>> data);
    adjmat(const std::vector<std::vector<int>>& data);
    adjmat(const std::map<std::pair<int, int>, int>& edges);

    // operator() indexes with "true" index pairs corresponding to memory
    int& operator()(const int r, const int c) { return _data.at(r * _dim + c); }

    const int& operator()(const int r, const int c) const
    {
        return _data.at(r * _dim + c);
    }

    // operator[] indexes by mapping vertex labels to memory locations
    int& operator[](const std::pair<size_t, size_t>& idx)
    {
        return operator()(_vmap.at(idx.first), _vmap.at(idx.second));
    }
    const int& operator[](const std::pair<size_t, size_t>& idx) const
    {
        return operator()(_vmap.at(idx.first), _vmap.at(idx.second));
    }

    size_t dim() const { return _dim; }
    // assigns a value to map to infinity
    void infmap(const int& i) { _inf = i; }

    void vmap(const std::map<int, size_t>& nvmap) { _vmap = nvmap; }
    std::map<int, size_t>& vmap() { return _vmap; }
    const std::map<int, size_t>& vmap() const { return _vmap; }

    friend std::istream& operator>>(std::istream&, adjmat&);
    friend std::ostream& operator<<(std::ostream&, const adjmat&);

    friend bool operator==(const adjmat& lhs, const adjmat& rhs)
    {
        return lhs._dim == rhs._dim && lhs._data == rhs._data;
    }

private:
    // returns vertices from an edge set.
    static std::set<int>
    get_vertices(const std::map<std::pair<int, int>, int>& edges);

    static std::map<int, size_t> default_vmap(size_t dim);
    static std::map<int, size_t>
    gen_vmap(const std::map<std::pair<int, int>, int>& edges);

    size_t _dim;
    // maps vertex label to index
    std::map<int, size_t> _vmap = default_vmap(_dim);
    std::vector<int> _data;
    // maps a specific value to infinity for printing
    int _inf = 2;
};

#endif
