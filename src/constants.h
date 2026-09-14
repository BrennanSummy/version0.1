#pragma once
#include <vector>
#include <string>

/* The width and height of the board. The char matrix
    dimensions must be known at compile time. */
inline constexpr size_t bWidth  = 80;
inline constexpr size_t bHeight = 80;
inline constexpr size_t bBWidth  = 2*bWidth -1;
inline constexpr size_t bBHeight = 2*bHeight-1;

const std::string dir = "/home/b_lin/Documents/Coding/barnard-cobden-labs/ads_sim_proj/version0.1/out/";