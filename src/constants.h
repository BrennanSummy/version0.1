#pragma once
#include <vector>
#include <string>

/* The width and height of the board. The char matrix
    dimensions must be known at compile time. */
inline constexpr size_t bWidth  = 200;
inline constexpr size_t bHeight = 200;
inline constexpr size_t bBWidth  = 2*bWidth -1;
inline constexpr size_t bBHeight = 2*bHeight-1;

const std::string dir = "/home/b_lin/Documents/Coding/barnard-cobden-labs/ads_sim_proj/version0.1/out/";

// Number of pixels to randomly update per step
const int stepSizeInPixelUpdates = 2000;

// Number of steps to do between boundary length scans / information gathering
const int scanBoundsPeriodInSteps = 100;

// Number of reads to do between screenshots
const int grabScreenPeriodInReads = 10000;

// Number of reads to take per file/parameter set
const int readsPerFile = 2000;