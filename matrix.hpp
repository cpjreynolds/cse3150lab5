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
    adjmat(const std::vector<int>& data, size_t dim)
        : _dim{dim}, _data{data} {};

    adjmat(size_t dim, const int& val) : _dim{dim}, _data(dim * dim, val){};

    adjmat(std::initializer_list<std::initializer_list<int>> data);
    adjmat(const std::vector<std::vector<int>>& data);
    adjmat(const std::map<std::pair<int, int>, int>& edges);

    // operator() indexes by mapping vertex labels to memory locations
    int& operator()(const int rv, const int cv)
    {
        return operator[]({_vmap.at(rv), _vmap.at(cv)});
    }

    const int& operator()(const int rv, const int cv) const
    {
        return operator[]({_vmap.at(rv), _vmap.at(cv)});
    }

    // operator[] indexes with "true" index pairs corresponding to memory
    int& operator[](const std::pair<size_t, size_t>& idx)
    {
        return _data.at(idx.first * _dim + idx.second);
    }
    const int& operator[](const std::pair<size_t, size_t>& idx) const
    {
        return _data.at(idx.first * _dim + idx.second);
    }

    size_t dim() const { return _dim; }
    // assigns a value to map to infinity
    void map_inf(const int& i) { _inf = i; }

    friend std::istream& operator>>(std::istream&, adjmat&);
    friend std::ostream& operator<<(std::ostream&, const adjmat&);

private:
    // returns vertices from an edge set.
    static std::set<int>
    get_vertices(const std::map<std::pair<int, int>, int>& edges);

    static std::map<int, size_t> default_vmap(size_t dim);
    static std::map<int, size_t>
    gen_vmap(const std::map<std::pair<int, int>, int>& edges);

    // maps vertex label to index
    size_t _dim;
    std::map<int, size_t> _vmap = default_vmap(_dim);
    std::vector<int> _data;
    // maps a specific value to infinity for printing
    int _inf = 2;
};

#endif
