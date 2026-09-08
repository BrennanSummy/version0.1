#pragma once
#include <vector>

/* The width and height of the board. The char matrix
    dimensions must be known at compile time. */
inline constexpr size_t width  = 100;
inline constexpr size_t height = 100;
inline constexpr size_t bBWidth  = 2*width -1;
inline constexpr size_t bBHeight = 2*height-1;