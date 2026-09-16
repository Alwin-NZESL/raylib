#pragma once

#include <vector>
#include <stdexcept>
#include <ostream>
#include <istream>

template<typename T>
class Grid
{
public:
    Grid(size_t width, size_t height) : width(width), height(height), data(width * height) {}

    T& cell(int x, int y) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            throw std::out_of_range("Cell coordinates out of bounds");
        return data[y * width + x];
    }

    const T& cell(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height)
            throw std::out_of_range("Cell coordinates out of bounds");
        return data[y * width + x];
    }

    bool valid( int x, int y ) const {
        return (x >= 0 && x < width && y >= 0 && y < height);
    }

    size_t get_width() const { return width; }
    size_t get_height() const { return height; }

    void resize(size_t new_width, size_t new_height) {
        width = new_width;
        height = new_height;
        data.resize(new_width * new_height);
    }

    void clear() {
        std::fill(data.begin(), data.end(), T());
    }

    bool operator==(const Grid<T>& other) const {
        return width == other.width && height == other.height && data == other.data;
    }
    
private:
    size_t width;
    size_t height;
    std::vector<T> data;
};

template <typename T>
inline std::ostream &operator<<( std::ostream& os, const Grid<T>& grid )
{
    os << grid.get_width() << " " << grid.get_height() << "\n";

    for (size_t  y = 0; y < grid.get_height(); ++y) {
        for (size_t  x = 0; x < grid.get_width(); ++x)
            os << grid.cell(x, y) << " ";

        os << "\n";
    }

    return os;
}

template<typename T>
inline std::istream& operator>>( std::istream& is, Grid<T>& grid )
{
    grid.clear();

    size_t width, height;
    
    is >> width >> height;

    // Resize the grid to the correct dimensions
    grid.resize(width, height);

    for (size_t  y = 0; y < height; ++y) {
        for (size_t  x = 0; x < width; ++x)
            is >> grid.cell(x, y);
    }

    return is;
}
